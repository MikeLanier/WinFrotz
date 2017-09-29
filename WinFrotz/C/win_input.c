/*
 * win_inpt.c
 *
 * Visual C interface, input functions (Windows)
 *
 */
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <crtdbg.h>
#include "../frotz/frotz.h"
#include "..\include\logfile_c.h"
#include "../include/windefs.h"
#ifndef	_CONSOLE
#include "../include/crosscomp.h"
#endif

#ifndef HISTORY_BUFSIZE
#define HISTORY_BUFSIZE 500
#endif

extern bool is_terminator (zchar);

extern bool read_yes_or_no (const char *);
extern void read_string (int, zchar *);

extern int completion (const zchar *, zchar *);

static int globaltimeout;

static unsigned long limit = 0;

static struct {
    zchar buffer[HISTORY_BUFSIZE];
    int latest;
    int current;
    int prefix_len;
} history;

static struct {
    zchar *buffer;
#if defined(WINFROTZ) || defined(_CONSOLE)
	zchar last_buffer[ INPUT_BUFFER_SIZE ];
	int	last_pos;
#endif
    int pos;
    int length;
    int max_length;
    int width;
    int max_width;
} input;

static bool overwrite = 0;

int show_cursor = 0;

int	start_buffer_point = 0;

static void input_edit_undo( void );
static void input_font_change( int nPrevState );

extern void	cleanup( void );

#ifdef BUFFERED_TEXTOUT
extern void clear_output_buffer( void );
#ifndef CONSOLE
extern BOOL	bRefresh;
#endif
#else
#define clear_output_buffer() {};
#endif

/*	To make the code maintenance easier even the windowed version carries	
	character entry around in an INPUT_RECORD structure; even those this is
	designed for consoles. bCtrl and bAlt hold those keys states when the
	current character was typed.											*/
static INPUT_RECORD	InKey;
BOOL				bCtrl, bAlt;

#ifdef _CONSOLE
extern	HWND	hWnd;
extern	BOOL	bUseFileDialog;

extern	int		cTranslate[];

extern	HANDLE	hConsoleOutput, hConsoleInput;
extern	DWORD	cursorHeight;
extern	CONSOLE_CURSOR_INFO			cursorInfo;

#else
extern char szBigBuffer[];
extern int	bbpos;
extern char *ScrollRoll[];
extern int	nScrollBuffs;

int			kbBuffer[KEYBOARD_BUFFER_SIZE + 1];
char		kbModals[KEYBOARD_BUFFER_SIZE + 1];
int			kbIndex = 0;

extern	int				zcodeops;
extern	unsigned long	zcodestart, zcodetime;

extern	HWND	hWnd;
extern	HDC		hdc, h_memDC;
extern	int		nState;

extern	HANDLE	hThread;
extern long		nScrollPos;
extern fontprops	fpFonts[NUM_FONT_TYPES];
#endif

/*	Custom filters for file types (user may add his own during usage		*/
static char strCustomFilter[CUSTOM_FILTER_SIZE];

#ifdef _CONSOLE
extern COORD				curCoord;
#else
extern fakecoord			curCoord;
#endif

extern int current_style;

extern int	*current_font_width;
extern int	current_font_height;

extern void	resetScrollPos( void );

static UINT APIENTRY OFNHookProc( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );
BOOL CenterWindow( HWND hwndChild, HWND hwndParent);

#ifndef _CONSOLE
void AdjustScrollBack( void )
{
	//LOG_ENTER
	if( bbpos > 30000 )
	{
		int	halfmark = 16000, newsize;
		szBigBuffer[bbpos] = 0;
		while( szBigBuffer[halfmark++]!=13  );
		while( szBigBuffer[halfmark]==13 || szBigBuffer[halfmark]==10 )
			halfmark++;
		newsize = bbpos - halfmark;
		if( nScrollBuffs < 100 )
		{
			if( (ScrollRoll[ nScrollBuffs ] = malloc( halfmark + 1)) == NULL )
				os_fatal("Memory allocation failure for scrollback buffers" );
			memcpy( ScrollRoll[ nScrollBuffs ], szBigBuffer, halfmark );
			ScrollRoll[ nScrollBuffs ][halfmark]=0;
			nScrollBuffs++;
		}
		strcpy( szBigBuffer, &szBigBuffer[halfmark] );
		bbpos = strlen( szBigBuffer );
	}
	//LOG_EXIT
}
#endif

static void switch_cursor (bool cursor)
{
	//LOG_ENTER
	if( cursor )
	{
		show_cursor = TRUE;	
#ifdef _CONSOLE
		if( cursorHeight )
			cursorInfo.bVisible = TRUE;
		SetConsoleCursorInfo( hConsoleOutput, &cursorInfo );
#endif
	}
	else
	{
		show_cursor = FALSE;
#ifdef _CONSOLE
		cursorInfo.bVisible = FALSE;
		SetConsoleCursorInfo( hConsoleOutput, &cursorInfo );
#endif
	}
		
	//LOG_EXIT
}/* switch_cursor */

/*
 * set_timer
 *
 * Set a time limit of timeout/10 seconds if timeout is not zero;
 * otherwise clear the time limit.
 *
 */

static void set_timer (int timeout)
{
	//LOG_ENTER
	if( timeout != INFINITE && timeout!=0 )
	{
		limit = timeGetTime() + timeout * 100;
		globaltimeout = timeout * 100;
	}
	else
		limit = 0;

	//LOG_EXIT
}/* set_timer */

/*
 * time_limit_hit
 *
 * Return true if a previously set time limit has been exceeded.
 *
 */

