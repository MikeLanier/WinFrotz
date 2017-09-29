/*
 * Interface.c
 *
 * IO interface for Win95/NT Console app
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include "../frotz/frotz.h"
#include "..\include\logfile_c.h"
#include <mmsystem.h>
#include <wingdi.h>
#include <crtdbg.h>
#include "../include/windefs.h"
#ifndef	_CONSOLE
#include "../include/crosscomp.h"
#endif

int current_style = 0;
int current_font = TEXT_FONT;
int current_fg = -1;
int current_bg = -1;
extern char szBigBuffer[];
extern int bbpos;

/*	The current drawing location											*/
#ifdef _CONSOLE
COORD				curCoord;
extern	HANDLE				hConsoleOutput, hConsoleInput;
#else
COLORREF text_fg;
COLORREF text_bg;

fakecoord			curCoord;

/*	fontprops is described in crosscomp.h. current_font_style is used to	
	keep track of what the _last_ font style was so we can know when to load
	the "STANDARD" font back in (Windows has to explicitly load each font 
	style, it saves time to remember which one we have and not load it if
	is already being used)													*/
fontprops	fpFonts[NUM_FONT_TYPES];

#endif

int			current_font_style;

#ifdef BUFFERED_TEXTOUT
static unsigned int	nExpectedY=0, nExpectedX=0, nStartX=0, buffout_index=0, buffout_xsize=0;
static char			buffered_output[255];
#endif

/*	current_font_width is a pointer to the array of character sizes for the
	current font. It can be used as an equvalent to what Frotz thinks of as
	os_char_width(). current_font_height is similar but for font height		*/
int *current_font_width;
int current_font_height = 1;


#ifdef _CONSOLE
int user_foreground = 9;
int user_background = 6;
int user_reverse_fg = -1;
int user_reverse_bg = -1;
int user_emphasis = 13;
int text_fg = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
int text_bg = BACKGROUND_BLUE;
int pc_colour_fg[18] = {
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,	/*WHITE*/
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,	/*WHITE*/
	0,														/*BLACK*/
	FOREGROUND_RED,											/*RED*/
	FOREGROUND_GREEN,										/*GREEN*/
	FOREGROUND_RED | FOREGROUND_GREEN,						/*YELLOW*/
	FOREGROUND_BLUE,										/*BLUE*/
	FOREGROUND_BLUE | FOREGROUND_RED,						/*MAGENTA*/
	FOREGROUND_BLUE | FOREGROUND_GREEN,						/*CYAN*/
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,	/*WHITE*/
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,	/*DARK GREY*/
	FOREGROUND_RED | FOREGROUND_INTENSITY,					/*RED*/
	FOREGROUND_GREEN | FOREGROUND_INTENSITY,				/*GREEN*/
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,/*YELLOW*/
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE| FOREGROUND_INTENSITY,					/*BLUE*/
	FOREGROUND_BLUE | FOREGROUND_GREEN| FOREGROUND_INTENSITY,/*MAGENTA*/
	FOREGROUND_BLUE | FOREGROUND_RED| FOREGROUND_INTENSITY,	/*CYAN*/
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE| FOREGROUND_INTENSITY/*BRIGHT WHITE*/
};

int pc_colour_bg[18] = {
	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,	/*WHITE*/
	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,	/*WHITE*/
	0,														/*BLACK*/
	BACKGROUND_RED,											/*RED*/
	BACKGROUND_GREEN,										/*GREEN*/
	BACKGROUND_RED | BACKGROUND_GREEN,						/*YELLOW*/
	BACKGROUND_BLUE,										/*BLUE*/
	BACKGROUND_BLUE | BACKGROUND_RED,						/*MAGENTA*/
	BACKGROUND_BLUE | BACKGROUND_GREEN,						/*CYAN*/
	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,	/*WHITE*/
	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,	/*DARK GREY*/
	BACKGROUND_RED | BACKGROUND_INTENSITY,					/*RED*/
	BACKGROUND_GREEN | BACKGROUND_INTENSITY,				/*GREEN*/
	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY,/*YELLOW*/
	BACKGROUND_BLUE | BACKGROUND_INTENSITY,					/*BLUE*/
	BACKGROUND_BLUE ,/*MAGENTA*/
	BACKGROUND_BLUE | BACKGROUND_RED| BACKGROUND_INTENSITY,	/*CYAN*/
	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE| BACKGROUND_INTENSITY/*BRIGHT WHITE*/
};
#else
COLORREF pc_colour_fg[18] = {
	RGB(0,0,0),			/*BLACK*/
	RGB(0,0,0),			/*BLACK*/
	RGB(0,0,0),			/*BLACK*/
	RGB(160,0,0),		/*RED*/
	RGB(0,167,0),		/*GREEN*/
	RGB(255,248,80),	/*YELLOW*/
	RGB(0,0,160),		/*BLUE*/
	RGB(160,0,160),		/*MAGENTA*/
	RGB(0,167,160),		/*CYAN*/
	RGB(255,255,255),	/*WHITE*/
	RGB(64,64,64),		/*DARK GREY*/
	RGB(0,0,128),		/*DARK BLUE */
	RGB(255,255,255),	/*BRIGHT WHITE*/
	RGB(0,0,0),			/*BLACK*/
	RGB(0,0,0),			/*USER FOREGROUND (14)*/
	RGB(0,0,0),			/*UESR BACKGROUND (15)*/
	RGB(0,0,0),			/*BLACK*/
	RGB(0,0,0)			/*SPECIAL COLOR: used to save pixel color */
};

