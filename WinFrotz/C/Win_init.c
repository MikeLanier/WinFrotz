/*
 * win_init.c
 *
 * Visual C interface, initialisation, destruction
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <sys\types.h>
#include "../frotz/frotz.h"
#include "../include/logfile_c.h"
#include <mmsystem.h>
#include <wingdi.h>
#include <crtdbg.h>
#include "../include/windefs.h"
#ifndef	_CONSOLE
#include "../include/crosscomp.h"
#endif

/****************************************************************************
 *	Common variables and constants to both versions							*
 ***************************************************************************/

#define INFORMATION "\
\n\
FROTZ V2.32 - interpreter for all Infocom games. Complies with standard\n\
1.0 of Graham Nelson's specification. Written by Stefan Jokisch in 1995-7\n\
Win95/NT versions by Richard Lawrence\n\
\n\
Syntax: frotz [options] story-file\n\
\n\
  -a   watch attribute setting  \t -l # left margin\n\
  -A   watch attribute testing  \t -o   watch object movement\n\
  -b # background colour        \t -O   watch object locating\n\
  -B # reverse background colour\t -p   alter piracy opcode\n\
  -c # context lines            \t -r # right margin\n\
  -d # display mode (see below) \t -s # random number seed value\n\
  -e # emphasis colour [mode 1] \t -S # transscript width\n\
  -f # foreground colour        \t -t   set Tandy bit\n\
  -F # reverse foreground colour\t -T   bold typing [modes 2+4+5]\n\
  -g # font [mode 5] (see below)\t -u # slots for multiple undo\n\
  -h # screen height            \t -w # screen width\n\
  -i   ignore runtime errors    \t -x   expand abbreviations g/x/z\n\
\n\
Fonts are 0 (fixed), 1 (sans serif), 2 (comic), 3 (times), 4 (serif).\n\
\n\
Display modes are 0 (mono), 1 (text), 2 (CGA), 3 (MCGA), 4 (EGA), 5 (Amiga)."

extern void init_buffer_vars( void );
extern void restart_header(void);
extern void init_memory (void);
extern void reset_memory (void);
extern void init_undo (void);
extern void interpret(void);

#ifdef BUFFERED_TEXTOUT
void clear_output_buffer( void );
#endif

extern FILE *story_fp;
char icon_name[MAX_FILE_NAME + 1];

extern int  load_aliases(const char* filename);
extern int	open_graphics_file (int number);
extern int	init_sound (void);
extern void close_graphics_file(void);
extern void reset_sound(void);

/*	Frotz code globals required in this module - some of these have been 
	changed to non-static because they are used in other os-dependent 
	modules, notably the Windows code										*/

int display = -1;
extern int current_style;
int user_tandy_bit = -1;
static zword user_screen_width = (zword)-1;
static zword user_screen_height = (zword)-1;
extern int user_min_volume;
extern int user_max_volume;
extern int has_played_sound;
extern int current_font;
extern int locked;
char stripped_story_name[MAX_PATH];
char data_path[MAX_PATH];
int user_random_seed = -1;
void assign_opcodes( void );
void ComputeFontWidths( void );
HWND			hWnd = NULL;

DWORD				cursorHeight=1;

int		cTranslate[256];

int getopt (int, char *[], const char *);

extern const char *optarg;
extern int optind;

extern char script_name[];
extern char command_name[];
extern char save_name[];
extern char auxilary_name[];

#ifdef WINFROTZ
extern int	finished;
#endif

/****************************************************************************
 *	Console mode client variables and constants								*
 ***************************************************************************/
#ifdef _CONSOLE

COORD				curCoord;

static BOOL WINAPI HandlerRoutine(DWORD dwCtrlType);

HANDLE				hConsoleOutput, hConsoleInput;
CONSOLE_CURSOR_INFO			cursorInfo;
static CONSOLE_SCREEN_BUFFER_INFO	ConsoleInfo;
static DWORD	InitInputMode, InitOutputMode;
BOOL						bUseFileDialog = TRUE;

SIZE	charSize;

extern	int pc_colour_fg[], pc_colour_bg[], user_foreground, user_background;
extern	int user_reverse_fg, user_reverse_bg, user_emphasis, user_bold;

/****************************************************************************
 *	Full Windows client variables and constants								*
 ***************************************************************************/
#else
fakecoord			curCoord;

char szBigBuffer[32000];
char *ScrollRoll[100];
int	nScrollBuffs;
int	bbpos;

/*	Used for the about box to figure out how fast the "Z-machine" is running*/
int				zcodeops;
unsigned long	zcodestart, zcodetime;

extern	COLORREF pc_colour_fg[], *pc_colour_bg, user_foreground, user_background;
extern	COLORREF user_reverse_fg, user_reverse_bg, user_emphasis, user_bold, text_fg, text_bg;

extern int		kbBuffer[];

extern void flush_buffer (void);
extern int font_height;
extern int font_width;
extern frotzwin wp[];
int			mouse_x, mouse_y;

extern	fontprops	fpFonts[NUM_FONT_TYPES];
extern	int			current_font_style;
extern	int *current_font_width;

extern zbyte far *zmp;
extern zbyte far *pcp;

HDC			hdc = NULL;
LPBITMAPINFO	lpbmi;
extern BOOL MakeMemPalette( );
extern DestroyMemPalette( );

extern	void	resetScrollPos( void );
void	cleanup( void );

/*	All drawing is done to a bitmap. h_memDC is the memory device context	
	that was created compatible with the current display. Since mem devices
	are permanent we globally select the bitmap into this context, as well
	as the font and brush													*/
HDC			h_memDC = NULL;
HGDIOBJ		oldMemBmap = NULL;
HBITMAP		hbmScreenBuffer = NULL;
HGDIOBJ		hgdiOldFont = NULL;

/*	graphics_mode is the display mode when in a V6 game; reg_display is the
	display mode that is recorded in the registry. The Frotz variable 
	"display" could be set to either of these depending on whether the user
	is running a V6 game or not. Dual color mode is "CGA_MODE" to Frotz, 
    others are "MCGA_MODE"													*/
int			graphics_mode;
int			reg_display = -1;

/*	Hack to keep the status from being shown before it was drawn by Frotz	*/
int			status_shown = 0;

/*	The erase brush. This is used much like fonts; it's allocated and 
	selected into the memory context, then only if it's the wrong color for
	the current erase is it reallocated										*/
HGDIOBJ		bkgBrush=NULL, startBrush=NULL;
COLORREF	bkgBrushcolor=RGB(0,0,0);

/*	nState is the ugly state variable for thread "signalling". States are
	enumerated in STATES.H. Semaphores aren't really required for our simple
	needs. hThread is just the handle to this (Frotz code) thread			*/
int			nState = UNSTARTED;
HANDLE		hThread = NULL; 

BOOL		bStoryLoaded = FALSE;
BOOL		bStoryInit = FALSE;

/*	nUpdates will cause draw operations after almost any action. There is	
	considerable overhead to this so skipping it runs much faster (the screen
	is updated at input time when the user is ready to see it). Having this
	set to 2 (very frequent) causes smooth but slow scrolling				*/
int			nUpdates;

/*	The X and Y size of our memory bitmap selected into h_memDC. See below	*/
int			bmapX, bmapY;

/*	By default the Y size of the allocated bitmap is twice as large as the	
	actual screen requirement. This is so we can get around having to call
	BitBlt as often; an incredibly slow function. When we can we simply
	offset nScrollPos by the amount we are scrolling, in effect moving a 
	scrolling window down our large bitmap until we reach the bottom, THEN
	we BitBlt just once (or at least much less frequently)					*/
long		nScrollPos = 0;
static	DWORD	last_scroll_time = 0;

char	*statusmem, *status_style = NULL;
int		status_last_char;

#endif

/****************************************************************************/
/* END of windowed version specific globals									*/
/****************************************************************************/


