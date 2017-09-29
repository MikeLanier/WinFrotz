/*
 * win_scrn.c
 *
 * Visual C interface, screen manipulation
 *
 */

#include <windows.h>
#include "../frotz/frotz.h"
#include "..\include\logfile_c.h"
#include <wingdi.h>
#include <crtdbg.h>
#include "../include/windefs.h"
#ifndef	_CONSOLE
#include "../include/crosscomp.h"
#endif

#ifdef _CONSOLE
COORD				curCoord;
extern	HANDLE				hConsoleOutput, hConsoleInput;
extern	COLORREF	text_fg;
#else

#ifdef BUFFERED_TEXTOUT
extern BOOL	bRefresh;
#endif

extern	HGDIOBJ		bkgBrush, startBrush;
extern	COLORREF	bkgBrushcolor;
extern	HDC		hdc;
extern	HDC		h_memDC;
extern	int		nUpdates;
extern	long	nScrollPos;
void	resetScrollPos( void );

#endif

extern	COLORREF text_bg;

extern	HWND	hWnd;

extern frotzwin wp[];

extern void clear_output_buffer( void );
/*
 * os_erase_area
 *
 * Fill a rectangular area of the screen with the current background
 * colour. Top left coordinates are (1,1). The cursor does not move.
 *
 */

void os_erase_area (int top, int left, int bottom, int right)
{
	char	*buffer = NULL;

#ifdef _CONSOLE
	DWORD	cWritten;
	COORD	tmpCoord;
	int i;

	//LOG_ENTER
	if( left==1 && right==h_screen_width )
	{
		tmpCoord.X = 0;
		tmpCoord.Y = top - 1;

		i = (right-left + 1) * (bottom - top + 1);
		FillConsoleOutputCharacter(
			hConsoleOutput,
			' ',
			i,
			tmpCoord,
			&cWritten);
		FillConsoleOutputAttribute(
			hConsoleOutput,		/* screen buffer handle     */ 
			(WORD)(text_bg),	/* color to fill with       */ 
			i,					/* number of cells to fill  */ 
			tmpCoord,			/* first cell to write to   */ 
			&cWritten);			/* actual number written to */ 
	}
	else
	{
		top--;
		left--;
		bottom--;
		right--;

		for( i=top; i <=bottom; i++ )
		{
			/* Fill a screen buffer area with the space character. */  
			tmpCoord.X = left;	/* start at first cell */ 
			tmpCoord.Y = i;	/*   of first row      */ 

			FillConsoleOutputCharacter( 
				hConsoleOutput,		/* screen buffer handle     */ 
				' ',				/* fill with spaces         */ 
				right-left+1,		/* number of cells to fill  */ 
				tmpCoord,			/* first cell to write to   */ 
				&cWritten			/* actual number written to */ 
				);
			
			/* Set screen buffer colors for area */ 
			
			FillConsoleOutputAttribute( 
				hConsoleOutput,		/* screen buffer handle     */ 
				(WORD)(text_bg),	/* color to fill with       */ 
				right-left+1,		/* number of cells to fill  */ 
				tmpCoord,			/* first cell to write to   */ 
				&cWritten			/* actual number written to */ 
				);
		}
	}
	SetConsoleCursorPosition( hConsoleOutput, curCoord );
	//LOG_EXIT
#else
	//LOG_ENTER
    top--;
    left--;
    bottom--;
    right--;

#ifdef BUFFERED_TEXTOUT
	bRefresh = TRUE;
#endif
	if( text_bg != bkgBrushcolor )
	{
		_ASSERT( startBrush );
		SelectObject( h_memDC, startBrush );
		_ASSERT( bkgBrush );
		DeleteObject( bkgBrush );
		
		bkgBrush = CreateSolidBrush( text_bg );
		_ASSERT( bkgBrush );
		bkgBrushcolor = text_bg;

		startBrush = SelectObject( h_memDC, bkgBrush );
		_ASSERT( startBrush );
	}

	if( !cwin )
	{
		PATBLT( h_memDC, left, top + nScrollPos, right-left+1, bottom-top+1, PATCOPY);
//		if( nUpdates )
//		{
			BITBLT( hdc, left, top, right-left, bottom-top, h_memDC, left, top + nScrollPos, SRCCOPY );
//		}
	}
	else
	{
		PATBLT( h_memDC, left, top, right-left+1, bottom-top+1, PATCOPY );
//		if( nUpdates )
//		{
			BITBLT( hdc, left, top, right-left, bottom-top, h_memDC, left, top, SRCCOPY );
//		}
	}


	//LOG_EXIT
#endif
}/* os_erase_area */

/*
 * os_scroll_area
 *
 * Scroll a rectangular area of the screen up (units > 0) or down
 * (units < 0) and fill the empty space with the current background
 * colour. Top left coordinates are (1,1). The cursor stays put.
 *
 */