COLORREF *pc_colour_bg = &pc_colour_fg[0];
COLORREF user_foreground = RGB(192,192,192);
COLORREF user_background = RGB(0,0,128);
COLORREF user_reverse_fg = RGB(0,0,128);
COLORREF user_reverse_bg= RGB(192,192,192);
COLORREF user_emphasis = RGB(255,255,0);
COLORREF user_bold = RGB( 255, 255, 255 );
COLORREF text_fg;
COLORREF text_bg;

extern	void	resetScrollPos( void );
extern	HDC		hdc, h_memDC;
extern	HGDIOBJ	hgdiOldFont;
extern	int		display;
extern	long	nScrollPos;
extern	char	*statusmem, *status_style;
extern	int		status_last_char;
extern	int		nUpdates;
#ifdef BUFFERED_TEXTOUT
BOOL	bRefresh = TRUE;
#endif
#endif

extern	HWND	hWnd;

#ifdef BUFFERED_TEXTOUT
void clear_output_buffer( void )
{
	//LOG_ENTER
#ifdef _CONSOLE
	if( buffout_index )
	{
		int	numwritten;
		COORD	temp = { nStartX, nExpectedY };

		SetConsoleCursorPosition( hConsoleOutput, temp );
		WriteConsole( hConsoleOutput, (LPSTR)buffered_output, buffout_xsize, &numwritten, NULL );
#else
	if( buffout_index )
	{
		TEXTOUT( h_memDC, nStartX, nExpectedY + nScrollPos, buffered_output, buffout_index );
		if( nUpdates > 1 )
		{
			BITBLT( hdc, nStartX, nExpectedY, buffout_xsize, current_font_height, h_memDC, nStartX, nExpectedY + nScrollPos, SRCCOPY);
			//TEXTOUT( hdc, nStartX, nExpectedY + nScrollPos, buffered_output, buffout_index );
		}
#endif
		buffered_output[0] = 0;
		buffout_index = buffout_xsize = 0;
		nExpectedY = curCoord.Y;
		nStartX = curCoord.X;
	}
	//LOG_EXIT
}
#endif

extern char *euro_substitute;

/*
 * os_font_data
 *
 * Return true if the given font is available. The font can be
 *
 *    TEXT_FONT
 *    PICTURE_FONT
 *    GRAPHICS_FONT
 *    FIXED_WIDTH_FONT
 *
 * The font size should be stored in "height" and "width". If
 * the given font is unavailable then these values must _not_
 * be changed.
 *
 */

int os_font_data (int font, int *height, int *width)
{
	//LOG_ENTER

    /* Not every font is available in every mode */
#ifndef _CONSOLE
    if (font == GRAPHICS_FONT )
	{
		if( !fpFonts[ZORK_FONT].hfHandle )
		{
			*height = fpFonts[FIXED].nHeight;
			*width = fpFonts[FIXED].nWidths['X'];
	//LOG_EXIT
			return 0;
		}
		else
		{
			*height = fpFonts[ZORK_FONT].nHeight;
			*width = fpFonts[ZORK_FONT].nWidths['X'];
	//LOG_EXIT
			return 1;
		}
	}

	if( font == TEXT_FONT )			
	{
		*height = fpFonts[STANDARD].nHeight;
		*width = fpFonts[STANDARD].nWidths['X'];
	}
	else
	{
		*height = fpFonts[FIXED].nHeight;
		*width = fpFonts[FIXED].nWidths['X'];
	}
#else
    *height = 1;
    *width = 1;

#endif

	//LOG_EXIT
    return 1;

}/* os_font_data */

/****************************************************************************/
/*	I found it easier if I split adjust_style into adjust_text_color and		
	os_set_text_style (which calls adjust_text_color anyway). It ends up
	jumping around a little less and clearly defines what each is doing.	*/
/****************************************************************************/
static void adjust_text_color( void )
{
	//LOG_ENTER
   /* Convert Z-machine colours to IBM PC colours */

#ifdef BUFFERED_TEXTOUT
	clear_output_buffer();
#endif

#ifdef _CONSOLE
	/* Handle reverse text style */

	if (current_style & REVERSE_STYLE) 
	{
		text_fg = (user_reverse_fg != -1) ? pc_colour_fg[user_reverse_fg] : pc_colour_fg[current_bg];
		text_bg = (user_reverse_bg != -1) ? pc_colour_bg[user_reverse_bg] : pc_colour_bg[current_fg];
	}
	else
	{
		text_fg = pc_colour_fg[current_fg];
		text_bg = pc_colour_bg[current_bg];
	}

    /* Handle emphasis style */
    if (current_style & EMPHASIS_STYLE)
		text_fg = pc_colour_fg[user_emphasis];
	
    /* Handle boldface style */
    if (current_style & BOLDFACE_STYLE) 
		text_fg = text_fg | FOREGROUND_INTENSITY;

    /* Set the screen attribute  */
	SetConsoleTextAttribute( hConsoleOutput, (WORD)(text_fg | text_bg) );	
#else
	if( display > CGA_MODE )
	{
		/* Handle reverse text style */

		if (current_style & REVERSE_STYLE) 
		{
			text_fg = (user_reverse_fg != -1) ? user_reverse_fg : pc_colour_bg[current_bg];
			text_bg = (user_reverse_bg != -1) ? user_reverse_bg : pc_colour_fg[current_fg];
			if( !cwin || h_version == V6 )
			{
				text_fg = pc_colour_bg[current_bg];
				text_bg = pc_colour_fg[current_fg];
			}
		}
		else
		{
			text_fg = pc_colour_fg[current_fg];
			text_bg = pc_colour_bg[current_bg];
		}
	}
	else //MONO (2-color bitmap) stuff
	{
		/*	In dual color all colors are either "on" or "off". When they are
			blitted to the screen "on" becomes foreground and "off" becomes
			background (standard Windows behavior for mono bitmaps)			*/
		if( current_fg > 0 && current_fg!=15 )
			text_fg = RGB( 255, 255, 255 );
		else
			text_fg = RGB( 0, 0, 0 );

		if( current_bg > 1 && text_fg!=RGB( 255, 255, 255 ) )
			text_bg = RGB( 255, 255, 255 );
		else
			text_bg = RGB( 0, 0, 0 );

		if( current_style & REVERSE_STYLE )
		{
			text_fg = RGB( 0, 0, 0 );
			text_bg = RGB( 255, 255, 255 );
		}
	}
//	if( current_style & REVERSE_STYLE )
		SetBkMode( h_memDC, OPAQUE);
//	else
//		SetBkMode( h_memDC, TRANSPARENT);
	SetBkColor( h_memDC, text_bg );
	SetTextColor( h_memDC, text_fg);
#endif
	//LOG_EXIT
}

/*
 * os_set_colour
 *
 * Set the foreground and background colours which can be:
 *
 *     DEFAULT_COLOUR
 *     BLACK_COLOUR
 *     RED_COLOUR
 *     GREEN_COLOUR
 *     YELLOW_COLOUR
 *     BLUE_COLOUR
 *     MAGENTA_COLOUR
 *     CYAN_COLOUR
 *     WHITE_COLOUR
 *
 *     MS-DOS 320 columns MCGA mode only:
 *
 *     GREY_COLOUR
 *
 *     Amiga only:
 *
 *     LIGHTGREY_COLOUR
 *     MEDIUMGREY_COLOUR
 *     DARKGREY_COLOUR
 *
 * There may be more colours in the range from 16 to 255; see the
 * remarks on os_peek_colour.
 *
 */

/****************************************************************************/
/*	I use color value 17 to pass colors looked up via os_peek_colour - that
	function stores the color in array position 17 and then returns "17" as
	the color value, where it can be looked up later here					*/
/****************************************************************************/
void os_set_colour (int new_foreground, int new_background)
{
	//LOG_ENTER
    /* Save colours in global variables */

    current_fg = new_foreground;
    current_bg = new_background;

    /* Apply changes */
    adjust_text_color( );
	//LOG_EXIT
}/* os_set_colour */

/*
 * os_set_font
 *
 * Set the font for text output. The interpreter takes care not to
 * choose fonts which aren't supported by the interface.
 *
 */

void os_set_font (int new_font)
{
	//LOG_ENTER
#ifndef _CONSOLE 
	if( current_font == GRAPHICS_FONT )
	{
		SelectObject( h_memDC, hgdiOldFont );
		hgdiOldFont = SelectObject( h_memDC, fpFonts[STANDARD].hfHandle );
		current_font_width = &fpFonts[STANDARD].nWidths[0];
		current_font_height = fpFonts[STANDARD].nHeight;
	}
#endif
    /* Save font in global variable */
    current_font = new_font;

#ifndef _CONSOLE
	if( new_font == GRAPHICS_FONT && fpFonts[ZORK_FONT].hfHandle )
	{
		SelectObject( h_memDC, hgdiOldFont );
		hgdiOldFont = SelectObject( h_memDC, fpFonts[ZORK_FONT].hfHandle );
		current_font_width = &fpFonts[ZORK_FONT].nWidths[0];
		current_font_height = fpFonts[ZORK_FONT].nHeight;
	}
#endif
	//LOG_EXIT
}/* os_set_font */

/*
 * os_set_text_style
 *
 * Set the current text style. Following flags can be set:
 *
 *     REVERSE_STYLE
 *     BOLDFACE_STYLE
 *     EMPHASIS_STYLE (aka underline aka italics)
 *     FIXED_WIDTH_STYLE
 *
 */

/****************************************************************************/
/*	This function becomes complicated, because for each "style" WinFrotz has
	to switch to a completely new font (and select it into context). Equally,
	when a "style" is turned off, I need to load back in the standard font
	type. This is checked for by current_font_style, which shows which font
	style is selected, NOT which one is current to the Z-machine			*/
/****************************************************************************/
void os_set_text_style (int new_style)
{
	//LOG_ENTER
    /* Save style in global variable */
    current_style = new_style;
	
    adjust_text_color( );

	/* Many times the Z-Machine will call for a text style change even when the new style matches
	   the old. In addition, to WinFrotz the "styles" of regular and reverse fonts are the same,
	   as reverse is just a color operation, not a font style. So check here to see if we can just
	   return without traversing the big if block and doing the whole select thing, etc.			*/
	if( current_font_style == new_style || current_font_style == (new_style | REVERSE_STYLE) || current_font == GRAPHICS_FONT )
	{
	//LOG_EXIT
		return;
	}
#ifndef _CONSOLE
	_ASSERT( hgdiOldFont );

	if( new_style <= REVERSE_STYLE )
	{ 
		SelectObject( h_memDC, hgdiOldFont );
		_ASSERT( fpFonts[STANDARD].hfHandle );
		hgdiOldFont = SelectObject( h_memDC, fpFonts[STANDARD].hfHandle );
		current_font_style = new_style;
		current_font_width = &fpFonts[STANDARD].nWidths[0];
		current_font_height = fpFonts[STANDARD].nHeight;
	//LOG_EXIT
		return;
	}

	if( new_style & FIXED_WIDTH_STYLE )
	{
		current_font_width = &fpFonts[FIXED].nWidths[0];
		current_font_height = fpFonts[FIXED].nHeight;
		if( new_style == FIXED_WIDTH_STYLE || new_style == (FIXED_WIDTH_STYLE | REVERSE_STYLE))
		{
			SelectObject( h_memDC, hgdiOldFont );
			_ASSERT( fpFonts[FIXED].hfHandle );
			hgdiOldFont = SelectObject( h_memDC, fpFonts[FIXED].hfHandle );
			current_font_style = new_style;
	//LOG_EXIT
			return;
		}

		if( new_style & EMPHASIS_STYLE && new_style & BOLDFACE_STYLE )
		{
			if( fpFonts[FIXED_EMPH_BOLD].nUsage == DISPLAY_FONT )
			{
				SelectObject( h_memDC, hgdiOldFont );
				_ASSERT( fpFonts[FIXED_EMPH_BOLD].hfHandle );
				hgdiOldFont = SelectObject( h_memDC, fpFonts[FIXED_EMPH_BOLD].hfHandle );
			}
			else
			{
				SelectObject( h_memDC, hgdiOldFont );
				_ASSERT( fpFonts[FIXED_EMPHASIS].hfHandle );
				hgdiOldFont = SelectObject( h_memDC, fpFonts[FIXED_EMPHASIS].hfHandle );
				text_fg = user_bold;
				SetBkColor( h_memDC, text_bg );
				SetTextColor( h_memDC, text_fg);
			}
			current_font_style = new_style;
	//LOG_EXIT
			return;
		}

		if( new_style & EMPHASIS_STYLE )
		{
			if( fpFonts[FIXED_EMPHASIS].nUsage == DISPLAY_FONT )
			{
				SelectObject( h_memDC, hgdiOldFont );
				_ASSERT( fpFonts[FIXED_EMPHASIS].hfHandle );
				hgdiOldFont = SelectObject( h_memDC, fpFonts[FIXED_EMPHASIS].hfHandle );
			}
			else if( fpFonts[FIXED_EMPHASIS].nUsage == DISPLAY_FONT_UNDERLINE )
			{
				SelectObject( h_memDC, hgdiOldFont );
				_ASSERT( fpFonts[FIXED_UNDERLINE].hfHandle );
				hgdiOldFont = SelectObject( h_memDC, fpFonts[FIXED_UNDERLINE].hfHandle );
			} 
			else
			{
				if( fpFonts[FIXED_EMPHASIS].nUsage == DISPLAY_COLOR )
					text_fg = user_emphasis;
				SelectObject( h_memDC, hgdiOldFont );
				_ASSERT( fpFonts[STANDARD].hfHandle );
				hgdiOldFont = SelectObject( h_memDC, fpFonts[STANDARD].hfHandle );
				SetBkColor( h_memDC, text_bg );
				SetTextColor( h_memDC, text_fg);
			}
			current_font_style = new_style;
	//LOG_EXIT
			return;
		} 
		
		if( new_style & BOLDFACE_STYLE ) 
		{
			SelectObject( h_memDC, hgdiOldFont );
			_ASSERT( fpFonts[FIXED_BOLD].hfHandle );
			hgdiOldFont = SelectObject( h_memDC, fpFonts[FIXED_BOLD].hfHandle );
			
			if( fpFonts[FIXED_BOLD].nUsage == DISPLAY_COLOR )
			{
				text_fg = user_bold;
				SetBkColor( h_memDC, text_bg );
				SetTextColor( h_memDC, text_fg);
			}
			current_font_style = new_style;
	//LOG_EXIT
			return;
		}

		DEBUGBOX( hWnd, "Unhandled style for standard font", "Font styles", MB_OK );
	//LOG_EXIT
		return;
	}
	else
	{	/* "Standard" (proportionate window 0) fonts */
		/* Handle emphasis style */
		current_font_height = fpFonts[STANDARD].nHeight;

		if( (new_style & BOLDFACE_STYLE) && (new_style & EMPHASIS_STYLE) )
		{
			if( fpFonts[BOLD].nUsage == DISPLAY_FONT )
			{
				SelectObject( h_memDC, hgdiOldFont );
				_ASSERT( fpFonts[EMPH_BOLD].hfHandle );
				hgdiOldFont = SelectObject( h_memDC, fpFonts[EMPH_BOLD].hfHandle );
				current_font_width = &fpFonts[EMPH_BOLD].nWidths[0];
			}
			else if( fpFonts[BOLD].nUsage == DISPLAY_COLOR )
			{
				current_font_width = &fpFonts[STANDARD].nWidths[0];
				text_fg = user_bold;
				SetBkColor( h_memDC, text_bg );
				SetTextColor( h_memDC, text_fg);
			}
			current_font_style = new_style;
	//LOG_EXIT
			return;
		}

		if( new_style & BOLDFACE_STYLE ) 
		{
			if( fpFonts[BOLD].nUsage == DISPLAY_FONT )
			{
				SelectObject( h_memDC, hgdiOldFont );
				_ASSERT( fpFonts[BOLD].hfHandle );
				hgdiOldFont = SelectObject( h_memDC, fpFonts[BOLD].hfHandle );
				current_font_width = &fpFonts[BOLD].nWidths[0];
			}
			else if( fpFonts[BOLD].nUsage == DISPLAY_COLOR )
			{
				current_font_width = &fpFonts[STANDARD].nWidths[0];
				text_fg = user_bold;
				SetBkColor( h_memDC, text_bg );
				SetTextColor( h_memDC, text_fg);
			}
			current_font_style = new_style;
	//LOG_EXIT
			return;
		} 
		
		if( new_style & EMPHASIS_STYLE )
		{
			if( fpFonts[EMPHASIS].nUsage == DISPLAY_FONT )
			{
				SelectObject( h_memDC, hgdiOldFont );
				_ASSERT( fpFonts[EMPHASIS].hfHandle );
				hgdiOldFont = SelectObject( h_memDC, fpFonts[EMPHASIS].hfHandle );
				current_font_width = &fpFonts[EMPHASIS].nWidths[0];
			}
			else if( fpFonts[EMPHASIS].nUsage == DISPLAY_FONT_UNDERLINE )
			{
				SelectObject( h_memDC, hgdiOldFont );
				_ASSERT( fpFonts[UNDERLINE].hfHandle );
				hgdiOldFont = SelectObject( h_memDC, fpFonts[UNDERLINE].hfHandle );
				current_font_width = &fpFonts[UNDERLINE].nWidths[0];
			} 
			else
			{
				if( fpFonts[EMPHASIS].nUsage == DISPLAY_COLOR )
					text_fg = user_emphasis;
				SelectObject( h_memDC, hgdiOldFont );
				_ASSERT( fpFonts[STANDARD].hfHandle );
				hgdiOldFont = SelectObject( h_memDC, fpFonts[STANDARD].hfHandle );
				current_font_width = &fpFonts[UNDERLINE].nWidths[0];
				SetBkColor( h_memDC, text_bg );
				SetTextColor( h_memDC, text_fg);
			}
			current_font_style = new_style;
	//LOG_EXIT
			return;
		} 
		DEBUGBOX( hWnd, "Unhandled style for standard font", "Font styles", MB_OK );
	}
#endif
	//LOG_EXIT
}/* os_set_text_style */

/*
 * os_display_char (Windows graphical version)
 *
 * Display a character of the current font using the current colours
 * and text style. The cursor moves to the next position. Printable
 * codes are all ASCII values from 32 to 126, European characters from
 * EURO_MIN to EURO_MAX (as defined in the Specification of the
 * Z-machine), character 9 (gap between two sentences) and character
 * 11 (paragraph indentation). The screen should not be scrolled after
 * printing to the bottom right corner.
 *
 */

#ifndef _CONSOLE 
void os_display_char (zchar c)
{
	//LOG_ENTER
    /* Handle special indentations */

    if (c == 9) 
	{
		os_display_char (' ');
		os_display_char (' ');
		c = ' ';
    }
    if (c == 11) 
	{
		os_display_char (' ');
		c = ' ';
    }

	/* Print character and return if text */

#ifdef BUFFERED_TEXTOUT
	if( curCoord.Y != (int)nExpectedY )
	{
		bRefresh = TRUE;
		clear_output_buffer();
		nExpectedY = curCoord.Y;
		nStartX = curCoord.X;
	}

	if( curCoord.X != (int)nExpectedX )
	{
		clear_output_buffer();
		nExpectedY = curCoord.Y;
		nStartX = curCoord.X;
	}
#endif

	_ASSERT( h_memDC );
	if( !cwin ) 
	{
#ifdef BUFFERED_TEXTOUT
		buffered_output[buffout_index++] = c;
#else
		TEXTOUT( h_memDC, curCoord.X, curCoord.Y + nScrollPos, (LPSTR)&c, 1 ); 
#endif
		szBigBuffer[bbpos++]=c;
	}
	else 
	{
		TEXTOUT( h_memDC, curCoord.X, curCoord.Y, (LPSTR)&c, 1 );
//		BITBLT( hdc, curCoord.X, curCoord.Y, current_font_width[c], current_font_height, h_memDC, curCoord.X, curCoord.Y, SRCCOPY);

		/*	Stuff printed to the status area is saved in a text array so it
			can be re-displayed later, if the user changes the status colors
			for instance													*/
		if( cwin==1 && statusmem )
		{
			int charpos = curCoord.X / fpFonts[FIXED].nWidths['X'];
			int loc = charpos + 255 * curCoord.Y / fpFonts[FIXED].nHeight;

			statusmem[ loc ] = c;
			status_style[ loc ] = current_style;
			if( !current_style )
				status_style[ loc ] = FIXED_WIDTH_STYLE;
			charpos++;
			if( charpos > status_last_char )
				status_last_char = charpos ;
		}
	}
	/* Move cursor to next position */
	curCoord.X += CURRENT_FONT_WIDTH(c);
#ifdef BUFFERED_TEXTOUT
	buffout_xsize += CURRENT_FONT_WIDTH(c);
	nExpectedX = curCoord.X;
#endif
	//LOG_EXIT
}/* os_display_char */
#endif

/*
 * os_display_char (Windows console version)
 *
 * Display a character of the current font using the current colours
 * and text style. The cursor moves to the next position. Printable
 * codes are all ASCII values from 32 to 126, European characters from
 * EURO_MIN to EURO_MAX (as defined in the Specification of the
 * Z-machine), character 9 (gap between two sentences) and character
 * 11 (paragraph indentation). The screen should not be scrolled after
 * printing to the bottom right corner.
 *
 */

#ifdef _CONSOLE
void os_display_char (zchar c)
{
	int	numwritten;

	//LOG_ENTER
    /* Handle special indentations */

    if (c == 9) 
	{
		os_display_char (' ');
		os_display_char (' ');
		c = ' ';
    }
    if (c == 11) 
	{
		os_display_char (' ');
		c = ' ';
    }

	/* Print character and return if text */

#ifdef BUFFERED_TEXTOUT
	if( curCoord.Y != (short)nExpectedY || curCoord.X != (short)nExpectedX )
	{
		clear_output_buffer();
		nExpectedY = curCoord.Y;
		nStartX = curCoord.X;
	}
	buffered_output[buffout_index++] = c;
	buffout_xsize++;
	nExpectedX = curCoord.X;
#else

	/* Ugly kludge to avoid strange console behavoir when printing to the last character
	   position. Setting hConsoleOutput all the time to 0 slows printing way down??? */
	if( curCoord.X < (h_screen_width-1) || curCoord.Y < (h_screen_height-1) )
		WriteConsole( hConsoleOutput, (LPSTR)&c, 1, &numwritten, NULL );
	else
	{
		SetConsoleMode( hConsoleOutput, 0 );
		WriteConsole( hConsoleOutput, (LPSTR)&c, 1, &numwritten, NULL );
		SetConsoleMode( hConsoleOutput, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT );
	}
    /* Move cursor to next position */
    curCoord.X++;
#endif
	//LOG_EXIT
}/* os_display_char */
#endif

/*
 * os_display_string
 *
 * Pass a string of characters to os_display_char.
 *
 */

void os_display_string (const zchar *s)
{

    int c;

	//LOG_ENTER
    while ((c = (unsigned char) *s++) != 0)

	if (c == ZC_NEW_FONT || c == ZC_NEW_STYLE) {

	    int arg = (unsigned char) *s++;

	    if (c == ZC_NEW_FONT)
		os_set_font (arg);
	    if (c == ZC_NEW_STYLE)
		os_set_text_style (arg);

	} else os_display_char ((zchar)c);

	//LOG_EXIT
} /* os_display_string */

/*
 * os_char_width 
 *
 * Return the length of the character in screen units.
 *
 */

int os_char_width (zchar c)
{
	//LOG_ENTER
#ifdef _CONSOLE
	//LOG_EXIT
	return 1;
#else
	//LOG_EXIT
	return( CURRENT_FONT_WIDTH(c) );
#endif
}
/* os_char_width */


/*
 * os_string_width
 *
 * Calculate the length of a word in screen units. Apart from letters,
 * the word may contain special codes:
 *
 *    ZC_NEW_STYLE - next character is a new text style
 *   ZC_NEW_FONT  - next character is a new font
 *
 */

int os_string_width (const zchar *s)
{
    int width = 0;
    int c;
#ifdef _CONSOLE
	//LOG_ENTER
    while ((c = (unsigned char) *s++) != 0)
	{
		if( c == 9 )
			width += 3;
		else if( c == 11 )
			width += 2;
		else if( c == ZC_NEW_STYLE || c ==ZC_NEW_FONT )
			*s++;
		else
			width++;
	}

	//LOG_EXIT
	return	width;
#else
	int	*font_width = current_font_width;
	int	font_type = current_font;

	//LOG_ENTER
    while ((c = (unsigned char) *s++) != 0)

	if (c == ZC_NEW_STYLE || c == ZC_NEW_FONT) {

	    int arg = (unsigned char) *s++;

	    if (c ==ZC_NEW_FONT)
		{
			font_type = arg;
			if( arg == FIXED_WIDTH_FONT )
				font_width = &fpFonts[FIXED].nWidths[0];
			else if( arg == GRAPHICS_FONT )
				font_width = &fpFonts[GRAPHICS].nWidths[0];
			else
				font_width = &fpFonts[STANDARD].nWidths[0];
		}
		else if( font_type == TEXT_FONT )
		{
			switch( arg )
			{
				case	EMPHASIS_STYLE:
				{
					if( fpFonts[EMPHASIS].nUsage == DISPLAY_FONT )
						font_width = &fpFonts[EMPHASIS].nWidths[0];
					else
						font_width = &fpFonts[STANDARD].nWidths[0];	
					break;
				}

				case	BOLDFACE_STYLE:
				{
					if( fpFonts[BOLD].nUsage == DISPLAY_FONT )
						font_width = &fpFonts[BOLD].nWidths[0];
					else
						font_width = &fpFonts[STANDARD].nWidths[0];	
					break;
				}

				case	EMPHASIS_STYLE | BOLDFACE_STYLE:
				{
					font_width = &fpFonts[FIXED_EMPHASIS].nWidths[0];
					break;
				}

				default:
				{
					font_width = &fpFonts[STANDARD].nWidths[0];	
					break;
				}
			}
		}
	} else width += font_width[c];

	//LOG_EXIT
    return width;
#endif
}/* os_string_width */

/*
 * os_set_cursor
 *
 * Place the text cursor at the given coordinates. Top left is (1,1).
 *
 */

void os_set_cursor (int y, int x)
{
	//LOG_ENTER
#ifdef _CONSOLE
	--y;
	--x;
#ifdef BUFFERED_TEXTOUT
	if( y != curCoord.Y && buffout_index )
		clear_output_buffer();
#endif
	curCoord.Y = max( y, 0 );
	curCoord.X = max( x, 0 );

	if( curCoord.Y > h_screen_height-1 )
		curCoord.Y = h_screen_height-1;

	if( curCoord.X > h_screen_width-1 )
		curCoord.X = h_screen_width-1;

	SetConsoleCursorPosition( hConsoleOutput, curCoord );
#else
	curCoord.Y = --y;
	curCoord.X = --x;
#endif
	//LOG_EXIT
}/* os_set_cursor */

void os_more_prompt (void)
{
    int saved_x;

    /* Save text font and style */

    int saved_font = current_font;
    int saved_style = current_style;

	//LOG_ENTER
    /* Choose plain text style */

	os_set_font( TEXT_FONT );
    os_set_text_style ( 0 );

    /* Wait until the user presses a key */

    saved_x = curCoord.X;

    os_display_string ((zchar *) "[MORE]");
    os_read_key (0, TRUE);

    os_erase_area (curCoord.Y + 1,
		   saved_x + 1,
		   curCoord.Y + current_font_height,
		   curCoord.X + 1);

    curCoord.X = saved_x;
#ifdef _CONSOLE
	SetConsoleCursorPosition( hConsoleOutput, curCoord );
#endif

    /* Restore text font and style */

    current_font = saved_font;
    current_style = saved_style;

    os_set_font( saved_font );
	os_set_text_style( saved_style );

	//LOG_EXIT
}/* os_more_prompt */

#if 0
 /*
 * os_more_prompt
 *
 * Display a MORE prompt, wait for a keypress and remove the MORE
 * prompt from the screen.
 *
 */

void os_more_prompt (void)
{
    int saved_font = current_font;
    int saved_style = current_style;

    int saved_x = curCoord.X;
    /* Choose plain text style */

	//LOG_ENTER
#ifndef _CONSOLE
	resetScrollPos();
#endif

    current_font = TEXT_FONT;
    current_style = 0;

    os_set_text_style( current_style );

    /* Wait until the user presses a key */

#ifdef _CONSOLE
    os_display_string ("[MORE]");
#else
	TEXTOUT( h_memDC, curCoord.X, curCoord.Y, "[MORE]", 6 );
	InvalidateRect( hWnd, NULL, FALSE );
#endif
    os_read_key (0, TRUE);

    /* Remove MORE prompt from the screen */

    os_erase_area (curCoord.Y + 1,
		   saved_x + 1,
		   curCoord.Y + current_font_height,
		   curCoord.X + os_string_width("[MORE]") + 1);

    /* Restore text font and style */

    current_font = saved_font;
    current_style = saved_style;

    os_set_text_style( current_style );

    /* Restore cursor position */

    curCoord.X = saved_x;
#ifdef _CONSOLE
	SetConsoleCursorPosition( hConsoleOutput, curCoord );
#endif
	//LOG_EXIT
}/* os_more_prompt */

#endif