static bool out_of_time (void)
{
	//LOG_ENTER

    if (limit != 0) 
	{
	//LOG_EXIT
		return timeGetTime() >= limit;

    } else 
		{
	//LOG_EXIT
			return FALSE;
	}

}/* out_of_time */

#if defined( WINFROTZ ) && !defined( _CONSOLE )
static void CheckValidSingleKey( void )
{
	int i;

	//LOG_ENTER
	for( i=0; i < kbIndex; i++ )
	{
		if( kbModals[i] == ALT_KEY_PRESSED || kbModals[i] == CTRL_KEY_PRESSED )
		{
			if( i )
			{
				memcpy( &kbModals[i], &kbModals[i+1], i - 1);
				memcpy( &kbBuffer[i], &kbBuffer[i+1], sizeof( int ) * (i - 1) );
			}
			kbIndex--;
			continue;
		}
		if( (kbBuffer[i] >= SPECIAL_KEY_MIN && kbBuffer[i] <= SPECIAL_KEY_MAX) || kbBuffer[i]==27 )
		{
			if( i )
			{
				memcpy( &kbModals[i], &kbModals[i+1], i - 1);
				memcpy( &kbBuffer[i], &kbBuffer[i+1], sizeof( int ) * (i - 1) );
			}
			kbIndex--;
			continue;
		}
	}
	//LOG_EXIT
}
#endif

/*
 * get_key
 *
 * Read a keypress or a mouse click. Returns...
 *
 *	ZC_TIME_OUT = time limit exceeded,
 *	ZC_BACKSPACE = the backspace key,
 *	ZC_RETURN = the return key,
 *	ZC_HKEY_MIN...ZC_HKEY_MAX = a hot key,
 *	ZC_ESCAPE = the escape key,
 *	ZC_ASCII_MIN...ZC_ASCII_MAX = ASCII character,
 *	ZC_ARROW_MIN...ZC_ARROW_MAX = an arrow key,
 *	ZC_FKEY_MIN...ZC_FKEY_MAX = a function key,
 *	ZC_NUMPAD_MIN...ZC_NUMPAD_MAX = a number pad key,
 *	ZC_SINGLE_CLICK = single mouse click,
 *	ZC_DOUBLE_CLICK = double mouse click,
 *	ZC_LATIN1_MIN+1...ZC_LATIN1_MAX = ISO Latin-1 character,
 *	SPECIAL_KEY_MIN...SPECIAL_KEY_MAX = a special editing key.
 *
 */