void os_scroll_area (int top, int left, int bottom, int right, int units)
{
	//LOG_ENTER
#ifdef BUFFERED_TEXTOUT
	clear_output_buffer();
#endif

#ifdef _CONSOLE
	{
		SMALL_RECT	srctScrollRect, srctClipRect; 
		CHAR_INFO	chiFill; 
		COORD		coordDest;

		top--;
		left--;
		bottom--;
		right--;

		/* Set scrolling rectangle */

		srctScrollRect.Top = top; 
		srctScrollRect.Bottom = bottom; 
		srctScrollRect.Left = left; 
		srctScrollRect.Right = right; 

		coordDest.X = left; 
		coordDest.Y = top - units; 

		srctClipRect = srctScrollRect;

		chiFill.Attributes = (unsigned short)(text_fg | text_bg); 
		chiFill.Char.AsciiChar = ' '; 

		/* Scroll up one line. */ 

		ScrollConsoleScreenBuffer( 
			hConsoleOutput,  /* screen buffer handle     */ 
			&srctScrollRect, /* scrolling rectangle      */ 
			&srctClipRect,   /* clipping rectangle       */ 
			coordDest,       /* top left destination cell*/ 
			&chiFill);       /* fill character and color */ 
	}
#else
//	RECT	rcScroll;

//	rcScroll.top = --top;
//	rcScroll.left = --left;
//	rcScroll.bottom = --bottom;
//	rcScroll.right = --right;

	top--;
	left--;
	bottom--;

#ifdef BUFFERED_TEXTOUT
	bRefresh = TRUE;
#endif

	if( text_bg != bkgBrushcolor )
	{
		_ASSERT( startBrush );
		SelectObject( h_memDC, startBrush );
		_ASSERT( bkgBrush );
		DeleteObject( bkgBrush );
		
		bkgBrush = CreateSolidBrush( text_bg );
		_ASSERT( bkgBrush );
		bkgBrushcolor = text_bg;

		startBrush = SelectObject( h_memDC, bkgBrush );
		_ASSERT( startBrush );
	}

	/*	It wouldn't be good to use the "rolling window" scroll method
		indicated by nScrollPos with V6 games or any game that is scrolling
		only a portion of the screen.										*/
	if( (!cwin) & (right==h_screen_width) & (units>0) )
	{
		right--;
		if( nScrollPos + units>= h_screen_height )
		{
			resetScrollPos();
			nScrollPos += units;
		}
		else
		{
			nScrollPos += units;
			//PATBLT( h_memDC, left, bottom /*- units*/+ nScrollPos, right-left, units, PATCOPY);
		}
	}
	else
	{
		resetScrollPos();
		right--;
		BITBLT( h_memDC, left, top, right-left, bottom-units-top, h_memDC, 
			   left, top+units, SRCCOPY);
		if( units > 0 )
		{
			PATBLT( h_memDC, left, bottom-units, right-left, units, PATCOPY);
		}
		else
		{
			PATBLT( h_memDC, left, top-units, right-left, -units, PATCOPY);
		}
	}

//	ScrollDC(
//		h_memDC,	// handle of device context 
//		0,			// horizontal scroll units
//		-units,		// vertical scroll units
//		&rcScroll,	// address of structure for scrolling rectangle
//		&rcScroll,	// address of structure for clipping rectangle
//		NULL,		// handle of scrolling region
//		NULL	 	// address of structure for update rectangle
//	);

	if( nUpdates )
	{
		if( nUpdates > 1 )
		{
			RECT	rcScroll;
			rcScroll.left = left;
			rcScroll.right = right;
			rcScroll.top = top;
			rcScroll.bottom = bottom;
			ScrollWindowEx( hWnd, 0, -units, &rcScroll,	&rcScroll, NULL, NULL, 0 );
			BITBLT( hdc, left, bottom-units, right-left, bottom, h_memDC, left, bottom-units + nScrollPos, SRCCOPY );
		}
		else
		{
			InvalidateRect( hWnd, NULL, FALSE );
		}
	}
#endif
	//LOG_EXIT
}/* os_scroll_area */

#ifndef _CONSOLE
void resetScrollPos( void )
{
	//LOG_ENTER
	if( nScrollPos )
	{
		BITBLT( h_memDC, 0, wp[0].y_pos, h_screen_width, h_screen_height - wp[0].y_pos, h_memDC, 0, nScrollPos + wp[0].y_pos, SRCCOPY);
		PATBLT( h_memDC, 0, h_screen_height, h_screen_width, nScrollPos, PATCOPY);
		nScrollPos = 0;
	}
	//LOG_EXIT
}
#endif