/*
 * os_fatal
 *
 * Display error message and stop interpreter.
 *
 */

void os_fatal (const char *s)
{
	char ErrString[256];
	//LOG_ENTER
#ifdef _CONSOLE
	SetConsoleTitle( "WinFrotz Fatal Error!" );
#endif
	sprintf(ErrString, "Fatal: %s\n", s); 
	MessageBox( hWnd, &ErrString[0], "WinFrotz Error", MB_ICONSTOP );

#ifdef _DEBUG
	FormatMessage( 
    FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) ErrString,
    511,
    NULL );

	MessageBox( hWnd, &ErrString[0], "GetLastError", MB_ICONSTOP );
#endif

#ifndef _CONSOLE
	cleanup();
#endif
	//LOG_EXIT
	exit (1);
}/* os_fatal */

#ifdef _CONSOLE
/*
 * dectoi
 *
 * Convert a string containing a decimal number to integer. The string may
 * be NULL, but it must not be empty.
 *
 */

int dectoi (const char *s)
{
    int n = 0;

	//LOG_ENTER
    if (s != NULL)

	do {

	    n = 10 * n + (*s & 15);

	} while (*++s > ' ');

	//LOG_EXIT
    return n;

}/* dectoi */

/*
 * hextoi
 *
 * Convert a string containing a hex number to integer. The string may be
 * NULL, but it must not be empty.
 *
 */

int hextoi (const char *s)
{
    int n = 0;

	//LOG_ENTER
    if (s != NULL)

	do {

	    n = 16 * n + (*s & 15);

	    if (*s > '9')
		n += 9;

	} while (*++s > ' ');

	//LOG_EXIT
    return n;

}/* hextoi */

 
/*
 * parse_options
 *
 * Parse program options and set global flags accordingly.
 *
 */

static void parse_options (int argc, char **argv)
{
    int c;

	//LOG_ENTER
    do {

	int num = 0;

	c = getopt (argc, argv, "aAb:B:c:d:e:f:F:g:h:il:oOpr:s:S:tTu:w:x");

	if (optarg != NULL)
	    num = dectoi (optarg);

	if (c == 'a')
	    option_attribute_assignment = 1;
	if (c == 'A')
	    option_attribute_testing = 1;
	if (c == 'b')
	    user_background = num;
	if (c == 'B')
	    user_reverse_bg = num;
	if (c == 'c')
	    option_context_lines = num;
	if (c == 'd')
	    display = optarg[0] | 32;
	if (c == 'e')
	    user_emphasis = num;
	if (c == 'f')
	    user_foreground = num;
	if (c == 'F')
	    user_reverse_fg = num;
	if (c == 'h')
	    user_screen_height = num;
	if (c == 'i')
	    option_ignore_errors = 1;
	if (c == 'l')
	    option_left_margin = num;
	if (c == 'o')
	    option_object_movement = 1;
	if (c == 'O')
	    option_object_locating = 1;
	if (c == 'p')
	    option_piracy = 1;
	if (c == 'r')
	    option_right_margin = num;
	if (c == 's')
	    user_random_seed = num;
	if (c == 'S')
	    option_script_cols = num;
	if (c == 't')
	    user_tandy_bit = 1;
	if (c == 'u')
	    option_undo_slots = num;
	if (c == 'w')
	    user_screen_width = num;
	if (c == 'x')
	    option_expand_abbreviations = 1;

    } while (c != EOF);

	//LOG_EXIT
}/* parse_options */
#endif

/*
  os_process_arguments
 
  Handle command line switches. Some variables may be set to activate
  special features of Frotz:
 
      option_attribute_assignment
      option_attribute_testing
      option_context_lines
      option_object_locating
      option_object_movement
      option_left_margin
      option_right_margin
      option_piracy
      option_undo_slots
 
  The name of the story file is stored in "story_name".
 
 */
void os_process_arguments (int argc, char *argv[])
{
	int i;
    const char *p;
	//LOG_ENTER
#ifdef _CONSOLE

    /* Parse command line options */

    parse_options (argc, argv);

    if (optind != argc - 1) {
	puts (INFORMATION);
	exit (EXIT_FAILURE);
    }

    /* Set the story file name */
	story_name = argv[optind];
#else
    story_name = argv[optind];
	if( !story_name)
	{
	//LOG_EXIT
		return;
	}
#endif

    /* Strip path and extension off the story file name */

    p = story_name;

    for (i = 0; story_name[i] != 0; i++)
	if (story_name[i] == '\\' || story_name[i] == ':')
	    p = story_name + i + 1;

    for (i = 0; p[i] != 0 && p[i] != '.' && i < MAX_PATH; i++)
	stripped_story_name[i] = p[i];

    stripped_story_name[i] = 0;

	for( i=strlen( story_name ); i > 0 && story_name[i]!='\\' && story_name[i]!=':'; i-- );
	if( i )
		strncpy( data_path, story_name, i+1 );
	else
		strcpy( data_path, ".\\" );

    /* Create nice default file names */
	strcpy (icon_name, stripped_story_name);
    strcpy (script_name, stripped_story_name);
    strcpy (command_name, stripped_story_name);
    strcpy (save_name, stripped_story_name);
    strcpy (auxilary_name, stripped_story_name);

	strcat (icon_name, ".ico");
    strcat (script_name, ".scr");
    strcat (command_name, ".rec");
    strcat (save_name, ".sav");
    strcat (auxilary_name, ".aux");

	//LOG_EXIT
}/* os_process_arguments */


 /*	os_init_screen
 
	Initialise the IO interface. Prepare the screen and other devices
	(mouse, sound card). Set various OS depending story file header
	entries:
   
	h_config (aka flags 1)
	h_flags (aka flags 2)
	h_screen_cols (aka screen width in characters)
	h_screen_rows (aka screen height in lines)
	h_screen_width
	h_screen_height
	h_font_height (defaults to 1)
	h_font_width (defaults to 1)
	h_default_foreground
	h_default_background
	h_interpreter_number
	h_interpreter_version
	h_user_name (optional; not used by any game)
 
*/

