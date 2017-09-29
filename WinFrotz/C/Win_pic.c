/*
 * win_pict.c
 *
 * Visual C interface, picture functions (Windows)
 *
 */

#include <stdio.h>
#include <windows.h>
#include <wingdi.h>
#include "../frotz/frotz.h"
#include "..\include\logfile_c.h"
#include <crtdbg.h>
#ifndef	_CONSOLE
#include "../include/crosscomp.h"
#include "../include/windefs.h"
#endif


/****************************************************************************
 *	Common variables and constants to both versions							*
 ***************************************************************************/
#define PIC_NUMBER 0
#define PIC_WIDTH 2
#define PIC_HEIGHT 4
#define PIC_FLAGS 6
#define PIC_DATA 8
#define PIC_COLOUR 11

#define ALL_WHITE	224
#define ALL_BLACK	225

static struct {
    BYTE fileno;
    BYTE flags;
    WORD unused1;
    WORD images;
    WORD link;
    BYTE entry_size;
    BYTE padding;
    WORD checksum;
    WORD unused2;
    WORD version;
} gheader;

extern int display;
extern char data_path[];
extern char stripped_story_name[];
extern HWND	hWnd;

void close_graphics_file (void);
static FILE *graphics_fp = NULL;
static char extension[] = ".mg1";
static unsigned char *graphics_info = NULL;
int graphics_number = 0;
int	highest_color_count = 0;

/****************************************************************************
 *	Full Windows client variables and constants								*
 ***************************************************************************/

#ifndef _CONSOLE
extern HDC		h_memDC;
extern COLORREF	user_foreground, user_background, text_bg, text_fg, user_reverse_fg, user_reverse_bg, user_bold, user_emphasis;
extern 	LPBITMAPINFO	lpbmi;
extern COLORREF pc_colour_fg[];
extern fakecoord			curCoord;
extern int			nUpdates, current_fg, current_bg;

extern HDC			hdc;

HPALETTE	hmemPal = NULL;
HPALETTE	holdPal = NULL;

/****************************************************************************
 *	Console mode client variables and constants								*
 ***************************************************************************/
#else

extern	COORD	curCoord;
extern	HANDLE	hConsoleOutput;
#endif

/*
 * DestroyMemPalette
 *
 * Unselect and null out any created palettes for memory context.
 *
 */
#ifndef _CONSOLE
void DestroyMemPalette( void )
{
	//LOG_ENTER
	if( holdPal )
		SelectObject( h_memDC, holdPal );
	holdPal = NULL;
	if( hmemPal )
		DeleteObject( hmemPal );
	hmemPal = NULL;
	//LOG_EXIT
}
#endif
/*
 * open_graphics_file
 *
 * Open the graphics file. In EGA mode, the pictures may be stored in
 * two separate files.
 *
 */

int open_graphics_file (int number)
{
#ifdef _CONSOLE
	//LOG_ENTER
	//LOG_EXIT
	return 0;
#else
    char fname[MAX_FILE_NAME + 1];
	int i;

	//LOG_ENTER
    /* Free resources occupied by other graphics files */

    if( graphics_number )
		close_graphics_file();

    /* Build graphics file name */

	if( story_id == BEYOND_ZORK )
		strcpy( extension, ".mg1" );
	else
	{
		extension[1] = "cmem"[display - 2];
		extension[3] = '0' + number;
	}

    strcpy (fname, stripped_story_name);
	for( i = strlen( fname ); fname[i]!='.' && i > 0; i-- );
	if( fname[i]=='.' )
		fname[i] = 0;
    strcat (fname, extension);

    /* Open file, load header, allocate memory, load picture dictionary */

    if ((graphics_fp = fopen (fname, "rb")) == NULL) 
	{
		
		char	szFullName[ MAX_FILE_NAME + 30 ];
		
		strcpy( szFullName, data_path );
		strcat( szFullName, fname );

		if ((graphics_fp = fopen (szFullName, "rb")) == NULL) 
		{
			if( story_id == BEYOND_ZORK )
			{
	//LOG_EXIT
				return 0;
			}

			if( h_version == V6 )
			/* Warn the user about the problem */
			{
				char	szError[MAX_FILE_NAME + 40];
				sprintf( szError, "Cannot find graphics file (%s) for this display mode\n(The story may work anyway without graphics)", fname );
#ifdef _CONSOLE
				print_string(szError);
				os_read_key (0);
#else
				MessageBox( hWnd, szError, "WinFrotz Error", MB_OK );
#endif
			}
			/* Indicate failure */
	//LOG_EXIT
			return 0;
		}
    }

    /* Load header */

	PROBE
    fread (&gheader, sizeof (gheader), 1, graphics_fp);

    /* Load picture directory */

	if ((graphics_info = LocalAlloc( LPTR, gheader.images * gheader.entry_size)) != NULL) {

	PROBE
	fread (graphics_info, gheader.entry_size, gheader.images, graphics_fp);

	graphics_number = number;

    } else 
	{
		fclose (graphics_fp);
		graphics_fp = NULL;
	}

	//LOG_EXIT
    return graphics_number;
#endif
}/* open_graphics_file */