static WORD get_key( BOOL cursor )
{
	int	c;
#ifndef _CONSOLE
	int	nPrevState = nState;
	

	//LOG_ENTER
	bAlt = bCtrl = FALSE;

	/* This is the one place it's good to go into TRANSPARENT mode, because some characters overlap somewhat
	   and it looks unnatural when they clip on the input line. Otherwise we want OPAQUE; for instance, 
	   REVERSE mode doesn't even work without OPAQUE													*/
	SetBkMode( h_memDC, TRANSPARENT );
	clear_output_buffer(); 
	SetBkMode( h_memDC, OPAQUE );
	if( !kbIndex )
	{
		switch_cursor( cursor );
		while( !kbIndex && nState!=TIME_ELAPSE )
		{
			if( !input.pos || bRefresh )
			{
				InvalidateRect( hWnd, NULL, FALSE );
				bRefresh = FALSE;
			}
			else
			{
				BITBLT( hdc, 0, curCoord.Y, h_screen_width, curCoord.Y + current_font_height, h_memDC, 0, curCoord.Y + nScrollPos, SRCCOPY );
			}
			if( input.pos )
				PostMessage( hWnd, WM_CURSOR_POS, input.buffer[ input.pos ], (LPARAM)input.buffer );
			else
				PostMessage( hWnd, WM_CURSOR_POS, 'X', (LPARAM)input.buffer );

			zcodetime += timeGetTime() - zcodestart;

#ifndef _CONSOLE
			if( limit )
			{
				if( timeGetTime() > limit )
					nState = TIME_ELAPSE;
				else
				{
					InvalidateRect( hWnd, NULL, FALSE );
					SetTimer(hWnd, 1, globaltimeout, NULL );
				}
			}
#endif
	
			if( nState != TIME_ELAPSE )
			{
				PostMessage( hWnd, WM_BUFFER_READY, WM_BUFFER_READY, (LPARAM)0L );

				/*	When the Z-machine code is waiting for input, it just goes to
					sleep. The Windows side will wake it up again when a timer
					expires, it has a key to work with, or some other state		*/
				SuspendThread( hThread );
			}

			if( limit )
				KillTimer( hWnd, 1 );

			switch( nState )
			{
				case	TIME_ELAPSE:
				{
					if( !limit ) /* This should only happen when we flub a timer exchange		*/
					{
						DEBUGBOX( hWnd, "Timer elapsed but timeout is INFINITE!", "Debug", MB_OK );
						nState = nPrevState;
					}
					else
					{
						nState = BUSY;
						zcodestart = timeGetTime();
						limit = 0;
						return (int)ZC_TIME_OUT;
					}
					break;
				}
			
				/* User quit/closed so cleanup (exit will be called from cleanup) */
				case	SIGNAL_FINISHED:	
					cleanup( );
					break;

				/*	User changed display font. Select the new font, reprint the
					current input line, and go back to what we were doing before*/
				case	CHANGE_FONT:
				{
					input_font_change( nPrevState );
					nState = nPrevState;
					break;
				}

				case	UNDO_EDIT:
				{
					input_edit_undo();
					nState = nPrevState;
					break;
				}

				case	OS_READ:
					break;

				case	WAIT_FOR_KEY:
				{
					CheckValidSingleKey( );
					if( !kbIndex )
						nState = nPrevState;
				}
				break;

				default:
				{
					DEBUGBOX( hWnd, "Unhandled case in get_key returned!", "Debug", MB_OK );
				}
				break;
			}
		}
	}

	zcodestart = timeGetTime();

	nState = BUSY;	

	if( kbModals[0] & ALT_KEY_PRESSED )
		bAlt = TRUE;
	else if( kbModals[0] & CTRL_KEY_PRESSED )
		bCtrl = TRUE;

	memcpy( &kbModals[0], &kbModals[1], kbIndex - 1 );
	kbModals[kbIndex] = 0;
	c = kbBuffer[0];
	memcpy( &kbBuffer[0], &kbBuffer[1], --kbIndex * sizeof( int ) );
#else
	BOOL bSuccess;
	DWORD NumberOfEventsRead = 1;
	DWORD dwTimeout;
	char kbBuffer[2];

	//LOG_ENTER
	if( limit )
		dwTimeout = limit - timeGetTime();
	else
		dwTimeout = INFINITE;

	clear_output_buffer(); 

	kbBuffer[1] = 0;

	bSuccess = FALSE;
	/* Put the console into wait mode and exit on a keyboard event */
	/* Throw away everything but keyboard events and mouse clicks  */
	while( !bSuccess )
	{
 		if( (WaitForSingleObject( hConsoleInput, dwTimeout))==WAIT_TIMEOUT )
			return 0; /* Return special time out character */
		else
		{
			ReadConsoleInput(hConsoleInput, &InKey, 1, &NumberOfEventsRead );
			/* Look for a key event, with the key going down (meaning we ignore key up) and
			   ignore the mode keys (shift ctrl etc) since they don't generate an actual key */
			if( InKey.EventType == KEY_EVENT && InKey.Event.KeyEvent.bKeyDown==TRUE  &&
				(InKey.Event.KeyEvent.wVirtualKeyCode < 16 || InKey.Event.KeyEvent.wVirtualKeyCode > 18))
				bSuccess = TRUE;
			if( InKey.EventType == MOUSE_EVENT && InKey.Event.MouseEvent.dwButtonState )
			{
				mouse_x = InKey.Event.MouseEvent.dwMousePosition.X + 1;
				mouse_y = InKey.Event.MouseEvent.dwMousePosition.Y + 1;
				bSuccess = TRUE;
			}
		}
	}
	bCtrl = (InKey.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED) || (InKey.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED);
	bAlt = (InKey.Event.KeyEvent.dwControlKeyState & LEFT_ALT_PRESSED) || (InKey.Event.KeyEvent.dwControlKeyState & RIGHT_ALT_PRESSED );

	c = (unsigned char)InKey.Event.KeyEvent.wVirtualKeyCode;

	if( !bAlt && !bCtrl )
	{
		if( InKey.EventType == MOUSE_EVENT )
			return ZC_SINGLE_CLICK;

		if( cTranslate[ c ] )
			return( (WORD)cTranslate[c]);

		if( !InKey.Event.KeyEvent.uChar.AsciiChar )
			return c;

		c = kbBuffer[0] = InKey.Event.KeyEvent.uChar.AsciiChar;
	}
#endif

	if( bAlt )
	{
		bAlt = FALSE;
		switch( c )
		{
			case	'A':			
			case	'a':
				return ZC_HKEY_ALIAS;

			case	'B':
			case	'b':
				return SPECIAL_KEY_WORD_LEFT;

			case	'D':			
			case	'd':
				return ZC_HKEY_DEBUG;

			case	'F':
			case	'f':
				return SPECIAL_KEY_WORD_RIGHT;
			
			case	'N':			
			case	'n':
				return ZC_HKEY_RESTART;

			case	'P':			
			case	'p':
				return ZC_HKEY_PLAYBACK;

			case	'R':			
			case	'r':
				return ZC_HKEY_RECORD;

			case	'S':			
			case	's':
				return ZC_HKEY_SEED;
			
			case	'U':			
			case	'u':
				return ZC_HKEY_UNDO;

			case	'X':
			case	'x':
				return ZC_HKEY_QUIT;

			default:
				bAlt = TRUE;
				break;
		}
	}

	if( bCtrl )
	{
		bCtrl = FALSE;
		switch( c )
		{
			case	1:			/* CTRL-A, move to beginning of line */
			case	'A':
				return SPECIAL_KEY_HOME;

			case	2:			/* CTRL-B, cursor left */
			case	'B':
				return 131;

			case	4:			/* CTRL-D delete character below cursor */
			case	'D':
				return SPECIAL_KEY_DELETE;

			case	5:			/* CTRL-E, move to end of line */
			case	'E':
				return SPECIAL_KEY_END;

			case	6:			/* CTRL-F cursor right */
			case	'F':
				return 132;

			case	'H':
				return 8;

			case	14:			/* Ctrl-N get next command */
			case	'N':
				return 130;

			case	16:			/* Ctrl-P get previous command */
			case	'P':
				return 129;

			case	'T':
				return SPECIAL_KEY_TRANSPOSE;

			case	21:			/* CTRL-U delete whole input line (EMACS mode from OS/2 code) */
			case	'U':
				return 27;

			case	37:			/* CTRL Left-arrow */
				return SPECIAL_KEY_WORD_LEFT;
			
			case	39:			/* CTRL Right-arrow */
				return SPECIAL_KEY_WORD_RIGHT;
	
			case	46:			
				return SPECIAL_KEY_DELETE_WORD;

			case	8:
			case	127:
				return SPECIAL_KEY_DELETE_LEFT;

			default:
				bCtrl = TRUE;
				break;
		}
	}
	//LOG_EXIT
	return c;
}/* get_key */

