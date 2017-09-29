typedef unsigned char zbyte;
typedef unsigned short zword;
typedef unsigned char zchar;

/* NOTE: From here to the comment line START WINDOWS DEFS are repeats of constants
   that are defined in frotz.h. It's poor form to do it this way but it's worse to
   include a bunch of DOS defines that belong nowhere near a Windows program. */
/*** Character codes ***/

#define ZC_TIME_OUT 0x00
#define ZC_NEW_STYLE 0x01
#define ZC_NEW_FONT 0x02
#define ZC_BACKSPACE 0x08
#define ZC_INDENT 0x09
#define ZC_GAP 0x0b
#define ZC_RETURN 0x0d
#define ZC_HKEY_MIN 0x0e
#define ZC_HKEY_RECORD 0x0e
#define ZC_HKEY_PLAYBACK 0x0f
#define ZC_HKEY_SEED 0x10
#define ZC_HKEY_UNDO 0x11
#define ZC_HKEY_RESTART 0x12
#define ZC_HKEY_QUIT 0x13
#define ZC_HKEY_DEBUG 0x14
#define ZC_HKEY_HELP 0x15
#if defined (WINFROTZ) || defined(_CONSOLE)
#define ZC_HKEY_ALIAS 0x16
#define ZC_HKEY_MAX 0x16
#else
#define ZC_HKEY_MAX 0x15
#endif
#define ZC_ESCAPE 0x1b
#define ZC_ASCII_MIN 0x20
#define ZC_ASCII_MAX 0x7e
#define ZC_BAD 0x7f
#define ZC_ARROW_MIN 0x81
#define ZC_ARROW_UP 0x81
#define ZC_ARROW_DOWN 0x82
#define ZC_ARROW_LEFT 0x83
#define ZC_ARROW_RIGHT 0x84
#define ZC_ARROW_MAX 0x84
#define ZC_FKEY_MIN 0x85
#define ZC_FKEY_MAX 0x90
#define ZC_NUMPAD_MIN 0x91
#define ZC_NUMPAD_MAX 0x9a
#define ZC_SINGLE_CLICK 0x9b
#define ZC_DOUBLE_CLICK 0x9c
#define ZC_MENU_CLICK 0x9d
#define ZC_LATIN1_MIN 0xa0
#define ZC_LATIN1_MAX 0xff

/*** File types ***/

#define FILE_RESTORE 0
#define FILE_SAVE 1
#define FILE_SCRIPT 2
#define FILE_PLAYBACK 3
#define FILE_RECORD 4
#define FILE_LOAD_AUX 5
#define FILE_SAVE_AUX 6
#define FILE_LOAD_INIT 7
#define FILE_SAVE_INIT 8

typedef struct tagfrotzwin {
    zword y_pos;
    zword x_pos;
    zword y_size;
    zword x_size;
    zword y_cursor;
    zword x_cursor;
    zword left;
    zword right;
    zword nl_routine;
    zword nl_countdown;
    zword style;
    zword colour;
    zword font;
    zword font_size;
    zword attribute;
    zword line_count;
} frotzwin;

typedef struct tagfakecoord {
	int Y;
	int X;
} fakecoord;

#define byte0(v)	((unsigned char *)&v)[0]
#define byte1(v)	((unsigned char *)&v)[1]
#define byte2(v)	((unsigned char *)&v)[2]
#define byte3(v)	((unsigned char *)&v)[3]
#define word0(v)	((unsigned *)&v)[0]
#define word1(v)	((unsigned *)&v)[1]

#ifndef HISTORY_BUFFER_SIZE
#define HISTORY_BUFFER_SIZE 500
#endif
#ifndef HISTORY_MIN_ENTRY
#define HISTORY_MIN_ENTRY 4
#endif
/* START WINDOWS DEFS */

#define SPECIAL_KEY_MIN 256