/*
 * close_graphics_file
 *
 * Free resources allocated for the current graphics file.
 *
 */

void close_graphics_file (void)
{
	//LOG_ENTER
	if( graphics_number == 0 )
	{
	//LOG_EXIT
		return;
	}

	highest_color_count = 0;
#ifndef _CONSOLE
	DestroyMemPalette();
#endif
	if( graphics_fp )
	{
		fclose (graphics_fp);
		graphics_fp = NULL;
	}
	if( graphics_info )
	{
		LocalFree (graphics_info);
		graphics_info = NULL;
	}

    graphics_number = 0;
	//LOG_EXIT
}/* close_graphics_file */

/*
 * picture_data
 *
 * Return information on the given picture.
 *
 */

/* The extra boolean on the end here shows whether we want to lie about the picture geometry.
   If it is TRUE WinFrotz will tell the dimensions that the picture WOULD be given the current
   display size (the scaled size, in other words). If FALSE WinFrotz will return the actual,
   original dimensions of the data
*/

static int picture_data (int picture, int *height, int *width, long *data, long *colour, unsigned *flags, BOOL retScale )
{
#ifdef _CONSOLE
	//LOG_ENTER
	//LOG_EXIT
	return 0;
#else
    unsigned char *info;
    long val;
    int old;
    int i, scaleY;
	double scaleX;

	//LOG_ENTER
	if( display==MCGA_MODE )
	{
		scaleX = min( (double)h_screen_width / 320, (double)h_screen_height / 200 );
		scaleY = (int)scaleX;
	}
	else if( display == CGA_MODE )
	{
		scaleX = (double)h_screen_width / 640;
		scaleY = h_screen_height / 200;
	}
	else
	{
		scaleX = (double)h_screen_width / 640;
		scaleY = h_screen_height / 200;
	}

   /* Scan the picture directory. The pictures may be scattered among
       several files (in EGA mode only), so try all the graphics files
       in turn. At the moment, Frotz assumes that the graphics header
       entry "link" is set to a non-zero value when there is another
       graphics file. This is actually just a guess that works for the
       existing graphics files... */

    old = graphics_number;

    do {

	if (graphics_number == 0)
	    break;

	info = graphics_info;

	for (i = 0; i < gheader.images; i++) {

	    if (picture == * (unsigned short *) (info + PIC_NUMBER))
		goto picture_found;

	    info += gheader.entry_size;
	}

	graphics_number++;

	if (gheader.link == 0)
	    graphics_number = 1;

	open_graphics_file (graphics_number);

    } while (graphics_number != old);

	//LOG_EXIT
    return 0;

    /* Read all information from directory entry */

picture_found:

    *height = * (unsigned short *) (info + PIC_HEIGHT);
    *width = * (unsigned short *) (info + PIC_WIDTH);
	if( retScale )
	{
		*height = *height * scaleY;
		i = (int)(*width * scaleX);
		*width = i;
	}
    *flags = * (unsigned short *) (info + PIC_FLAGS);

    byte0 (val) = info[PIC_DATA + 2];
    byte1 (val) = info[PIC_DATA + 1];
    byte2 (val) = info[PIC_DATA];
    byte3 (val) = 0;

    *data = val;

    if (gheader.entry_size >= 14) {

	byte0 (val) = info[PIC_COLOUR + 2];
	byte1 (val) = info[PIC_COLOUR + 1];
	byte2 (val) = info[PIC_COLOUR];
	byte3 (val) = 0;

    } else val = 0;

    *colour = val;

    /* Indicate success */

	//LOG_EXIT
    return 1;
#endif
}/* picture_data */