/*
 * cursor_left
 *
 * Move the cursor one character to the left.
 *
 */

static void cursor_left (void)
{
	//LOG_ENTER

	if (input.pos > 0)
	{
		curCoord.X -= CURRENT_FONT_WIDTH( (unsigned char) input.buffer[--input.pos] );
#ifdef _CONSOLE
		input.pos--;
#endif
	}

	//LOG_EXIT
}/* cursor_left */

/*
 * cursor_right
 *
 * Move the cursor one character to the right.
 *
 */

static void cursor_right (void)
{
	//LOG_ENTER

    if (input.pos < input.length)
	{
		curCoord.X += CURRENT_FONT_WIDTH( (unsigned char) input.buffer[input.pos++] );
#ifdef _CONSOLE
		input.pos++;
#endif
	}
	//LOG_EXIT
}/* cursor_right */

/*
 * first_char
 *
 * Move the cursor to the beginning of the input line.
 *
 */

static void first_char (void)
{
	//LOG_ENTER

    while (input.pos > 0)
	cursor_left ();

	//LOG_EXIT
}/* first_char */

/*
 * last_char
 *
 * Move the cursor to the end of the input line.
 *
 */

static void last_char (void)
{
	//LOG_ENTER

    while (input.pos < input.length)
	cursor_right ();

	//LOG_EXIT
}/* last_char */

/*
 * prev_word
 *
 * Move the cursor to the start of the previous word.
 *
 */

static void prev_word (void)
{
	//LOG_ENTER

    do {

	cursor_left ();

	if (input.pos == 0)
	    return;

    } while (input.buffer[input.pos] == ' ' || input.buffer[input.pos - 1] != ' ');

	//LOG_EXIT
}/* prev_word */

/*
 * next_word
 *
 * Move the cursor to the start of the next word.
 *
 */

static void next_word (void)
{
	//LOG_ENTER

    do {

	cursor_right ();

	if (input.pos == input.length)
	    return;

    } while (input.buffer[input.pos] == ' ' || input.buffer[input.pos - 1] != ' ');

	//LOG_EXIT
}/* next_word */

/*
 * input_move
 *
 * Helper function to move parts of the input buffer:
 *
 *    newc != 0, oldc == 0: INSERT
 *    newc != 0, oldc != 0: OVERWRITE
 *    newc == 0, oldc != 0: DELETE
 *    newc == 0, oldc == 0: NO OPERATION
 *
 */

#define H(x) (x ? 1 : 0)

static void input_move (zchar newc, zchar oldc)
{
    int newwidth = (newc != 0) ? os_char_width (newc) : 0;
    int oldwidth = (oldc != 0) ? os_char_width (oldc) : 0;

    zchar *p = input.buffer + input.pos;

    int saved_x = curCoord.X;

    int updated_width = input.width + newwidth - oldwidth;
    int updated_length = input.length + H (newc) - H (oldc);

	//LOG_ENTER
    if (updated_width > input.max_width)
	return;
    if (updated_length > input.max_length)
	return;

    input.width = updated_width;
    input.length = updated_length;

    if (oldc != 0 && newc == 0)
	memmove (p, p + 1, updated_length - input.pos + 1);
    if (newc != 0 && oldc == 0)
	memmove (p + 1, p, updated_length - input.pos);

    if (newc != 0)
	*p = newc;

	/* Have to erase the old string if in Windowed mode, because printing is always
	   in TRANSPARENT for the DC to allow italics and the like to work, thus this 
	   would print over top of the old text											*/

#ifndef _CONSOLE
	os_erase_area (
		curCoord.Y + 1,
		saved_x + 1,
		curCoord.Y + current_font_height,
		saved_x + os_string_width(p) );
#endif
    os_display_string (p);

/*    switch_scrn_attr (TRUE);*/

    if (oldwidth > newwidth)

	os_erase_area (
	    curCoord.Y + 1,
	    curCoord.X + 1,
	    curCoord.Y + current_font_height,
	    curCoord.X + oldwidth - newwidth + 1);

/*    switch_scrn_attr (FALSE); */

    curCoord.X = saved_x;

#ifdef _CONSOLE
	SetConsoleCursorPosition( hConsoleOutput, curCoord );
#endif

    if (newc != 0)
	cursor_right ();

	//LOG_EXIT
}/* input_move */

#undef H

/*
 * delete_char
 *
 * Delete the character below the cursor.
 *
 */

static void delete_char (void)
{
	//LOG_ENTER

    input_move (0, input.buffer[input.pos]);
	clear_output_buffer();

	//LOG_EXIT
}/* delete_char */

/*
 * delete_left
 *
 * Delete the character to the left of the cursor.
 *
 */

static void delete_left (void)
{
	//LOG_ENTER

    if (input.pos > 0) {
	cursor_left ();
	delete_char ();
    }

	//LOG_EXIT
}/* delete_left */

/*
 * truncate_line
 *
 * Truncate the input line to n characters.
 *
 */

static void truncate_line (int n)
{
	//LOG_ENTER

    last_char ();

    while (input.length > n)
	delete_left ();

	//LOG_EXIT
}/* truncate_line */