void os_init_screen (void)
{
	char	szTitle[512];
	FILE	*localfp;
	int i;

	//LOG_ENTER
#ifdef _CONSOLE
	COORD	tmpCoord;
	BOOL	bSuccess;
	SMALL_RECT	Screen;
#endif
	curCoord.X = 0;
	curCoord.Y = 0;

    /* Set various bits in the configuration byte. These bits tell
       the game which features are supported by the interpreter. */

    if (h_version == V3 && user_tandy_bit != -1)
	h_config |= CONFIG_TANDY;
    if (h_version == V3)
	h_config |= CONFIG_SPLITSCREEN;
    if (h_version == V3 )
	h_config |= CONFIG_PROPORTIONAL;
    if (h_version >= V4 )
	h_config |= CONFIG_BOLDFACE;
    if (h_version >= V4 )
	h_config |= CONFIG_EMPHASIS | CONFIG_FIXED | CONFIG_TIMEDINPUT;
    if (h_version >= V5 && display!=CGA_MODE )
	h_config |= CONFIG_COLOUR;
    

	/* TODO: Why does this differ from Stefan's code? He set this bit on in V5 games but doesn't get
	   the time in the status line problem....huh? */
	if (h_version >= V5 && open_graphics_file(1) )
	h_config |= CONFIG_PICTURES;

    /* Handle various game flags. These flags are set if the game wants
       to use certain features. The flags must be cleared if the feature
       is not available. */

    if (h_flags & GRAPHICS_FLAG && story_id!=BEYOND_ZORK )
	{
		if (display <= TEXT_MODE || open_graphics_file (1) == 0)
			h_flags &= ~GRAPHICS_FLAG;
	}
    if (h_version == V3 && (h_flags & OLD_SOUND_FLAG))
	if (!init_sound ())
	    h_flags &= ~OLD_SOUND_FLAG;
    if (h_flags & SOUND_FLAG)
	if (!init_sound ())
	    h_flags &= ~SOUND_FLAG;
    if (h_version >= V5 && (h_flags & UNDO_FLAG))
	if (!option_undo_slots)
	    h_flags &= ~UNDO_FLAG;
    if (h_flags & COLOUR_FLAG)
	if (display <= CGA_MODE )
	    h_flags &= ~COLOUR_FLAG;
    h_flags &= ~MENU_FLAG;

#ifndef _CONSOLE

	if( h_version == V6 )
	{
		if( display == EGA_MODE )
			pc_colour_fg[9] = RGB( 255, 255, 255);
		if( (GetDeviceCaps( h_memDC, RASTERCAPS) & RC_PALETTE) )
		{
			user_foreground = pc_colour_fg[9];
			user_background = pc_colour_fg[0];
		}
		else
		{
			user_foreground = pc_colour_fg[12];
			user_background = pc_colour_bg[10];
		}
	}
	pc_colour_fg[14] = user_foreground;
	pc_colour_bg[15] = user_background;
#else
	pc_colour_fg[14] = pc_colour_fg[user_foreground];
	pc_colour_bg[14] = pc_colour_bg[user_foreground];
	pc_colour_bg[15] = pc_colour_bg[user_background];
	pc_colour_fg[15] = pc_colour_fg[user_background];
#endif

	h_default_foreground = 14;
	h_default_background = 15;
#ifdef _CONSOLE
	SetConsoleTitle("WinFrotz Initializing" );

	h_screen_width = 80; 
	h_screen_height = 25;

	h_font_height = 1;
	h_font_width = 1;

	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode( hConsoleOutput, &InitOutputMode );
	GetConsoleMode( GetStdHandle(STD_INPUT_HANDLE), &InitInputMode );
		  
	if (user_screen_width != (zword)-1)
		h_screen_width = user_screen_width;
	if (user_screen_height != (zword)-1)
		h_screen_height = user_screen_height;
		  
	tmpCoord = GetLargestConsoleWindowSize( hConsoleOutput );
	if( h_screen_width > tmpCoord.X )
		h_screen_width = tmpCoord.X;
	if( h_screen_height > tmpCoord.Y )
		h_screen_height = tmpCoord.Y;
		  
	tmpCoord.X = h_screen_width;
	tmpCoord.Y = h_screen_height;
		  
	if( GetConsoleScreenBufferInfo( hConsoleOutput, &ConsoleInfo ) )
	{
		if( ConsoleInfo.dwSize.X != h_screen_width || ConsoleInfo.dwSize.Y != h_screen_height )
		{
			if( !SetConsoleScreenBufferSize( hConsoleOutput, tmpCoord ) )
				os_fatal("Could not set specified screen size");
			
			Screen.Left=0;
			Screen.Top=0;
			Screen.Right=h_screen_width-1;
			Screen.Bottom=h_screen_height-1;
			if( !SetConsoleWindowInfo( hConsoleOutput, TRUE, &Screen ) )
				os_fatal("Could not set specified screen size");
			
		}
	}

	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode( hConsoleInput, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT );

	if( h_version == V6 )
		MessageBox( hWnd, V6_WARNING, "WinFrotz Info", MB_ICONEXCLAMATION );

#ifdef BUFFERED_TEXTOUT
		SetConsoleMode( hConsoleOutput, ENABLE_PROCESSED_OUTPUT /*| ENABLE_WRAP_AT_EOL_OUTPUT*/ );
#endif

	GetConsoleCursorInfo( hConsoleOutput, &cursorInfo );
	cursorInfo.dwSize = cursorHeight;
		  
	bSuccess = SetConsoleCtrlHandler(HandlerRoutine,TRUE );

	/* Consoles are weird, since you never create your own Window, and therefore don't
	   have a given handle to it. This seems to work, but not always, grrr */
	hWnd = FindWindow("ConsoleWindowClass", "WinFrotz Initializing" );
	if( !hWnd )
		hWnd = FindWindow( "tty", "WinFrotz Initializing" );
	if( !hWnd )
		hWnd = FindWindow( NULL, "WinFrotz Initializing" );
	strcpy( szTitle, "WinFrotz - " );
	strcat( szTitle, stripped_story_name );
	SetConsoleTitle( szTitle );
#else
	memset( lpbmi, 0, sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * 226 );

	if( display == EGA_MODE )
	{
		//Set up EGA-mode color values - have YOU tried looking up the RGB
		//values of the EGA palette lately??

		lpbmi->bmiColors[0].rgbRed = 0;
		lpbmi->bmiColors[0].rgbGreen = 0;
		lpbmi->bmiColors[0].rgbBlue = 0;

		lpbmi->bmiColors[1].rgbRed = 0;
		lpbmi->bmiColors[1].rgbGreen = 0;
		lpbmi->bmiColors[1].rgbBlue = 168;

		lpbmi->bmiColors[2].rgbRed = 0;	
		lpbmi->bmiColors[2].rgbGreen = 168;
		lpbmi->bmiColors[2].rgbBlue = 0;

		lpbmi->bmiColors[3].rgbRed = 0;
		lpbmi->bmiColors[3].rgbGreen = 168;
		lpbmi->bmiColors[3].rgbBlue = 168;

		lpbmi->bmiColors[4].rgbRed = 168;
		lpbmi->bmiColors[4].rgbGreen = 0;
		lpbmi->bmiColors[4].rgbBlue = 0;

		lpbmi->bmiColors[5].rgbRed = 168;
		lpbmi->bmiColors[5].rgbGreen = 0;
		lpbmi->bmiColors[5].rgbBlue = 168;

		lpbmi->bmiColors[6].rgbRed = 168;
		lpbmi->bmiColors[6].rgbGreen = 84;
		lpbmi->bmiColors[6].rgbBlue = 0;

		lpbmi->bmiColors[7].rgbRed = 168;
		lpbmi->bmiColors[7].rgbGreen = 168;
		lpbmi->bmiColors[7].rgbBlue = 168;

		lpbmi->bmiColors[8].rgbRed = 88;
		lpbmi->bmiColors[8].rgbGreen = 88;
		lpbmi->bmiColors[8].rgbBlue = 88;

		lpbmi->bmiColors[9].rgbRed = 0;
		lpbmi->bmiColors[9].rgbGreen = 0;
		lpbmi->bmiColors[9].rgbBlue = 212;

		lpbmi->bmiColors[10].rgbRed = 0;
		lpbmi->bmiColors[10].rgbGreen = 212;
		lpbmi->bmiColors[10].rgbBlue = 0;

		lpbmi->bmiColors[11].rgbRed = 0;
		lpbmi->bmiColors[11].rgbGreen = 212;
		lpbmi->bmiColors[11].rgbBlue = 212;

		lpbmi->bmiColors[12].rgbRed = 252;
		lpbmi->bmiColors[12].rgbGreen = 84;
		lpbmi->bmiColors[12].rgbBlue = 84;

		lpbmi->bmiColors[13].rgbRed = 212;
		lpbmi->bmiColors[13].rgbGreen = 0;
		lpbmi->bmiColors[13].rgbBlue = 212;

		lpbmi->bmiColors[14].rgbRed = 212;
		lpbmi->bmiColors[14].rgbGreen = 212;
		lpbmi->bmiColors[14].rgbBlue = 0;

		lpbmi->bmiColors[15].rgbRed = 255;
		lpbmi->bmiColors[15].rgbGreen = 255;
		lpbmi->bmiColors[15].rgbBlue = 255;
	}

	text_fg = pc_colour_fg[9];
	text_bg = pc_colour_fg[0];
	if( display < MCGA_MODE )
	{
		text_fg = RGB( 0,0,0 );
		text_bg = RGB( 255,255,255 );
		pc_colour_fg[14] = RGB(255,255,255);
		pc_colour_bg[15] = RGB(0,0,0);
		SetBkColor( hdc, user_foreground );
		SetTextColor( hdc, user_background );
	}

	MakeMemPalette();

	if( startBrush )
		SelectObject( h_memDC, startBrush );
	if( bkgBrush )
		DeleteObject( bkgBrush );
	if( display < MCGA_MODE )
	{
		bkgBrush = CreateSolidBrush( RGB( 255, 255, 255 ) );
		bkgBrushcolor = RGB( 255, 255, 255 );
	}
	else
	{
		bkgBrush = CreateSolidBrush( user_background );
		bkgBrushcolor = user_background;
	}
	_ASSERT( bkgBrush );
	startBrush = SelectObject( h_memDC, bkgBrush );
	_ASSERT( startBrush );

	if( hgdiOldFont )
		SelectObject( h_memDC, hgdiOldFont );
	_ASSERT( fpFonts[STANDARD].hfHandle );
	hgdiOldFont = SelectObject( h_memDC, fpFonts[STANDARD].hfHandle );
	_ASSERT( hgdiOldFont );
	current_font_style = 0;

	/* h_screen_width and h_screen_height have already been set */
	SetBkMode( h_memDC, OPAQUE);
	SetBkColor( h_memDC, user_background );
	SetTextColor( h_memDC, user_foreground);

	memset( kbBuffer, 0, KEYBOARD_BUFFER_SIZE * sizeof( int ));
#endif

	/* Initialize the character translators. We init to an invalid character */
	/* and fill in the valid ones (WinFrotz will print a ? for invalids as per spec */
	memset( cTranslate, 0, 256 * sizeof( int ));

	cTranslate[8] = 8;
	cTranslate[9] = 9;
	cTranslate[13] = 13;
	cTranslate[27] = 27;
	cTranslate[32] = 32;
	cTranslate[33] = 129;	
	cTranslate[34] = 130;
	cTranslate[35] = SPECIAL_KEY_END;
	cTranslate[36] = SPECIAL_KEY_HOME;
	cTranslate[37] = 131;
	cTranslate[38] = 129;
	cTranslate[39] = 132;
	cTranslate[40] = 130;
	cTranslate[45] = SPECIAL_KEY_INSERT;
	cTranslate[46] = SPECIAL_KEY_DELETE;

	/* Set the screen dimensions in character grid positions. */

	if( h_font_height )	  
		h_screen_rows = h_screen_height / h_font_height;
	if( h_font_width )
		h_screen_cols = h_screen_width / h_font_width;
		  
	/* Set the interpreter number (a constant telling the game which
	operating system it runs on) and the interpreter version. The
	interpreter number has effect on all V6 games and Beyond Zork. */
		  
    h_interpreter_number = INTERP_MSDOS;
    h_interpreter_version = 'F';
		  
	/* Load any pre-existing aliases. First try the current directory, then the directory the story
	file came from with the story name */
		  
	localfp = fopen( "frotz.frc", "rb" );
	if( localfp )
	{
		fclose( localfp );
		load_aliases( "frotz.frc" );
	}

	strcpy( szTitle, data_path );
	strcat( szTitle, stripped_story_name );
	for( i=strlen( szTitle ); i > 0 && szTitle[i]!='.' && szTitle[i]!='\\'; i-- );
	if( szTitle[i]=='.' )
		szTitle[i] = '\0';
	strcat( szTitle, ".frc" );

	localfp = fopen( szTitle, "rb" );
	if( localfp )
	{
		fclose( localfp );
		load_aliases( szTitle );
	}
	
	//LOG_EXIT
}/* os_init_screen */