/*
 * os_picture_data
 *
 * Return true if the given picture is available. If so, store the
 * picture width and height in the appropriate variables. Picture
 * number 0 is a special case: Write the number of pictures available
 * and the picture file release number into the height and width
 * variables respectively when this picture number is asked for.
 *
 */

int os_picture_data (int picture, int *height, int *width)
{
#ifdef _CONSOLE
	//LOG_ENTER
	//LOG_EXIT
	return 0;
#else
    long data;
    long colour;
    unsigned flags;

	//LOG_ENTER
    if (picture == 0) {
	
	*height = gheader.images;
	*width = gheader.version;

	//LOG_EXIT
	return 1;
    }

	//LOG_EXIT
    return picture_data (picture, height, width, &data, &colour, &flags, TRUE);
#endif
}/* os_picture_data */

/*
 * get_scrnptr
 *
 * Return a pointer to the given line in video RAM.
 *
 */

static unsigned char *get_scrnptr(int y)
{
	//LOG_ENTER
	//LOG_EXIT
	return 0;
}/* get_scrnptr */


BOOL MakeMemPalette( void )
{
#ifdef _CONSOLE
	//LOG_ENTER
	//LOG_EXIT
	return TRUE;
#else
	LPLOGPALETTE lpPal;      /* pointer to a logical palette */
	HANDLE hLogPal;          /* handle to a logical palette	*/
	HPALETTE hPal = NULL;    /* handle to a palette	*/
	int i;                   /* loop index	*/
	WORD wNumColors;         /* number of colors in color table	*/
	BOOL bResult = FALSE;

	//LOG_ENTER
	if ( display < MCGA_MODE || !( GetDeviceCaps( h_memDC, RASTERCAPS ) & RC_PALETTE) )
	{
	//LOG_EXIT
		return FALSE;
	}

	if( display==EGA_MODE && hmemPal )
	{
	//LOG_EXIT
		return TRUE;
	}
	else
		highest_color_count = 16;

	/*TODO: Why does this overhead number need to be so high?*/
	wNumColors = highest_color_count + 35;

	if( 255 - highest_color_count - 21 < 0 )
	{
		MessageBox( hWnd, "WARNING: Too many colors encountered in this graphic for 8-bit palette", "WinFrotz", MB_ICONEXCLAMATION );
		highest_color_count = 255 - 21;
	}

	/* allocate memory block for logical palette */
	hLogPal = GlobalAlloc(GHND, sizeof(LOGPALETTE)
								+ sizeof(PALETTEENTRY)
								* wNumColors);

	/* if not enough memory, clean up and return NULL */
	if ( !hLogPal )
	{
	//LOG_EXIT
		return FALSE;
	}

	lpPal = (LPLOGPALETTE) GlobalLock((HGLOBAL) hLogPal);

	/* set version and number of palette entries */
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = (WORD)wNumColors;

	lpPal->palPalEntry[0].peRed = GetRValue( user_foreground );
	lpPal->palPalEntry[0].peGreen = GetGValue( user_foreground );
	lpPal->palPalEntry[0].peBlue = GetBValue( user_foreground );

	lpPal->palPalEntry[1].peRed = GetRValue( user_background );
	lpPal->palPalEntry[1].peGreen = GetGValue( user_background );
	lpPal->palPalEntry[1].peBlue = GetBValue( user_background );

	lpPal->palPalEntry[2].peRed = GetRValue( user_reverse_fg );
	lpPal->palPalEntry[2].peGreen = GetGValue( user_reverse_fg );
	lpPal->palPalEntry[2].peBlue = GetBValue( user_reverse_fg );

	lpPal->palPalEntry[3].peRed = GetRValue( user_reverse_bg );
	lpPal->palPalEntry[3].peGreen = GetGValue( user_reverse_bg );
	lpPal->palPalEntry[3].peBlue = GetBValue( user_reverse_bg );

	lpPal->palPalEntry[4].peRed = GetRValue( user_bold );
	lpPal->palPalEntry[4].peGreen = GetGValue( user_bold );
	lpPal->palPalEntry[4].peBlue = GetBValue( user_bold );

	lpPal->palPalEntry[5].peRed = GetRValue( user_emphasis );
	lpPal->palPalEntry[5].peGreen = GetGValue( user_emphasis );
	lpPal->palPalEntry[5].peBlue = GetBValue( user_emphasis );

	for( i = 6; i < 17; i++ )
	{
		lpPal->palPalEntry[i].peRed = GetRValue( pc_colour_fg[ i-3 ] );
		lpPal->palPalEntry[i].peGreen = GetGValue( pc_colour_fg[ i-3 ] );
		lpPal->palPalEntry[i].peBlue = GetBValue( pc_colour_fg[ i-3 ] );
	}

	if( h_version == 6 )
	{
		for (i = 17; i < (int)wNumColors; i++)
		{
			lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i-17].rgbRed;
			lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i-17].rgbGreen;
			lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i-17].rgbBlue;
			lpPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
			/*lpPal->palPalEntry[i].peFlags = 0;*/
		}
	}

	/* create the palette and get handle to it */

	if( !hmemPal )
	{
		hmemPal = CreatePalette( lpPal );
	}
	else
	{
		SetPaletteEntries( hmemPal, 0, wNumColors, lpPal->palPalEntry );
		/*AnimatePalette( hmemPal, 0, wNumColors, lpPal->palPalEntry );*/
	}

	if ( holdPal )
		SelectPalette( h_memDC, holdPal, TRUE);
	holdPal = NULL;

	if ( hmemPal )
	{
		holdPal = SelectPalette( h_memDC, hmemPal, TRUE );
		RealizePalette( h_memDC );
	}

	GlobalUnlock((HGLOBAL) hLogPal);
	GlobalFree((HGLOBAL) hLogPal);

	//LOG_EXIT
	return bResult;