/*
 * insert_char
 *
 * Insert a character into the input buffer.
 *
 */

static void insert_char (zchar newc)
{
    zchar oldc = 0;

	//LOG_ENTER
    if (overwrite)
	oldc = input.buffer[input.pos];

    input_move (newc, oldc);

	//LOG_EXIT
}/* insert_char */

/*
 * insert_string
 *
 * Add a string of characters to the input line.
 *
 */

static void insert_string (const zchar *s)
{
	//LOG_ENTER

    while (*s != 0) {

	if (input.length + 1 > input.max_length)
	    break;
	if (input.width + os_char_width (*s) > input.max_width)
	    break;

	insert_char (*s++);

    }

	//LOG_EXIT
}/* insert_string */

/*
 * erase_input
 *
 * Clear the input line.
 *
 */

static void erase_input (void)
{
	//LOG_ENTER
	strcpy( input.last_buffer, input.buffer );
	input.last_pos = input.pos;
    last_char ();

    while (input.pos)
	delete_left ();

	//LOG_EXIT
}/* erase_input */

static void transpose_char( void )
{
	//LOG_ENTER
	if( input.pos != input.length )
	{
		char c1 = input.buffer[ input.pos ];
		char c2 = input.buffer[ input.pos + 1 ];

		strcpy( input.last_buffer, input.buffer );
		input.last_pos = input.pos;
		cursor_right();
		cursor_right();
		delete_left ();
		delete_left ();
		insert_char( c2 );
		insert_char( c1 );
	}
	//LOG_EXIT
}

static void delete_word_left( void )
{
	//LOG_ENTER
	strcpy( input.last_buffer, input.buffer );
	input.last_pos = input.pos;
	while( input.pos && input.buffer[input.pos - 1]==' ' )
		delete_left();
	while( input.pos && input.buffer[input.pos - 1]!=' ' )
		delete_left();
	//LOG_EXIT
}

/*	Delete the word currently under the cursor. Note that the cursor could be anywhere    */
/*  from the first to the last character of the word, or in white space to the left of it */
static void delete_word( void )
{
	//LOG_ENTER
	strcpy( input.last_buffer, input.buffer );
	input.last_pos = input.pos;

	/* If we're on white space, delete the white space until the next non-whitespace */
	/* character and then return													 */
	if( input.buffer[ input.pos ] == ' ' )
	{
		while( input.pos < input.length && input.buffer[ input.pos ] == ' ' )
			delete_char();
		return;
	}

	while( input.pos < input.length && input.buffer[ input.pos ] != ' ' )
		delete_char();
	
	if( input.pos < input.length && input.buffer[ input.pos ] )
	{
		while( input.buffer[ input.pos ] == ' ' && input.pos >= input.length )
			delete_char();
		return;
	}
	//LOG_EXIT
}

#if defined( WINFROTZ ) && !defined( _CONSOLE )
static void input_font_change( int nPrevState )
{
	//LOG_ENTER
	os_set_text_style( current_style );
	if( nPrevState == OS_READ )
	{
		os_erase_area( curCoord.Y + 1, 1, h_screen_height + 1, h_screen_width+1 );
		curCoord.X = 0;
		os_display_string( ">" );
		os_display_string( input.buffer );
		clear_output_buffer();
		InvalidateRect( hWnd, NULL, FALSE );
	}
	//LOG_EXIT
}

static void input_edit_undo( void )
{
	//LOG_ENTER
	if( input.last_buffer[0] )
	{
		char saved_input[ INPUT_BUFFER_SIZE ];
		int	saved_input_pointer = input.last_pos;

		strcpy( saved_input, input.last_buffer );
		os_erase_area( curCoord.Y + 1, 1, h_screen_height + 1, h_screen_width+1 );
		erase_input();
		curCoord.X = 0;
		os_display_string( ">" );
		clear_output_buffer();
		insert_string( saved_input );
		first_char();
		while( saved_input_pointer-- > 0 )
			cursor_right();
		input.last_buffer[0] = 0;
		clear_output_buffer();
		InvalidateRect( hWnd, NULL, FALSE );
	}
	//LOG_EXIT
}
#endif

/*
 * tabulator_key
 *
 * Complete the word at the end of the input line, if possible.
 *
 */

static void tabulator_key (void)
{
    int status;

	//LOG_ENTER
    if (input.pos == input.length) {

	zchar extension[10];

	status = completion (input.buffer, extension);
	insert_string (extension);

    } else status = 2;

    /* Beep if the completion was impossible or ambiguous */

    if (status != 0)
	os_beep (status);

	//LOG_EXIT
}/* tabulator_key */

/*
 * store_input
 *
 * Copy the current input line to the history buffer.
 *
 */

static void store_input (void)
{
	//LOG_ENTER

    if (input.length >= HISTORY_MIN_ENTRY) {

	const zchar *ptr = input.buffer;

	do {

	    if (history.latest++ == HISTORY_BUFSIZE - 1)
		history.latest = 0;

	    history.buffer[history.latest] = *ptr;

	} while (*ptr++ != 0);

    }

	//LOG_EXIT
}/* store_input */

/*
 * fetch_entry
 *
 * Copy the current history entry to the input buffer and check if it
 * matches the prefix in the input buffer.
 *
 */

static bool fetch_entry (zchar *buf, int entry)
{
    int i = 0;

    zchar c;

	//LOG_ENTER
    do {

	if (entry++ == HISTORY_BUFSIZE - 1)
	    entry = 0;

	c = history.buffer[entry];

	if (i < history.prefix_len && input.buffer[i] != c)
	    return FALSE;

	buf[i++] = c;

    } while (c != 0);

	//LOG_EXIT
    return (i > history.prefix_len) && (i > 1);

}/* fetch_entry */