/*
* os_reset_screen
*
* Reset the screen before the program ends.
*
*/

void os_reset_screen (void)
{
	//LOG_ENTER

	/* Reset the sound board and graphics */
	
	close_graphics_file ();
	reset_sound();
	has_played_sound = 0;

    os_set_font (TEXT_FONT);
    os_set_text_style (0);
    os_display_string ("[Hit any key to exit.]");
#ifdef BUFFERED_TEXTOUT
	clear_output_buffer();
#endif
    /* Wait for a key */
#ifndef _CONSOLE
	if( finished )
#endif
	    os_read_key(0, TRUE);
	
#ifdef _CONSOLE
	/* Reset screen size */
	SetConsoleWindowInfo( hConsoleOutput, TRUE, &ConsoleInfo.srWindow );

	SetConsoleMode( hConsoleOutput, InitOutputMode );
	SetConsoleMode( hConsoleInput, InitInputMode );
	/* Reset text colors to what was present before WinFrotz */
	SetConsoleTextAttribute( hConsoleOutput, ConsoleInfo.wAttributes );
	{
		COORD	end = {0,0};
		DWORD	dummy;

		FillConsoleOutputAttribute( hConsoleOutput, ConsoleInfo.wAttributes, 
			(ConsoleInfo.srWindow.Right+1) * (ConsoleInfo.srWindow.Bottom+1), end, &dummy );
	}
#endif
	//LOG_EXIT
}/* os_reset_screen */

/*
 * os_restart_game
 *
 * This routine allows the interface to interfere with the process of
 * restarting a game at various stages:
 *
 *     RESTART_BEGIN - restart has just begun
 *     RESTART_WPROP_SET - window properties have been initialised
 *     RESTART_END - restart is complete
 *
 */

void os_restart_game (int stage)
{
#if defined( WINFROTZ ) && !defined( _CONSOLE )
    int x, y;
	
	//LOG_ENTER
    if (story_id == BEYOND_ZORK)
	{
		if (stage == RESTART_BEGIN)
		{
			// Set up the fixed underline (alternate emphasis) font
			if( !fpFonts[ZORK_FONT].hfHandle )
			{
				BOOL	bFontFound = FALSE;
				
				// Same for Fixed Font
				bFontFound = AddFontResource( "beyzork.ttf" );
				if( !bFontFound )
				{
					if( !(bFontFound = AddFontResource( "BEYZORK.FON" )) )
						MessageBox( hWnd, "Note: Could not load the Beyond Zork special font.\nMake sure it is in the WinFrotz directory.", "Warning", MB_OK );
				}
				if( bFontFound )
				{
					SendMessage( HWND_BROADCAST, WM_FONTCHANGE, 0, 0L );
					memset( &fpFonts[ZORK_FONT].lf, 0, sizeof( LOGFONT ) );
					strcpy( fpFonts[ZORK_FONT].lf.lfFaceName, "Zork" );
					fpFonts[ZORK_FONT].lf.lfHeight = fpFonts[FIXED].lf.lfHeight;
					fpFonts[ZORK_FONT].lf.lfWidth = fpFonts[FIXED].lf.lfWidth;
					fpFonts[ZORK_FONT].lf.lfWeight = 400;
					fpFonts[ZORK_FONT].lf.lfCharSet = 2;
					fpFonts[ZORK_FONT].lf.lfOutPrecision = 1;
					fpFonts[ZORK_FONT].lf.lfClipPrecision = 2;
					fpFonts[ZORK_FONT].lf.lfQuality = 1;
					fpFonts[ZORK_FONT].lf.lfPitchAndFamily = 49;
					fpFonts[ZORK_FONT].hfHandle = CreateFontIndirect( &fpFonts[ZORK_FONT].lf );	
					if( strcmp( fpFonts[ZORK_FONT].lf.lfFaceName, "Zork" ) )
						MessageBox( hWnd, "Note: Attempt to load Beyond Zork font failed.", fpFonts[ZORK_FONT].lf.lfFaceName, MB_OK );
					GetObject( fpFonts[ZORK_FONT].hfHandle, sizeof( LOGFONT ), &fpFonts[ZORK_FONT].lf);
				}
			}
			else
			{
				DeleteObject( fpFonts[ZORK_FONT].hfHandle );
				fpFonts[ZORK_FONT].hfHandle = NULL;
				strcpy( fpFonts[ZORK_FONT].lf.lfFaceName, "Zork" );
				fpFonts[ZORK_FONT].lf.lfHeight = fpFonts[FIXED].lf.lfHeight;
				fpFonts[ZORK_FONT].hfHandle = CreateFontIndirect( &fpFonts[ZORK_FONT].lf );	
			}

			ComputeFontWidths();

			if ( os_picture_data (1, &x, &y)) {
				
				os_draw_picture (1, 1, 1);
				os_read_key (0, FALSE);
				
			}
		}
	}
#endif
	//LOG_EXIT
}/* os_restart_game */