#define SPECIAL_KEY_HOME 256
#define SPECIAL_KEY_END 257
#define SPECIAL_KEY_WORD_LEFT 258
#define SPECIAL_KEY_WORD_RIGHT 259
#define SPECIAL_KEY_DELETE 260
#define SPECIAL_KEY_INSERT 261
#define SPECIAL_KEY_PAGE_UP 262
#define SPECIAL_KEY_PAGE_DOWN 263
#define SPECIAL_KEY_DELETE_WORD 264
#define SPECIAL_KEY_TRANSPOSE 265
#define SPECIAL_KEY_DELETE_LEFT 266
#define SPECIAL_KEY_TAB	267
#define SPECIAL_KEY_MAX 268

#define UP_ARROW	129
#define LEFT_ARROW	131
#define RIGHT_ARROW 132
#define DOWN_ARROW	130

typedef	struct	tagFONTPROPS {
	int		nUsage;
	int		nUsagePerm;
	int		nWidths[255];
	int		nHeight;
	HFONT	hfHandle;
	LOGFONT	lf;
} fontprops;

/* Note: STANDARD must remain 0 */
#define	STANDARD		0
#define	EMPHASIS		1
#define BOLD			2
#define EMPH_BOLD		3
#define	UNDERLINE		4
#define GRAPHICS		5
#define	FIXED			6
#define FIXED_EMPHASIS	7
#define FIXED_BOLD		8
#define FIXED_EMPH_BOLD 9
#define	FIXED_UNDERLINE	10
#define ZORK_FONT		11
#define	NUM_FONT_TYPES	12

#define DISPLAY_NO_EFFECT	0
#define DISPLAY_FONT		1
#define DISPLAY_COLOR		2
#define DISPLAY_FONT_UNDERLINE 4

#define CUSTOM_FILTER_SIZE 128	/* Size of user-defined filter list for file dialog */

#define UPDATE_ABOUT_BOX	35	/* Timer ID for about box text updates */
#define UPDATE_ABOUT_ICON	30	/* Timer ID for about box icon */

#ifdef	_CONSOLE
#define CURRENT_FONT_WIDTH( X ) 1
#else
#define	CURRENT_FONT_WIDTH( X ) current_font_width[ X ]
#endif

#ifdef	_DEBUG

#define TEXTOUT( hDC, X, Y, str, len ) { _ASSERT( TextOut( hDC, X, Y,  str, len ) ); }

#define RELEASEDC( hWnd, hDC ) {\
	_ASSERT( hWnd );\
	_ASSERT( hDC );\
	_ASSERT( ReleaseDC( hWnd, hDC ) );\
}

#define BITBLT( destDC, destX, destY, width, height, srcDC, srcX, srcY , Flags ) {\
	_ASSERT( BitBlt( destDC, destX, destY, width, height, srcDC, srcX, srcY , Flags ) );\
}

#define PATBLT( destDC, destX, destY, width, height, flags) {\
	_ASSERT( PatBlt( destDC, destX, destY, width, height, flags) );\
}

#define DEBUGBOX( hWnd, string1, string2, flags ) {\
	MessageBox( hWnd, string1, string2, flags );\
} 
#else

//#define TEXTOUT( hDC, X, Y, str, len ) {TextOut( hDC, X, Y, str, len ); }
#define TEXTOUT( hdc, X, Y, str, len ) { ExtTextOut( hdc, X, Y, 0, NULL, str, len, NULL);  }

#define RELEASEDC( hWnd, hDC ) { ReleaseDC( hWnd, hDC ); }

#define BITBLT( destDC, destX, destY, width, height, srcDC, srcX, srcY , Flags ) {\
	    BitBlt( destDC, destX, destY, width, height, srcDC, srcX, srcY , Flags ); }

#define PATBLT( destDC, destX, destY, width, height, flags) {\
		PatBlt( destDC, destX, destY, width, height, flags); }

#define DEBUGBOX( hWnd, string1, string2, flags ) {};
#endif

#define MONO_MODE 0
#define TEXT_MODE 1
#define CGA_MODE 2
#define MCGA_MODE 3
#define EGA_MODE 4

#ifdef _CONSOLE
#define V6_WARNING "\
You are attempting to load a V6 (graphical) Infocom game.\n\
WinFrotz will do it's best to display the text of this\n\
adventure, but can't display graphics while used as a\n\
console app. To view graphics in a V6 game use the windowed\n\
version of WinFrotz (a seperate executable)"