#endif
}

/*
 * os_draw_picture
 *
 * Display a picture at the given coordinates. Top left is (1,1).
 *
 */

void os_draw_picture (int picture, int y, int x)
{
#ifdef _CONSOLE 
	COORD	temp = curCoord;
	//LOG_ENTER
	print_string( "[Graphics not supported in this version]" );
	curCoord = temp;
	SetConsoleCursorPosition( hConsoleOutput, curCoord );
	//LOG_EXIT
	return;
#else
    unsigned char buf[512];
    unsigned char *table_val;
    unsigned short *table_seq;
    long data;
    long colour;
    unsigned int flags;
    unsigned int code;
    unsigned int prev_code = 0;
    unsigned int val;
    unsigned int transparent;
    int height, width;
    int count;
    int bits;
    int bits_per_code;
    int bits_left;
    int pos, scanwidth, pixy, cmp;
    int i, saved_display_mode;
	int	scaleY;
	double scaleX;
	BYTE			*bmcontents = NULL, *bmmask = NULL;
	BYTE			*bmoffset = NULL;
	BOOL			has_transparency = FALSE;

	//LOG_ENTER
	PostMessage( hWnd, WM_CURSOR_HIDE, WM_CURSOR_HIDE, (LPARAM)0L );

	if( story_id == BEYOND_ZORK )
	{
		saved_display_mode = display;
		display = MCGA_MODE;
	}

    y--;
    x--;

	lpbmi->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	lpbmi->bmiHeader.biPlanes = 1;
	lpbmi->bmiHeader.biBitCount = 8;
	lpbmi->bmiHeader.biCompression = BI_RGB;
	lpbmi->bmiHeader.biSizeImage = 0;
	lpbmi->bmiHeader.biXPelsPerMeter = 0;
	lpbmi->bmiHeader.biYPelsPerMeter = 0;
	lpbmi->bmiHeader.biClrUsed = 0;
	lpbmi->bmiHeader.biClrImportant = 0;

	pixy = y;
    /* Get all information on the current picture. */

    if (picture_data (picture, &height, &width, &data, &colour, &flags, FALSE) == 0)
	{
	//LOG_EXIT
	return;
	}

	if( display==CGA_MODE )
	{
		scaleX = (double)h_screen_width / 640;
		scaleY = h_screen_height / 200;
	}
	else if( display ==MCGA_MODE )
	{
		scaleX = min( (double)h_screen_width / 320, (double)h_screen_height / 200 );
		scaleY = (int)scaleX;
	}
	else
	{
		scaleX = (double)h_screen_width / 640;
		scaleY = h_screen_height / 200;
	}

	/* DWORD align DIB scan lines */

	if( width % sizeof( DWORD ) )
		scanwidth = ( width / sizeof( DWORD ) + 1 ) * sizeof( DWORD );
	else
		scanwidth = width;

	lpbmi->bmiHeader.biWidth = width;
	lpbmi->bmiHeader.biHeight = -height;

    /* Allocate memory for decompression. */

	table_seq = LocalAlloc( LPTR, 3840 * sizeof (unsigned) );
	table_val = LocalAlloc( LPTR, 3840 * sizeof (unsigned char) );

    if (table_val == NULL && table_seq != NULL)
	free (table_seq);
    if (table_seq == NULL && table_val != NULL)
	free (table_val);

    if (table_val == NULL || table_seq == NULL)
	{
	//LOG_EXIT
	return;
	}

    /* When the given picture provides a colour map then activate it.
       This is used only in MCGA mode; note that the same colour map
       is used for all pictures on the screen. The first colour to be
       defined is colour 2; colours 0 and 1 are not used. Each colour
       map may define up to 14 colours (i.e. colour 2 to 15). The
       colour map that is used for pictures in MCGA mode does not
       affect the standard Z-machine colours which are used for text.
       (This is based on Amiga interpreters which had to work with 16
       colours. Colour 0 and 1 were used for text, and changing the
       text colours actually changed the palette entries 0 and 1.) */

	/* CGA pics tend to run over the end of the array, so pad by scanline */

	bmcontents = LocalAlloc( LPTR, scanwidth * height + scanwidth );
	bmmask = LocalAlloc( LPTR, scanwidth * height + scanwidth );
	if( !bmcontents || !bmmask)
		os_fatal( "Could not allocate bitmap memory" );

	bmoffset = bmcontents;

    if (colour != 0) 
	{
		fseek (graphics_fp, colour, SEEK_SET);

		count = fgetc (graphics_fp);

		for( i=2; i <= count + 2; i++ )
		{
			lpbmi->bmiColors[i].rgbRed = fgetc( graphics_fp );
			lpbmi->bmiColors[i].rgbGreen = fgetc( graphics_fp );
			lpbmi->bmiColors[i].rgbBlue = fgetc( graphics_fp );
		}
	
		if( count + 2 > highest_color_count )
			highest_color_count = count + 2;
    }

	lpbmi->bmiColors[ALL_WHITE].rgbRed = 255;
	lpbmi->bmiColors[ALL_WHITE].rgbGreen = 255;
	lpbmi->bmiColors[ALL_WHITE].rgbBlue = 255;

	lpbmi->bmiColors[ALL_BLACK].rgbRed = 0;
	lpbmi->bmiColors[ALL_BLACK].rgbGreen = 0;
	lpbmi->bmiColors[ALL_BLACK].rgbBlue = 0;

	MakeMemPalette();

	/* Place the file pointer at the start of the picture data. */

    fseek (graphics_fp, data, SEEK_SET);

    /* Bit 0 of the flags variable indicates that the picture wants to
       use a transparent colour; the top four bits hold the index of
       the colour which is supposed to be transparent. In CGA and MCGA
       modes this will always be 0; in EGA mode this may be any colour
       between 0 and 15. */

    transparent = 0xffff;

    if (flags & 1)
	transparent = flags >> 12;

    /* The compressed picture is a stream of bits. We read the file
       byte-wise, storing the current byte in a variable called "bits".
       The number of bits which have not already been used is stored in
       "bits_left". At the start of decompression, 9 bits make one code.
       During the process this can rise up to 12 bits per code. */

    bits_per_code = 9;
    bits_left = 0;

    /* The uncompressed picture is just a long sequence of bytes. Each
       byte holds the colour of a pixel; therefore all resulting bytes
       will be in the range from 0 to 15. One line of pixels follows
       another, starting at the top left, ending at the bottom right.
       There is one exception to this: In CGA mode only, when bit 0 in
       the flags variable is clear, each byte in the uncompressed data
       holds the pattern for the next 8 pixels (high bits first). Our
       position in the current line is stored in "pos". It runs from 0
       to "width". */

	pos = 0;

    /* Prepare EGA board. */

    /* Loop until a special code signals the end of the picture. */

    for (;;) 
	{
		code = 0;

		/* Read the next code from the graphics file. */

		i = bits_per_code;

		do {

			if (bits_left <= 0) {
			bits = fgetc (graphics_fp);
			bits_left = 8;
			}

			code |= (bits >> (8 - bits_left)) << (bits_per_code - i);

			i -= bits_left;
			bits_left = -i;

		} while (i > 0);

		code &= 0xfff >> (12 - bits_per_code);

		/* Code 256 resets the number of bits per code to 9 and clears
		   the table (see below). The first code will always be 256,
		   allowing us to lazily omit the initialisation of "count" at
		   the start of decompression. */

		if (code == 256) {

			bits_per_code = 9;
			count = 257;

			continue;
		}

		/* The second special code, 257, marks the end of the picture. */

		if (code == 257)
			break;

		/* Codes from 0 to 255 are literals, i.e. they represent the
		   byte of the same value. All other codes refer to sequences
		   of bytes that are stored in a table. A special case is the
		   sequence that is next to be defined. When this sequence is
		   chosen, we must repeat the previous sequence and prefix it
		   with its last byte. */

		val = code;
		i = 0;

		if (code == (unsigned int)count) {
			val = prev_code;
			i = 1;
		}

		while (val > 255) {
			buf[i++] = table_val[val - 256];
			val = table_seq[val - 256];
		}

		buf[i] = (unsigned char)val;

		if (code == (unsigned int)count)
			buf[0] = (unsigned char)val;

		/* For each code, add a new sequence to the table: identical
		   to the previous sequence, but prefixed with the last byte
		   of the current one. After that, increment the number of
		   bits per code if the index of the next sequence to define
		   takes too many bits. The maximum number of bits is 12, so
		   the size of the table has to be (2 ^ 12 - 256). */

		table_val[count - 256] = (unsigned char)val;
		table_seq[count - 256] = prev_code;

		if (++count == (1 << bits_per_code) && bits_per_code < 12)
			bits_per_code++;

		/* The current sequence has to be read backwards; so when we
		   were talking about "prefixing" we actually meant appending,
		   and when we were talking about the "last" byte we actually
		   meant the first byte. Experiments show that the entire
		   sequence is never longer than 512 bytes. Therefore a buffer
		   of 512 bytes suffices to reverse the order of the sequence. */

		do 
		{
			if( pos >= width )
			{
				pos = 0;
				bmoffset += scanwidth;
				pixy+=scaleY;
			}

			/* Fetch the colour for the next pixel. */
			val = buf[i--];

			/* Display the next pixel */
			if (display != CGA_MODE) 
			{
				bmoffset[pos++] = (unsigned char)val;
			}
			else
			{
				if (flags & 1) 
				{
					if( val==2 )
						bmoffset[ pos++ ] = ALL_WHITE;
					else
						bmoffset[ pos++ ] = ALL_BLACK;
				}
				else
				{
					cmp = 0x80;
					while( cmp )
					{
						if( val & cmp )
							bmoffset[ pos++ ] = ALL_WHITE;
						else
							bmoffset[ pos++ ] = ALL_BLACK;
						cmp = cmp >> 1;
					}
				}
			} 
			/* If it's transparent, go back and deal with it. This is actually a valid
			   optimization; transparency is an infrequent occurence and enclosing the
			   above in an if() else() would involve a jump for instruction for every
			   pixel... */
			if( val == transparent )
			{
				--pos;
				has_transparency = TRUE;
				bmmask[ bmoffset - bmcontents + pos ] = ALL_WHITE;
				bmoffset[pos++] = ALL_BLACK;
			}

		} while (i >= 0);

		/* Remember the current code; this will be needed to build the
		   next sequence in the table. */
		prev_code = code;
    }

	pos = 0;


	if( has_transparency )
	{
		if( StretchDIBits( h_memDC, 
						   x, y, (int)(width * scaleX), height * scaleY, 
						   0, 0, width, height,
						   &bmmask[0], lpbmi, DIB_RGB_COLORS, SRCAND) ==GDI_ERROR )
			pos = 1;

		if( StretchDIBits( h_memDC, 
						   x, y, (int)(width * scaleX), height * scaleY, 
						   0, 0, width, height, 
						   &bmcontents[0], lpbmi, DIB_RGB_COLORS, SRCPAINT) ==GDI_ERROR )
			pos = 1;
	}
	else
	{
		if( story_id != BEYOND_ZORK )
		{
			if( StretchDIBits( h_memDC, 
							   x, y, (int)(width * scaleX), height * scaleY, 
							   0, 0, width, height, 
							   &bmcontents[0], lpbmi, DIB_RGB_COLORS, SRCCOPY) ==GDI_ERROR )
			pos = 1;
		}
		else
		{
			if( StretchDIBits( h_memDC, 
							   x, y, h_screen_width, h_screen_height, 
							   0, 0, width, height, 
							   &bmcontents[0], lpbmi, DIB_RGB_COLORS, SRCCOPY) ==GDI_ERROR )
			pos = 1;
		}
	}

	if( nUpdates > 1 )
	{
		BITBLT( hdc, x, y, (int)(width * scaleX), height * scaleY, h_memDC, x, y, SRCCOPY );
	}

    /* Free memory. */
	LocalFree( bmcontents );
	LocalFree( bmmask );
	LocalFree( table_seq );
	LocalFree( table_val );

	if( pos )
		os_fatal("Error in copying bitmap" );

	if( story_id == BEYOND_ZORK )
		display = saved_display_mode;

	//LOG_EXIT
#endif
}/* os_draw_picture */


/*
 * os_peek_colour
 *
 * Return the colour of the screen unit below the cursor. (If the
 * interface uses a text mode, it may return the background colour
 * of the character at the cursor position instead.) This is used
 * when text is printed on top of pictures. Note that this coulor
 * need not be in the standard set of Z-machine colours. To handle
 * this situation, Frotz extends the colour scheme: Colours above
 * 15 (and below 256) may be used by the interface to refer to non
 * standard colours. Of course, os_set_colour must be able to deal
 * with these colours.
 *
 */

int os_peek_colour (void)
{
#ifdef _CONSOLE
	//LOG_ENTER
	//LOG_EXIT
	return 0;
#else
	//LOG_ENTER
	pc_colour_fg[17] = GetPixel( h_memDC, curCoord.X, curCoord.Y );
	//LOG_EXIT
	return( 17 );
#endif
}/* os_peek_colour */