/*
 * os_random_seed
 *
 * Return an appropriate random seed value in the range from 0 to
 * 32767, possibly by using the current system time.
 *
 */

int os_random_seed (void)
{
	//LOG_ENTER

    if (user_random_seed == -1) {
		DWORD	dwSpace = 0;
		int		divisor = 0;

		dwSpace = ~dwSpace;
		divisor = dwSpace >> 15;

		/* Use the time of day as seed value */

	//LOG_EXIT
		return( timeGetTime() / divisor );

    } else 
		{
	//LOG_EXIT
			return user_random_seed;
	}

	//LOG_EXIT
}/* os_random_seed */

/*****************************************************************************
 *   FUNCTION: HandlerRoutine (DWORD)										 *
 *																			 *
 *   PURPOSE:  Process special Console events (ctrl-C, shutdown)			 *
 *																			 *
 *   COMMENTS:																 *
 *   Broken under Win95, where CTRL_SHUTDOWN_EVENT apparently does not get	 *
 *	 sent for some reason													 *
 ****************************************************************************/
#ifdef _CONSOLE
BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	//LOG_ENTER
	switch (dwCtrlType)
	{

		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		{
			handle_hot_key( ZC_HKEY_QUIT);
			break;
		}

		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		{
			exit(0);
		}
		default:
		{
			os_fatal("Unhandled control message passed to HandlerRoutine" );
	//LOG_EXIT
			return FALSE;
		}
	}
	//LOG_EXIT
	return(TRUE);
}
#endif

/*****************************************************************************
 *   FUNCTION: ComputeFontWidths (void)										 *
 *																			 *
 *   PURPOSE:  Pre-compute widths for individual characters	of various fonts *
 *																			 *
 *   COMMENTS:																 *
 *   This function rolls through all displayable characters for a font and   *
 *   stores the width for each in an array. This saves us time later for a   *
 *   variety of operations, such as Z-machine "how much of the line is left" *
 *   operations, and anything else that revolves around os_char_width.       *
 ****************************************************************************/
#ifndef _CONSOLE
void ComputeFontWidths( void ) 
{
	int			i, j;
	char		test[2];
	HGDIOBJ		oldFont;
	SIZE		tmpSize;
	TEXTMETRIC	tm;
	HDC			hdc;

	//LOG_ENTER
	hdc = GetDC( NULL );
	test[1] = '\0';
	for( i = 0; i < NUM_FONT_TYPES; i++ )
	{
		if( fpFonts[i].hfHandle )
		{
			oldFont = SelectObject( hdc, fpFonts[i].hfHandle );
			for( j=0; j < 255; j++ )
			{
				test[0] = (char)j;
				GetTextExtentPoint32( hdc,	test, 1, &tmpSize );
				fpFonts[i].nWidths[j] = tmpSize.cx;
			}
			/* Characters 9 and 11 are special indentations, so they have special widths */
			fpFonts[i].nWidths[9] = fpFonts[i].nWidths[' '] * 3;
			fpFonts[i].nWidths[11] = fpFonts[i].nWidths[' '] * 2;

			GetTextMetrics( hdc, &tm );
			fpFonts[i].nHeight = tm.tmHeight;
			SelectObject( hdc, oldFont );
		}
	}
	ReleaseDC( NULL, hdc );
	//LOG_EXIT
}

/****************************************************************************
 *	FUNCTION: AddStyleFonts (void)											*
 *																			*
 *	PURPOSE:  Given the base fonts, add italic, bold, etc varieties			*
 *																			*
 *	COMMENTS:																*
 *	We're dealing with two base fonts, the "standard" and "fixed" ones.		*
 *	Do not confuse the Z-machine terms; to a Z-machine these are _styles_,	*
 *	to us they are seperate fonts. In fact to Windows everything is a		*
 *	seperate font, so we have to have seperate instances of each style type	*
 *  (bold, underline, etc). This is where we fill those types in, with the	*
 *	assumption that there already exists a handle for the standard and		*
 *	fixed types.															*
 ***************************************************************************/