#else
#define	READ_REG_FAIL	32767
#define	ITEM_COLORMODE	"ColorMode"
#define ITEM_UPDATES	"Freq_Updates"
#define	ITEM_XSIZE		"X_Size"
#define ITEM_YSIZE		"Y_Size"
#define	ITEM_RESIZE		"Resize"

#define SECTION_COLORS	"Colors"
#define ITEM_FOREGROUND	"Foreground"
#define ITEM_BACKGROUND	"Background"
#define ITEM_INV_FOREGROUND "Inverse_Foreground"
#define ITEM_INV_BACKGROUND "Inverse_Background"
#define ITEM_EMPHASIS	"Emphasis"
#define	ITEM_BOLD		"Bold"
#define	ITEM_CUSTCOLORS	"CustColors"
#define	ITEM_XPOS		"StartX"
#define ITEM_YPOS		"StartY"

#define	ITEM_EMPHASIS_FONT_USAGE "EmphMode"
#define	ITEM_BOLD_FONT_USAGE	"BoldMode"

#define ITEM_DISPLAY_FONT		"VarFont"
#define ITEM_FIXED_FONT			"FixFont"
#define ITEM_GRAPHICS_FONT		"GraphFont"

#define ITEM_GRAPHICS_SCREEN_SIZE	"GraphScreenSize"
#define ITEM_GRAPHICS_MODE			"GraphMode"

#define	ITEM_SOUND_PLAYED	"IsSoundPlayed"
#define ITEM_SOUND_VOLUME	"SoundVolume"

#define ITEM_INITIALFILE	"DefaultStory"

#define ITEM_MISC_FLAGS		"MiscFlags"
#define STATUS_BAR_DISABLED	0x01
#define TOOL_BAR_DISABLED	0x02

#define FIXED_FONT_WARNING "\
Changing the fixed font at this time may cause\n\
your display to become unreadable. WinFrotz will\n\
not be able to correct for this if it occurs.\n\
\nAre you sure you wish to continue?"

#define FIXED_FONT_WARNING2 "\
Version 4 and 5 games, especially those from\n\
Infocom, don't deal well with changing the\n\
fixed font while a game is loaded. The status\n\
area may become unreadable.\n\n\
Are you sure you wish to continue?"

#define COLOR_WARNING "\
Using full color mode (required for multi color fonts)\n\
will slow down the display, but does allow for anti\n\
aliased fonts (with Plus! or NT 4.0) and unique colors.\n\n\
Are you sure you want to do this?"

#define COLOR_WARNING_V6 "\
Sorry, you cannot switch to color mode when a graphical\n\
game has already been initialized as CGA. Use the graphics\n\
option dialog box to select MCGA as your V6 mode, then\n\
restart the game."

#define MONO_WARNING "\
WinFrotz cannot switch a running game from multi color\n\
to 2-color mode. To switch back to 2-color, save the\n\
current game, quit it, then select 2-color and reload."

#define FIXED_FONT_TOO_LARGE "\
You cannot select this font size for the status bar\n\
because it would make the WinFrotz screen larger than\n\
the Windows desktop. WinFrotz can handle this but the\n\
Microsoft Toolbar cannot. Please select a smaller font."

#define FILE_WARNING "\
WinFrotz can only perform game file operations when at\n\
a text input prompt."

#define GRAPH_FONT_SIZE_WARN "\
It's not recommend that either large or small font sizes\n\
are choosen for graphics mode. The Z-machine code itself\n\
often does not deal with these sizes well. Preferably you\n\
should use a size around 8 pts.\n\n\
Do you want to continue with this font size?"

#define	VER_6_RESIZE "\
You cannot resize (or turn off locked mode) when running a\n\
Version 6 (graphical) game. These games depend heavily on a\n\
fixed screen size that never changes. After exiting a graphical\n\
game WinFrotz will reset your resize option to whatever it was\n\
before the Version 6 game was loaded, and you can at that point\n\
turn it on or off."
#endif