/*
 * get_prev_entry
 *
 * Copy the previous history entry to the input buffer.
 *
 */

static void get_prev_entry (void)
{
    zchar buf[INPUT_BUFFER_SIZE];

    int i = history.current;

	//LOG_ENTER
    do {

	do {

	    if (i-- == 0)
		i = HISTORY_BUFSIZE - 1;

	    if (i == history.latest)
		return;

	} while (history.buffer[i] != 0);

    } while (!fetch_entry (buf, i));

    truncate_line (history.prefix_len);

    insert_string (buf + history.prefix_len);

    history.current = i;

	//LOG_EXIT
}/* get_prev_entry */

/*
 * get_next_entry
 *
 * Copy the next history entry to the input buffer.
 *
 */

static void get_next_entry (void)
{
    zchar buf[INPUT_BUFFER_SIZE];

    int i = history.current;

	//LOG_ENTER
    truncate_line (history.prefix_len);

    do {

	do {

	    if (i == history.latest)
		return;

	    if (i++ == HISTORY_BUFSIZE - 1)
		i = 0;

	} while (history.buffer[i] != 0);

	if (i == history.latest)
	    goto no_further;

    } while (!fetch_entry (buf, i));

    insert_string (buf + history.prefix_len);

no_further:

    history.current = i;

	//LOG_EXIT
}/* get_next_entry */

/*
 * os_read_line
 *
 * Read a line of input from the keyboard into a buffer. The buffer
 * may already be primed with some text. In this case, the "initial"
 * text is already displayed on the screen. After the input action
 * is complete, the function returns with the terminating key value.
 * The length of the input should not exceed "max" characters plus
 * an extra 0 terminator.
 *
 * Terminating keys are the return key (13) and all function keys
 * (see the Specification of the Z-machine) which are accepted by
 * the is_terminator function. Mouse clicks behave like function
 * keys except that the mouse position is stored in global variables
 * "mouse_x" and "mouse_y" (top left coordinates are (1,1)).
 *
 * Furthermore, Frotz introduces some special terminating keys:
 *
 *     ZC_HKEY_PLAYBACK (Alt-P)
 *     ZC_HKEY_RECORD (Alt-R)
 *     ZC_HKEY_SEED (Alt-S)
 *     ZC_HKEY_UNDO (Alt-U)
 *     ZC_HKEY_RESTART (Alt-N, "new game")
 *     ZC_HKEY_QUIT (Alt-X, "exit game")
 *     ZC_HKEY_DEBUGGING (Alt-D)
 *     ZC_HKEY_HELP (Alt-H)
 *
 * If the timeout argument is not zero, the input gets interrupted
 * after timeout/10 seconds (and the return value is 0).
 *
 * The complete input line including the cursor must fit in "width"
 * screen units.
 *
 * The function may be called once again to continue after timeouts,
 * misplaced mouse clicks or hot keys. In this case the "continued"
 * flag will be set. This information can be useful if the interface
 * implements input line history.
 *
 * The screen is not scrolled after the return key was pressed. The
 * cursor is at the end of the input line when the function returns.
 *
 * Since Inform 2.2 the helper function "completion" can be called
 * to implement word completion (similar to tcsh under Unix).
 *
 */

#define new_history_search() \
    { history.prefix_len = input.pos; history.current = history.latest; }