void AddStyleFonts( void )
{
	TEXTMETRIC	tm;
	HGDIOBJ		oldFont;
	HDC			screendc = GetDC( NULL );
	int			i;

	//LOG_ENTER
	/* First set the default, permanent (user preference) display styles for the fonts */
	for( i = 0; i < NUM_FONT_TYPES; i++ )
		fpFonts[i].nUsage = fpFonts[i].nUsagePerm;

	if( !fpFonts[STANDARD].hfHandle || !fpFonts[FIXED].hfHandle )
	{
		MessageBox( hWnd, "WARNING: Attempt to add style fonts with no base types.", "WinFrotz", MB_ICONEXCLAMATION );
	//LOG_EXIT
		return;
	}

	// Set up the standard emphasis font
	if( fpFonts[EMPHASIS].hfHandle )
		DeleteObject( fpFonts[EMPHASIS].hfHandle );
	fpFonts[EMPHASIS].hfHandle = NULL;
	memcpy( &fpFonts[EMPHASIS].lf, &fpFonts[STANDARD].lf, sizeof( LOGFONT ) );
	fpFonts[EMPHASIS].lf.lfItalic = TRUE;
	fpFonts[EMPHASIS].hfHandle = CreateFontIndirect( &fpFonts[EMPHASIS].lf );
	if( !fpFonts[EMPHASIS].hfHandle )
		os_fatal( "Could not allocate italic version of standard font!" );
	GetObject( fpFonts[EMPHASIS].hfHandle, sizeof( LOGFONT ), &fpFonts[EMPHASIS].lf);

	// Set up the standard bold font
	if( fpFonts[BOLD].hfHandle )
		DeleteObject( fpFonts[BOLD].hfHandle );
	fpFonts[BOLD].hfHandle = NULL;
	memcpy( &fpFonts[BOLD].lf, &fpFonts[STANDARD].lf, sizeof( LOGFONT ) );
	fpFonts[BOLD].lf.lfWeight = 700;
	fpFonts[BOLD].hfHandle = CreateFontIndirect( &fpFonts[BOLD].lf );
	if( !fpFonts[BOLD].hfHandle )
		os_fatal( "Could not allocate bold version of the standard font!" );
	GetObject( fpFonts[BOLD].hfHandle, sizeof( LOGFONT ), &fpFonts[BOLD].lf);

	// Set up the standard emphasis + bold font (combo font, not actually required by the spec )
	if( fpFonts[EMPH_BOLD].hfHandle )
		DeleteObject( fpFonts[EMPH_BOLD].hfHandle );
	fpFonts[EMPH_BOLD].hfHandle = NULL;
	memcpy( &fpFonts[EMPH_BOLD].lf, &fpFonts[STANDARD].lf, sizeof( LOGFONT ) );
	fpFonts[EMPH_BOLD].lf.lfWeight = 700;
	fpFonts[EMPH_BOLD].lf.lfItalic = TRUE;
	fpFonts[EMPH_BOLD].hfHandle = CreateFontIndirect( &fpFonts[EMPH_BOLD].lf );
	if( !fpFonts[EMPH_BOLD].hfHandle )
		os_fatal( "Could not allocate italic+bold combo version of the standard font!" );
	GetObject( fpFonts[EMPH_BOLD].hfHandle, sizeof( LOGFONT ), &fpFonts[EMPH_BOLD].lf);

	// Set up the standard underline (alternate emphasis) font
	if( fpFonts[UNDERLINE].hfHandle )
		DeleteObject( fpFonts[UNDERLINE].hfHandle );
	fpFonts[UNDERLINE].hfHandle = NULL;
	memcpy( &fpFonts[UNDERLINE].lf, &fpFonts[STANDARD].lf, sizeof( LOGFONT ) );
	fpFonts[UNDERLINE].lf.lfUnderline = TRUE;
	fpFonts[UNDERLINE].hfHandle = CreateFontIndirect( &fpFonts[UNDERLINE].lf );
	if( !fpFonts[UNDERLINE].hfHandle )
		os_fatal( "Could not allocate underline version of the standard font!" );
	GetObject( fpFonts[UNDERLINE].hfHandle, sizeof( LOGFONT ), &fpFonts[UNDERLINE].lf);


	/* Set up the fixed emphasis font */
	/*****************************************************************************************/
	if( fpFonts[FIXED_EMPHASIS].hfHandle )
		DeleteObject( fpFonts[FIXED_EMPHASIS].hfHandle );
	fpFonts[FIXED_EMPHASIS].hfHandle = NULL;
	memcpy( &fpFonts[FIXED_EMPHASIS].lf, &fpFonts[FIXED].lf, sizeof( LOGFONT ) );
	fpFonts[FIXED_EMPHASIS].lf.lfItalic = TRUE;
	
	fpFonts[FIXED_EMPHASIS].lf.lfWidth = fpFonts[FIXED].nWidths['X'];

	fpFonts[FIXED_EMPHASIS].hfHandle = CreateFontIndirect( &fpFonts[FIXED_EMPHASIS].lf );
	if( !fpFonts[FIXED_EMPHASIS].hfHandle )
		os_fatal( "Could not allocate a required font!" );

	/* Ok, it's possible that when we grab the emphasis font we will end up with a fixed font
	   that doesn't match the original fixed in width. The Z-machine really does not handle 
	   this, so if that happened we just switch to using color for this style instead of the
	   new font style */
	oldFont = SelectObject( hdc, fpFonts[FIXED_EMPHASIS].hfHandle );
	GetTextMetrics( hdc, &tm );
	if( tm.tmAveCharWidth != fpFonts[FIXED].nWidths['X'] )
	{
		SelectObject( hdc, oldFont );
		DeleteObject( fpFonts[FIXED_EMPHASIS].hfHandle );
		fpFonts[FIXED_EMPHASIS].nUsage = DISPLAY_COLOR;
		memcpy( &fpFonts[FIXED_EMPHASIS].lf, &fpFonts[FIXED].lf, sizeof( LOGFONT ) );
		fpFonts[FIXED_EMPHASIS].hfHandle = CreateFontIndirect( &fpFonts[FIXED_EMPHASIS].lf );
		if( !fpFonts[FIXED_EMPHASIS].hfHandle )
			os_fatal( "Could not allocate a required font!" );
	}
	else
		SelectObject( hdc, oldFont );
	GetObject( fpFonts[FIXED_EMPHASIS].hfHandle, sizeof( LOGFONT ), &fpFonts[FIXED_EMPHASIS].lf);
	/*****************************************************************************************/
	
	// Set up the fixed bold font
	/*****************************************************************************************/
	if( fpFonts[FIXED_BOLD].hfHandle )
		DeleteObject( fpFonts[FIXED_BOLD].hfHandle );
	fpFonts[FIXED_BOLD].hfHandle = NULL;
	memcpy( &fpFonts[FIXED_BOLD].lf, &fpFonts[FIXED].lf, sizeof( LOGFONT ) );
	fpFonts[FIXED_BOLD].lf.lfWeight = 700;

	fpFonts[FIXED_BOLD].lf.lfWidth = fpFonts[FIXED].nWidths['X'];
	
	fpFonts[FIXED_BOLD].hfHandle = CreateFontIndirect( &fpFonts[FIXED_BOLD].lf );
	if( !fpFonts[FIXED_BOLD].hfHandle )
		os_fatal( "Could not allocate a required font!" );

	/* Same problem as FIXED_EMPHASIS example */
	oldFont = SelectObject( hdc, fpFonts[FIXED_BOLD].hfHandle );
	GetTextMetrics( hdc, &tm );
	if( tm.tmAveCharWidth != fpFonts[FIXED].nWidths['X'] )
	{
		SelectObject( hdc, oldFont );
		DeleteObject( fpFonts[FIXED_BOLD].hfHandle );
		fpFonts[FIXED_BOLD].nUsage = DISPLAY_COLOR;
		memcpy( &fpFonts[FIXED_BOLD].lf, &fpFonts[FIXED].lf, sizeof( LOGFONT ) );
		fpFonts[FIXED_BOLD].lf.lfWidth = fpFonts[FIXED].nWidths['X'];
		fpFonts[FIXED_BOLD].hfHandle = CreateFontIndirect( &fpFonts[FIXED_BOLD].lf );
		if( !fpFonts[FIXED_BOLD].hfHandle )
			os_fatal( "Could not allocate a required font!" );
	}
	else
		SelectObject( hdc, oldFont );
	GetObject( fpFonts[FIXED_BOLD].hfHandle, sizeof( LOGFONT ), &fpFonts[FIXED_BOLD].lf);
	/*****************************************************************************************/


	// Set up the fixed emphasis + bold font (combo font, not actually required by the spec )
	/*****************************************************************************************/
	if( fpFonts[FIXED_EMPH_BOLD].hfHandle )
		DeleteObject( fpFonts[FIXED_EMPH_BOLD].hfHandle );
	fpFonts[FIXED_EMPH_BOLD].hfHandle = NULL;
	memcpy( &fpFonts[FIXED_EMPH_BOLD].lf, &fpFonts[FIXED].lf, sizeof( LOGFONT ) );
	fpFonts[FIXED_EMPH_BOLD].lf.lfWeight = 700;
	fpFonts[FIXED_EMPH_BOLD].lf.lfItalic = TRUE;

	fpFonts[FIXED_EMPH_BOLD].lf.lfWidth = fpFonts[FIXED].nWidths['X'];

	fpFonts[FIXED_EMPH_BOLD].hfHandle = CreateFontIndirect( &fpFonts[FIXED_EMPH_BOLD].lf );
	if( !fpFonts[FIXED_EMPH_BOLD].hfHandle )
		os_fatal( "Could not allocate italic+bold combo version of the standard font!" );
	
	oldFont = SelectObject( hdc, fpFonts[FIXED_EMPH_BOLD].hfHandle );
	GetTextMetrics( hdc, &tm );
	if( tm.tmAveCharWidth != fpFonts[FIXED].nWidths['X'] )
	{
		SelectObject( hdc, oldFont );
		DeleteObject( fpFonts[FIXED_EMPH_BOLD].hfHandle );
		fpFonts[FIXED_EMPH_BOLD].nUsage = DISPLAY_COLOR;
		/* Yet another special case (an extra special case?). Generally when a font fails
		   because of incorrect widths, it's just bold that fails - emphasis works. So the
		   best choice here for FIXED+EMPHASIS is to copy the base type of _emphasis_, then
		   set it to a special color (since bold is too wide). Yippeee */
		memcpy( &fpFonts[FIXED_EMPH_BOLD].lf, &fpFonts[FIXED_EMPHASIS].lf, sizeof( LOGFONT ) );
		fpFonts[FIXED_EMPH_BOLD].hfHandle = CreateFontIndirect( &fpFonts[FIXED_EMPH_BOLD].lf );
		if( !fpFonts[FIXED_EMPH_BOLD].hfHandle )
			os_fatal( "Could not allocate a required font!" );
	}
	else
		SelectObject( hdc, oldFont );
	GetObject( fpFonts[FIXED_EMPH_BOLD].hfHandle, sizeof( LOGFONT ), &fpFonts[FIXED_EMPH_BOLD].lf);
	/*****************************************************************************************/


	// Set up the fixed underline (alternate emphasis) font
	/*****************************************************************************************/
	if( fpFonts[FIXED_UNDERLINE].hfHandle )
		DeleteObject( fpFonts[FIXED_UNDERLINE].hfHandle );
	fpFonts[FIXED_UNDERLINE].hfHandle = NULL;
	memcpy( &fpFonts[FIXED_UNDERLINE].lf, &fpFonts[FIXED].lf, sizeof( LOGFONT ) );
	fpFonts[FIXED_UNDERLINE].lf.lfUnderline = TRUE;
	fpFonts[FIXED_UNDERLINE].hfHandle = CreateFontIndirect( &fpFonts[FIXED_UNDERLINE].lf );
	if( !fpFonts[FIXED_UNDERLINE].hfHandle )
		os_fatal( "Could not allocate a required font!" );
	GetObject( fpFonts[FIXED_UNDERLINE].hfHandle, sizeof( LOGFONT ), &fpFonts[FIXED_UNDERLINE].lf);
	/*****************************************************************************************/
	//LOG_EXIT
}

/****************************************************************************
 *	FUNCTION: resetBitmap (void)											*
 *																			*
 *	PURPOSE:  Erase the bitmap that represents the screen, make sure erase	*
 *	brush itself is correct color.											*
 *																			*
 ***************************************************************************/
void resetBitmap( void )
{
	//LOG_ENTER
	if( bkgBrushcolor != user_background || display < MCGA_MODE )
	{
		if( startBrush )
			SelectObject( h_memDC, startBrush );
		if( bkgBrush )
			DeleteObject( bkgBrush );
		startBrush = bkgBrush = NULL;
		
		if( display < MCGA_MODE )
		{
			bkgBrush = CreateSolidBrush( RGB( 0, 0, 0) );
			bkgBrushcolor = RGB( 0, 0, 0);
		}
		else
		{
			bkgBrush = CreateSolidBrush( user_background );
			bkgBrushcolor = user_background;
		}
		if( !bkgBrush )
			os_fatal( "Could not allocate background brush" );
		
		startBrush = SelectObject( h_memDC, bkgBrush );
		if( !startBrush )
			os_fatal( "Could not select background brush" );
	}

	PATBLT( h_memDC, 0, 0, bmapX, bmapY, PATCOPY);
	nScrollPos = 0;
	//LOG_EXIT
}

/****************************************************************************
 *	FUNCTION: StartBitmap (void)											*
 *																			*
 *	PURPOSE:  Allocate the bitmap for the screen, select into the memory	*
 *	context used for drawing												*
 *																			*
 *	COMMENTS:																*
 *	Note that the screen buffer is always 2 times the actual Y-axis. This	*
 *	is to accomodate the "rolling window" used for faster screen updates.	*
 *	When drawing text, WinFrotz will simply roll the view window down the	*
 *	length of the bitmap, saving it from having to do constant BitBlts to	*
 *	scroll. This scroll view point is controlled by the variable nScrollPos	*
 ***************************************************************************/
static BOOL StartBitmap( )
{
	RECT		rcDisplay;

	//LOG_ENTER
	if( !hWnd )
	{
	//LOG_EXIT
		return FALSE;
	}

	GetClientRect( hWnd, &rcDisplay );

	bmapY = (rcDisplay.bottom - rcDisplay.top) * 2;
	bmapX = rcDisplay.right - rcDisplay.left;
	if( hbmScreenBuffer )
	{
		DeleteObject( hbmScreenBuffer );
		hbmScreenBuffer = NULL;
	}

	if( display < MCGA_MODE )
		hbmScreenBuffer = CreateBitmap( bmapX, bmapY, 1, 1, NULL );
	else
		hbmScreenBuffer = CreateCompatibleBitmap( hdc, bmapX, bmapY);

	if( !hbmScreenBuffer )
	{
		MessageBox( hWnd, "Couldn't create bitmap buffer!", "WinFrotz", MB_ICONSTOP );
		bStoryInit = FALSE;
		PostMessage( hWnd, WM_CLOSE, 0, 0L );
	//LOG_EXIT
		return FALSE;
	}
	
	if( oldMemBmap )
		SelectObject( h_memDC, oldMemBmap );
	else
		h_memDC = CreateCompatibleDC( hdc );

	oldMemBmap = SelectObject( h_memDC, hbmScreenBuffer );

	resetBitmap(); 

	//LOG_EXIT
	return TRUE;
}

/****************************************************************************
 *	FUNCTION: adjustScreenSize (void)										*
 *																			*
 *	PURPOSE:  Inform the Z-machine as well as possible when the user messes	*
 *	with screen geometry.													*
 *																			*
 *	COMMENTS:																*
 *	The status area represents a sticky problem. It can't be resized, since	*
 *	the Z-machine makes lots of assumptions about it. So in MainFrm.cpp		*
 *	resizing behavoir is limited (see OnGetMinMaxInfo) to avoid truly		*
 *	abusing the status area. Here we need to determine it's size to			*
 *	correctly alert the Z-machine to new screen rows/cols. Also all font	*
 *	sizes (as pertaining to geometry) have to be computed from scratch		*
 *	because we may BE here as a result of a font change.					*
 ***************************************************************************/
void adjustScreenSize( void )
{
	//LOG_ENTER
	if( bStoryInit )
	{
		h_font_height = (zbyte)fpFonts[FIXED].nHeight;
		h_font_width = (zbyte)fpFonts[FIXED].nWidths['X'];
		h_screen_rows = h_screen_height / h_font_height;
		h_screen_cols = h_screen_width / h_font_width;

		/*The status area needs to be refreshed when font changes occur, so its characters
		  are stored in a character array. Works great for it but not for window 0 (the 
		  primary display area) because window 0 doesn't necessary follow fixed spacing 
		  rules and placement															*/
		if( h_version != V6 && wp[1].x_size && !statusmem )
		{
			statusmem = calloc( 1, 255 * (h_screen_height / fpFonts[FIXED].nHeight) + 1);
			status_style = calloc( 1, 255 * (h_screen_height / fpFonts[FIXED].nHeight) + 1);
			if( !statusmem || !status_style )
				os_fatal( "Could not create window buffer memory" );
		}
		wp[1].font_size = (unsigned short)(fpFonts[FIXED].nHeight << 8 | fpFonts[FIXED].nWidths['X'] );
		wp[7].font_size = wp[1].font_size;

		/* Prepare status line */
		if (h_version <= V3) 
		{
			wp[7].x_size = h_screen_width;
			wp[7].y_size = (short)fpFonts[FIXED].nHeight;
		}

		wp[0].y_size = h_screen_height - wp[1].y_size - wp[7].y_size;
		/*DANGER YOUNG WILL ROBINSON! TODO: What if wp[0].y_pos is NOT supposed to be at
		the bottom of the status window???	*/
		wp[0].y_pos = wp[1].y_size + wp[7].y_size;
		wp[0].x_size = h_screen_width;
		wp[0].font_size = (unsigned short)(fpFonts[STANDARD].nHeight << 8 | fpFonts[STANDARD].nWidths['X']);

//		os_set_font (TEXT_FONT);

		restart_header ();
    
		/* Prepare lower window */
		wp[0].x_size = h_screen_width;

	    if (h_version <= V3 && status_shown )
		{
			int saved_font = current_font;
			int saved_style = current_style;
			z_show_status ();
			os_set_font( saved_font );
			os_set_text_style( saved_style );
		}
	}
	//LOG_EXIT
}