zchar os_read_line (int max, zchar *buf, int timeout, int width, int continued)
{
    int key = continued ? 9999 : 0;

	//LOG_ENTER
    /* Initialise input variables */

    input.buffer = buf;
    input.pos = strlen ((char *) buf);
    input.length = strlen ((char *) buf);
    input.max_length = max;
    input.width = os_string_width (buf);
    input.max_width = width - os_char_width (' ');

    /* Calculate time limit */

    set_timer (timeout);

    /* Loop until a terminator is found */
#ifndef _CONSOLE
	AdjustScrollBack( );
	start_buffer_point = bbpos;
	SetBkMode( h_memDC, OPAQUE );
    do {
		
		nState = OS_READ;
#else
	do {
		os_set_cursor( curCoord.Y + 1, curCoord.X + 1);
#endif

	if (key != 9999)
	    new_history_search ();

	/* Get next key from mouse or keyboard */

	key = get_key (TRUE);

	if (key < ZC_ASCII_MIN || key > ZC_ASCII_MAX && key < ZC_LATIN1_MIN || key > ZC_LATIN1_MAX) {

	    /* Ignore time-outs if the cursor is not at end of the line */

	    if (key == ZC_TIME_OUT && input.pos < input.length)
		key = 9999;

	    /* Backspace, return and escape keys */

	    if (key == ZC_BACKSPACE)
		delete_left ();
	    if (key == ZC_RETURN)
		store_input ();
	    if (key == ZC_ESCAPE)
		truncate_line (0);

	    /* Editing keys */

	    if (cwin == 0) {

		if (key == ZC_ARROW_UP)
		    get_prev_entry ();
		if (key == ZC_ARROW_DOWN)
		    get_next_entry ();
		if (key == ZC_ARROW_LEFT)
		    cursor_left ();
		if (key == ZC_ARROW_RIGHT)
		    cursor_right ();

		if (key >= ZC_ARROW_MIN && key <= ZC_ARROW_MAX)
		    key = 9999;

		if (key == SPECIAL_KEY_HOME)
		    first_char ();
		if (key == SPECIAL_KEY_END)
		    last_char ();
		if (key == SPECIAL_KEY_WORD_LEFT)
		    prev_word ();
		if (key == SPECIAL_KEY_WORD_RIGHT)
		    next_word ();
		if (key == SPECIAL_KEY_DELETE)
		{
			strcpy( input.last_buffer, input.buffer );
			input.last_pos = input.pos;
			delete_char ();
		}
		if (key == SPECIAL_KEY_INSERT)
		{
			overwrite = !overwrite;
#ifndef _CONSOLE
			PostMessage( hWnd, WM_CHANGE_STATUS, overwrite, 0L );
#endif
		}
		if (key == SPECIAL_KEY_TAB)
		    tabulator_key ();
		if (key == SPECIAL_KEY_DELETE_WORD)
			delete_word();
		if (key == SPECIAL_KEY_TRANSPOSE )
			transpose_char();
		if (key == SPECIAL_KEY_DELETE_LEFT )
			delete_word_left();

	    }

	    if (key == SPECIAL_KEY_PAGE_UP)
		key = ZC_ARROW_UP;
	    if (key == SPECIAL_KEY_PAGE_DOWN)
		key = ZC_ARROW_DOWN;

	} else insert_char ((zchar)key);

    } while (key > 0xff || !is_terminator ((zchar)key));

#ifndef _CONSOLE
		strcpy( &szBigBuffer[ start_buffer_point ], buf );
		bbpos = start_buffer_point + strlen( buf );
		if( key == 13 )
		{
			szBigBuffer[ bbpos++ ] = 13;
			szBigBuffer[ bbpos++ ] = 10;
		}
		szBigBuffer[ bbpos + 1 ] = 0;
		start_buffer_point = 0;
#endif

    last_char ();

//    overwrite = FALSE;

    /* Return terminating key */

	//LOG_EXIT
    return key;

}/* os_read_line */

 /*
 * os_read_key
 *
 * Read a single character from the keyboard (or a mouse click) and
 * return it. Input aborts after timeout/10 seconds.
 *
 */

zchar os_read_key (int timeout, bool cursor)
{
    int key;

	//LOG_ENTER
    set_timer (timeout);
#if defined( WINFROTZ ) && !defined( _CONSOLE )
	AdjustScrollBack();

    do {
		/* TODO: This refresh logic sucks */
		InvalidateRect( hWnd, NULL, FALSE );
		nState = WAIT_FOR_KEY;
#else
    do {
#endif
		key = get_key (cursor);

    } while (key > 0xff);

	//LOG_EXIT
    return key;

}/* os_read_key */

/*
 * os_read_file_name
 *
 * Return the name of a file. Flag can be one of:
 *
 *    FILE_SAVE     - Save game file
 *    FILE_RESTORE  - Restore game file
 *    FILE_SCRIPT   - Transscript file
 *    FILE_RECORD   - Command file for recording
 *    FILE_PLAYBACK - Command file for playback
 *    FILE_SAVE_AUX - Save auxilary ("preferred settings") file
 *    FILE_LOAD_AUX - Load auxilary ("preferred settings") file
 *
 * The length of the file name is limited by MAX_FILE_NAME. Ideally
 * an interpreter should open a file requester to ask for the file
 * name. If it is unable to do that then this function should call
 * print_string and read_string to ask for a file name.
 *
 */


int os_read_file_name (char *file_name, const char *default_name, int flag)
{
    char *extension;
    FILE *fp;
    int key;
	OPENFILENAME ofn;
	char filters[256], *c;
	char strAllFiles[21] = {'A','l','l',' ','F','i','l','e','s',' ',
		'(','*','.','*',')','\0','*','.','*','\0','\0' };

	//LOG_ENTER
	memset( filters, 0, 256 );

    /* Select appropriate extension */
	
    extension = ".aux";
	strcpy( filters,"Preferred Settings (*.aux)");

    if (flag == FILE_SAVE || flag == FILE_RESTORE)
	{
		extension = ".sav";
		strcpy( filters, "Saved games (*.sav)" );
	}
    if (flag == FILE_SCRIPT)
	{
		extension = ".scr";
		strcpy( filters, "Scripts (*.scr)" );
	}
    if (flag == FILE_RECORD || flag == FILE_PLAYBACK)
	{
		extension = ".rec";
		strcpy( filters, "Recording (*.rec)" );
	}
	if( flag == FILE_SAVE_INIT || flag == FILE_LOAD_INIT )
	{
		extension = ".frc";
		strcpy( filters, "Init files (*.frc)" );
	}

	/* Format the extension information for the dialog filters, and add the 'All Files' option */
	c = &filters[ strlen( filters )];
	*c = '\0';
	*++c = '*';
	c++;
	strcpy( c, extension );
	c+=strlen( extension );
	c++;
	memcpy( c, strAllFiles, strlen( strAllFiles) );
	
	file_name[0] = 0;
#ifndef _CONSOLE
	zcodetime += timeGetTime() - zcodestart;
	
#else
	if( bUseFileDialog )
	{
#endif
		//This prevents us from putting up a selector box when the screen has been switched
		//into text mode via ALT-ENTER (for the console). Doesn't do any harm for the regular
		//Windows version.
		if( hWnd && !(GetWindowLong(hWnd, GWL_STYLE) & WS_MINIMIZE) )
		{
			memset( &ofn, 0, sizeof( OPENFILENAME ));
			strcpy( file_name, default_name );

			/* Initialize OPENFILENAME structure. We want Explorer style dialogs */
			ofn.lStructSize = sizeof( OPENFILENAME );
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = filters;
			ofn.lpstrCustomFilter = strCustomFilter;
			ofn.nMaxCustFilter = CUSTOM_FILTER_SIZE;
			ofn.lpstrFile = file_name;
			ofn.nMaxFile = MAX_FILE_NAME;
			ofn.lpstrDefExt = &extension[1];	/* Don't want the '.' here */
			ofn.lpfnHook = (LPOFNHOOKPROC)OFNHookProc;	/* Hook function to center the dialog */
 
			if (flag == FILE_RESTORE || flag == FILE_PLAYBACK || flag == FILE_LOAD_AUX
				|| flag == FILE_LOAD_INIT )
				ofn.lpstrTitle = "WinFrotz Open";
			else
				ofn.lpstrTitle = "WinFrotz Save";
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_HIDEREADONLY;
			
			if (flag == FILE_RESTORE || flag == FILE_PLAYBACK || flag == FILE_LOAD_AUX || flag == FILE_LOAD_INIT )
			{
				if( !GetOpenFileName( &ofn ) )
					return 0;
			}
			else
			{
				if( !GetSaveFileName( &ofn ) )
					return 0;
			}
		}
		else if( !hWnd )
		{
			print_string("Enter file name (extension ");
			print_string(extension);
			print_string(" will be added).\nDefault is \"");
			print_string(default_name);
			print_string("\": ");
			
			read_string (MAX_FILE_NAME - 4, file_name);	
		}
#ifdef _CONSOLE
	}
#endif
	else
	{
		print_string("Enter file name (extension ");
		print_string(extension);
		print_string(" will be added).\nDefault is \"");
		print_string(default_name);
		print_string("\": ");
		
		read_string (MAX_FILE_NAME - 4, file_name);	
	}
#ifndef _CONSOLE
	zcodestart = timeGetTime();
#endif
    /* Use the default name if nothing was typed */
	
    if (file_name[0] == 0)
		strcpy (file_name, default_name);
	
    /* Add the extension */
	
    if (strchr (file_name, '.') == 0)
		strcat (file_name, extension);
	
    /* Return true if the file should be opened for reading */
	
    if (flag == FILE_RESTORE || flag == FILE_PLAYBACK || flag == FILE_LOAD_AUX || flag == FILE_LOAD_INIT )
		return 1;
	
    /* Return true if the file does not exist */
	
    if ((fp = fopen (file_name, "r")) == NULL)
		return 1;
	
    /* Return true if this is a pseudo-file (like PRN, CON, NUL) */
	
    fclose (fp);
	
    /* Ask user for permission to overwrite */

	key = MessageBox( hWnd, "Overwrite existing file? (Answering\nno will result in 'Save failed', this\nis harmless and can be ignored)", "WinFrotz", MB_YESNO );

#ifndef _CONSOLE
	zcodestart = timeGetTime();
#endif

	if( key == IDYES )
		return 1;

/*	sprintf( filters, "Could not open file %s\n", file_name );
	print_string( filters );*/

    /* Return false otherwise */
	
	//LOG_EXIT
    return 0;

}/* os_read_file_name */

static UINT APIENTRY OFNHookProc( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	//LOG_ENTER
	if( uiMsg == WM_INITDIALOG && hWnd )
	{
		CenterWindow( GetParent(hdlg), hWnd );
		return TRUE;
	}
	//LOG_EXIT
	return FALSE;
}

/*****************************************************************************
 *   FUNCTION: CenterWindow (HWND, HWND)									 *
 *																			 *
 *   PURPOSE:  Center one window over another								 *
 *																			 *
 *   COMMENTS:																 *
 *   Dialog boxes take on the screen position that they were designed at,	 *
 *   which is not always appropriate. Centering the dialog over a particular *
 *   window usually results in a better position.							 *
 ****************************************************************************/

static BOOL CenterWindow( HWND hwndChild, HWND hwndParent)
{
	RECT    rChild, rParent;
	int     wChild, hChild, wParent, hParent;
	int     wScreen, hScreen, xNew, yNew;
#ifdef _CONSOLE
	HDC     hdc;
	// Get the display limits
	hdc = GetDC (hwndChild);
#endif	
	//LOG_ENTER
	// Get the Height and Width of the child window
	GetWindowRect (hwndChild, &rChild);
	wChild = rChild.right - rChild.left;
	hChild = rChild.bottom - rChild.top;
	
	// Get the Height and Width of the parent window
	GetWindowRect (hwndParent, &rParent);
	wParent = rParent.right - rParent.left;
	hParent = rParent.bottom - rParent.top;
	
	wScreen = GetDeviceCaps (hdc, HORZRES);
	hScreen = GetDeviceCaps (hdc, VERTRES);
#ifdef _CONSOLE
	ReleaseDC(hwndChild, hdc);
#endif
	// Calculate new X position, then adjust for screen
	xNew = rParent.left + ((wParent - wChild) /2);
	if (xNew < 0) 
	{
		xNew = 0;
	} else if ((xNew+wChild) > wScreen) 
	{
		xNew = wScreen - wChild;
	}
	
	// Calculate new Y position, then adjust for screen
	yNew = rParent.top  + ((hParent - hChild) /2);
	if (yNew < 0) 
	{
		yNew = 0;
	} else if ((yNew+hChild) > hScreen) 
	{
		yNew = hScreen - hChild;
	}
	
	// Set it, and return
	//LOG_EXIT
	return SetWindowPos (hwndChild, NULL,
		xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