/****************************************************************************
 *	FUNCTION: drawStatusArea (void)											*
 *																			*
 *	PURPOSE:  Redraw the status window after a major screen change (font	*
 *	size, etc).																*
 *																			*
 *	COMMENTS:																*
 *	As noted previously contents of the status window are stored in a char	*
 *	array. The only formatting information recorded is normal or reverse	*
 *	text; normal chars are in the printable 0-127 range, reverse text has	*
 *	127 added to the ascii value. This isn't Z-machine spec compliant, it	*
 *	appears possible one could print in italics etc to the status area. Ah	*
 *	well.....																*
 ***************************************************************************/
void drawStatusArea( void )
{
	int saved_y, saved_x, c, count=0, saved_style, saved_win, width, height;

	//LOG_ENTER
	width = fpFonts[FIXED].nWidths['X'];
	height = fpFonts[FIXED].nHeight;
	saved_style = current_style;
	saved_win = cwin;
	saved_x = curCoord.X;
	saved_y = curCoord.Y;
	os_set_cursor( 1, 1 );

	if( !bStoryInit )
	{
	//LOG_EXIT
		return;
	}

	if( h_version == 3 )
		z_show_status();
	else
	{
		cwin = 1;
		os_set_text_style( current_style );
		for( curCoord.Y = 0; curCoord.Y <= wp[1].y_size; curCoord.Y += height )
		{
			count = (curCoord.Y / height) * 255;
			for( curCoord.X = 0; curCoord.X < wp[1].x_size; curCoord.X += width)
			{
				if( current_style != status_style[ count ] )
					os_set_text_style( status_style[ count ] );
				c = (zchar)statusmem[ count++ ];
				if( c )
				{
					os_display_char( (zchar)c );
					curCoord.X -= width;
				}
			}
		}
	}
	InvalidateRect( hWnd, NULL, FALSE );
	current_style = saved_style;
	cwin = saved_win;
	os_set_text_style( current_style );
	curCoord.X = saved_x;
	curCoord.Y = saved_y;
	os_set_cursor( saved_y + 1, saved_x + 1 );
	//LOG_EXIT
}

/****************************************************************************
 *	FUNCTION: init_thread (void)											*
 *																			*
 *	PURPOSE:  The top of the Z-machine thread, "main" if you will			*
 *																			*
 *	COMMENTS:																*
 *	Note that a couple of variables have been imported from foreign Frotz	*
 *	modules and are zero'd here (notably "locked"). The reason for this is	*
 *	children threads (such as the Z-Machine here) inherent the data space	*
 *	of the parent, including non-dynamic variable contents. This includes	*
 *	as one would figure globals, but you might NOT guess that it also		*
 *	includes statics, which are implicitly global. Thus some of these		*
 *	statics have been marked truly global and are reset here (they are		*
 *	never reset in Frotz, because there is only one thread there and they	*
 *	are created/destroyed at run time each time)							*
 ***************************************************************************/
void init_thread( void )
{
	RECT		rcDisplay;

	//LOG_ENTER
	bbpos = 0;
	memset( &ScrollRoll[0], 0, sizeof( char * )*100 );
	nScrollBuffs = 0;
	GetClientRect( hWnd, &rcDisplay );
	//h_font_height = (zbyte)max( fpFonts[FIXED].nHeight, fpFonts[STANDARD].nHeight );
	h_font_height = (zbyte)fpFonts[FIXED].nHeight;
	h_font_width = (zbyte)fpFonts[FIXED].nWidths['X'];
		
	h_screen_width = (zword)(rcDisplay.right - rcDisplay.left);
	h_screen_height = (zword)(rcDisplay.bottom - rcDisplay.top);
	h_screen_rows = h_screen_height / h_font_height;
	h_screen_cols = h_screen_width / h_font_width;

	zcodeops = 0;
	zcodestart = 0;
	zcodetime = 0;
	nScrollPos = 0;
	status_last_char = 0;
	status_shown = 0;

	/* Frotz variables that need to be reset here */
	locked = 0;
	finished = 0;

	init_memory ();

	init_buffer_vars();

	/* We can't know this until the Z-machine has run: we're loading a V6 game,
	   so send a message off to the GUI thread and go to sleep until it has
	   re-arranged the screen to handle the new requirements. WinFrotz allows 
	   for completely different screen geometry between regular and V6 games	*/
	if( h_version == V6 )
	{
		int i;

		display = graphics_mode;

		PostMessage( hWnd, WM_VERSION_6, (WPARAM)0, (LPARAM)0L );

		SuspendThread( hThread );

		// Set up the Version 6 font
		for( i=0; i < GRAPHICS; i++ )
		{
			if( fpFonts[i].hfHandle )
				DeleteObject( fpFonts[STANDARD].hfHandle );
		}

		memcpy( &fpFonts[STANDARD].lf, &fpFonts[GRAPHICS].lf, sizeof( LOGFONT ) );
		fpFonts[STANDARD].hfHandle = CreateFontIndirect( &fpFonts[STANDARD].lf );
		memcpy( &fpFonts[FIXED].lf, &fpFonts[GRAPHICS].lf, sizeof( LOGFONT ) );
		fpFonts[FIXED].hfHandle = CreateFontIndirect( &fpFonts[FIXED].lf );

		StartBitmap( );

		if( hgdiOldFont )
			SelectObject( h_memDC, hgdiOldFont );
		_ASSERT( fpFonts[STANDARD].hfHandle );
		hgdiOldFont = SelectObject( h_memDC, fpFonts[STANDARD].hfHandle );
		_ASSERT( hgdiOldFont );

	}
	else
	{
		display = reg_display;
		StartBitmap( );
	}

	lpbmi = (LPBITMAPINFO)malloc( sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * 226 );
	if( !lpbmi )
		os_fatal( "Could not allocate bitmap info header" );

	AddStyleFonts();

	ComputeFontWidths();

	current_font_width = &fpFonts[STANDARD].nWidths[0];

	adjustScreenSize();

    os_init_screen ();

    init_undo ();

    z_restart ();

	PostMessage( hWnd, WM_SET_ICON_TYPE, 0, 0L );

	zcodestart = timeGetTime();

    interpret ();

    reset_memory ();

	zmp = NULL;

    os_reset_screen ();

	PATBLT( h_memDC, 0, 0, bmapX, bmapY, PATCOPY);

	cleanup();
	//LOG_EXIT
}

void cleanup( void )
{
	//LOG_ENTER
	PostMessage( hWnd, WM_CURSOR_HIDE, WM_CURSOR_HIDE, 0 );

	if( zmp )
		os_reset_screen();

	DestroyMemPalette();

	while( nScrollBuffs > 0 )
		free( ScrollRoll[--nScrollBuffs] );

	if( statusmem )
		free( statusmem );
	statusmem = NULL;
	if( status_style )
		free( status_style );
	status_style = NULL;
	status_last_char = 0;

	if( h_memDC )
	{
		os_erase_area( 1, 1,  h_screen_height + 1, h_screen_width + 1);
		if( startBrush )
			SelectObject( h_memDC, startBrush );
		startBrush = NULL;
		if( hgdiOldFont )
			SelectObject( h_memDC, hgdiOldFont );
		if( bkgBrush )
			DeleteObject( bkgBrush );
		bkgBrush = NULL;
	}

	if( lpbmi )
		free( lpbmi );
	lpbmi = NULL;

	if( zmp )
	{
		reset_memory ();
		
		zmp = NULL;
	}

	ReleaseDC( hWnd, hdc );

	PostMessage( hWnd, WM_THREAD_DONE, WM_THREAD_DONE, 0 );
	
	nState = FINISHED;
	
	if( hThread )
		SuspendThread( hThread );

#ifdef _DEBUG
	MessageBox( hWnd, "Z-machine is closing", "WinFrotz", MB_ICONSTOP );
#endif

	//LOG_EXIT
	exit( 0 );
}
#endif
