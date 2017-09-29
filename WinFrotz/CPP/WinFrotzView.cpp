// WinFrotzView.cpp : implementation of the CWinFrotzView class
//

#include "../include/stdafx.h"
#include "../include/WinFrotz.h"
#include "../include/mainfrm.h"
#include "../include/WinFrotzDoc.h"
#include "../include/WinFrotzView.h"
#include "../include/CMyColorDialog.h"
#include "../include/CMyFontDialog.h"
#include "../include/globs.h"
#include "../include/states.h"
#include "../include/windefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C"	zword	h_screen_width;
extern "C"	zword	h_screen_height;
extern "C"	fakecoord	curCoord;
extern "C"	DWORD	cursorHeight;
extern "C"	int		nState;
extern "C"	BOOL	bStoryInit;
extern "C"	BOOL	bStoryLoaded;
extern "C"	fontprops	fpFonts[];
extern "C"	COLORREF	text_bg;
extern "C"	COLORREF	text_fg;
extern "C"	COLORREF	user_reverse_fg;
extern "C"	COLORREF	user_reverse_bg;
extern "C"	COLORREF	user_emphasis;
extern "C"	COLORREF	user_bold;
extern "C"  COLORREF	pc_colour_fg[];
extern "C"	frotzwin wp[];
extern "C"	HGDIOBJ	bkgBrush, startBrush;
extern "C"  HGDIOBJ hgdiOldFont;
extern "C"	void	adjustScreenSize( void );
extern "C"	HBITMAP	hbmScreenBuffer;
extern "C"	HDC		h_memDC;
extern "C"	long	nScrollPos;
extern "C"	int cwin;
extern "C"	int	show_cursor;
extern "C"	int	status_last_char;
extern "C"	COLORREF bkgBrushcolor;
extern "C"	int	input_pos;
extern "C"	void drawStatusArea( void );
extern "C"  void AddStyleFonts( void );
extern "C"	void resetScrollPos( void );
extern "C"	int		mouse_x, mouse_y;
extern "C"	int		kbBuffer[];
extern "C"	int		kbIndex;
extern "C"	char	kbModals[];
extern "C"	void	ComputeFontWidths( void );
extern "C"	BOOL MakeMemPalette( void );
extern "C" HGDIOBJ		oldMemBmap;
extern "C" char		*story_name;
extern "C" int		current_font_height;
extern "C" HDC	hdc;

extern	int	bResize;
extern	long	nLockXSize, nLockYSize;
extern	int		graphics_screen_size;
extern	BOOL	killZThread( void );
extern	BOOL	startThread( CDC*);
extern	void WriteRegDWORD( HKEY hkInput, char *item, DWORD value);
extern	void WriteRegBinary( HKEY hkInput, char *item, unsigned char *data, int size );

extern	CSize	csNormalDim;
CPoint		curPos;
static	int	cursorWidth = 0;
COLORREF	custColors[17];
extern	"C"	int			bmapX, bmapY;
CClientDC	*m_pDC;
extern "C" HPALETTE	hmemPal;

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzView

IMPLEMENT_DYNCREATE(CWinFrotzView, CView)

BEGIN_MESSAGE_MAP(CWinFrotzView, CView)
	//{{AFX_MSG_MAP(CWinFrotzView)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_FONT, OnViewFont)
	ON_COMMAND(ID_COLORS_USERFG, OnColorsUserfg)
	ON_COMMAND(ID_COLORS_USERBG, OnColorsUserbg)
	ON_COMMAND(ID_VIEW_STATUS_FONT, OnViewStatusFont)
	ON_COMMAND(ID_COLORS_USERINVBG, OnColorsUserinvbg)
	ON_COMMAND(ID_COLORS_USERINVFG, OnColorsUserinvfg)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_COLORS_EMPHASIS, OnColorsEmphasis)
	ON_COMMAND(ID_COLORS_BOLD, OnColorsBold)
	ON_UPDATE_COMMAND_UI(ID_COLORS_EMPHASIS, OnUpdateColorsEmphasis)
	ON_UPDATE_COMMAND_UI(ID_COLORS_BOLD, OnUpdateColorsBold)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_CURSOR_POS, OnCursorPos)
	ON_MESSAGE(WM_VERSION_6, OnV6Game)
	ON_MESSAGE(WM_CURSOR_HIDE, OnCursorHide)
	ON_MESSAGE(WM_THREAD_DONE, OnThreadDone)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzView construction/destruction

CWinFrotzView::CWinFrotzView()
{
	//LOGFUNCTION
	m_pDC = NULL;
}

CWinFrotzView::~CWinFrotzView()
{
	//LOGFUNCTION
}

int CWinFrotzView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	//LOGFUNCTION
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    m_pDC = new CClientDC(this);
    ASSERT(m_pDC != NULL);

	return 0;
}

void CWinFrotzView::OnDestroy() 
{
	//LOGFUNCTION
	CView::OnDestroy();
	int i;
	
	for( i = 0; i < NUM_FONT_TYPES; i++ )
	{
		if( i == ZORK_FONT )
		{
			if( fpFonts[i].hfHandle )
			{
				DeleteObject( fpFonts[i].hfHandle );
				fpFonts[i].hfHandle = NULL ;
				::RemoveFontResource( "BEYONDZO.FON" );
				::SendMessage( HWND_BROADCAST, WM_FONTCHANGE, 0, 0 );
			}
		}
		else
			if( fpFonts[i].hfHandle )
			{
				DeleteObject( fpFonts[i].hfHandle );
				fpFonts[i].hfHandle = NULL ;
			}
	}

	if( m_pDC )
		delete m_pDC;
	m_pDC = NULL;

	if( hbmScreenBuffer )
		::DeleteObject( hbmScreenBuffer );
	hbmScreenBuffer = NULL;

	if( oldMemBmap )
		::SelectObject( h_memDC, oldMemBmap );
	oldMemBmap = NULL;

	if( h_memDC )
		::DeleteDC( h_memDC );
	h_memDC = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzView drawing

void CWinFrotzView::OnDraw(CDC* pDC)
{
	//LOGFUNCTION
	if( hThread )
	{
		int		statsize;
		HPALETTE hOldPal=NULL;        // Previous palette

		if( hmemPal )
		{
			hOldPal = ::SelectPalette( hdc, hmemPal, FALSE ); 
			::RealizePalette(hdc);
		}

		statsize = wp[1].y_size + wp[7].y_size;
		if( statsize )
			::BitBlt( hdc, 0, 0, h_screen_width, statsize, h_memDC, 0, 0, SRCCOPY );
		::BitBlt( hdc, 0, statsize, h_screen_width, h_screen_height - statsize, h_memDC, 0, nScrollPos + statsize, SRCCOPY );

		if ( hOldPal )
			::SelectPalette( hdc, hmemPal, TRUE );
		return;
	}
	
	if( !bStoryInit )
	{
		if( bStoryLoaded && !hThread)
		{
			CWnd	*pWnd;

			pWnd = m_pDC->GetWindow( );
			hWnd = pWnd->m_hWnd;

			if( hdc )
				::ReleaseDC( hWnd, hdc );
			hdc = ::GetDC( hWnd );
			ASSERT( hWnd != NULL );
			ASSERT( hdc != NULL );

			if( !startThread( m_pDC  ) )
				MessageBox( "Could not launch the Z-code thread!" );
		}
	}
	else
	{
		CBrush	*cbTmp, *old;
		RECT	rc;

		cbTmp = new CBrush( user_background );
		if( cbTmp )
		{
			GetClientRect( &rc );
			old = (CBrush *)pDC->SelectObject( cbTmp );
			pDC->PatBlt( 0, 0, rc.right, rc.bottom, PATCOPY );
			pDC->SelectObject( old );
			delete cbTmp;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzView printing

BOOL CWinFrotzView::OnPreparePrinting(CPrintInfo* pInfo)
{
	//LOGFUNCTION
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWinFrotzView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	//LOGFUNCTION
	// TODO: add extra initialization before printing
}

void CWinFrotzView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	//LOGFUNCTION
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzView diagnostics

#ifdef _DEBUG
void CWinFrotzView::AssertValid() const
{
	CView::AssertValid();
}

void CWinFrotzView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWinFrotzDoc* CWinFrotzView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinFrotzDoc)));
	return (CWinFrotzDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzView message handlers

LRESULT CWinFrotzView::OnCursorPos(WPARAM wParam, LPARAM lParam)
{
	//LOGFUNCTION
	int	cursorfont = STANDARD;

	curPos.x = curCoord.X;
	curPos.y = curCoord.Y + fpFonts[cursorfont].nHeight -1;
	cursorWidth = 0;

	if( curPos.x == 0 || !show_cursor )
		return 0;

	if( cwin ) 
	{
		cursorfont = FIXED;
		cursorWidth = max(fpFonts[cursorfont].nWidths['X'], ::GetSystemMetrics( SM_CXBORDER));
		CreateSolidCaret( cursorWidth, cursorHeight );
	}
	else
	{
		if( lParam && nState == OS_READ )
		{
			cursorWidth = max(fpFonts[cursorfont].nWidths[(char )wParam], ::GetSystemMetrics( SM_CXBORDER));
			CreateSolidCaret( cursorWidth, cursorHeight );
		}
		else if( nState == WAIT_FOR_KEY )
		{
			cursorWidth = max(fpFonts[cursorfont].nWidths['X'], ::GetSystemMetrics( SM_CXBORDER));
			CreateSolidCaret( cursorWidth, cursorHeight );
		}
	}

	SetCaretPos( curPos );

	ShowCaret();

	return 1;
}

LRESULT CWinFrotzView::OnCursorHide(WPARAM wParam, LPARAM lParam)
{
	//LOGFUNCTION
	HideCaret();
	DestroyCaret();
	return 1;
}

LRESULT CWinFrotzView::OnV6Game(WPARAM wParam, LPARAM lParam)
{
	//LOGFUNCTION
	//This is a version 6 game, so we have to 1) Resize the screen to an integral boundary,
	//2) load a special font and copy it to display/standard font styles, 3) Lock the screen
	//with bResize

	//KillZThread should be the unwind for all of this where we set things back the way they
	//were before (RESTART would only restart the same type of game, File/Open will result
	//in a KillZThread call)
	RECT	rcWindow, rcThis;
	int		xoffset;
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd( );

	pMain->GetWindowRect( &rcWindow );
	::GetWindowRect( hWnd, &rcThis );
	
	//Why the hell am I not using NONCLIENTMETRICs here? Just stupid I guess.
	xoffset = (rcWindow.right - rcWindow.left) - (rcThis.right - rcThis.left) + GetSystemMetrics( SM_CXBORDER )*4;
	nMinYMod = (rcWindow.bottom - rcWindow.top) - (rcThis.bottom - rcThis.top) + GetSystemMetrics( SM_CXBORDER )*4;

	csNormalDim.cx = rcWindow.right - rcWindow.left;
	csNormalDim.cy = rcWindow.bottom - rcWindow.top;

	if( graphics_screen_size == 0 )
	{
		pMain->SetWindowPos( &wndTop, 0, 0, 
			640 + xoffset, 
			400 + nMinYMod, SWP_NOMOVE );
	}
	
	if( graphics_screen_size == 1 )
	{
		pMain->SetWindowPos( &wndTop, 0, 0, 
			960 + xoffset, 
			600 + nMinYMod, SWP_NOMOVE );
	}

	if( graphics_screen_size == 2 )
	{
		pMain->SetWindowPos( &wndTop, 0, 0, 
			1280 + xoffset, 
			800 + nMinYMod, SWP_NOMOVE );
	}

	pMain->GetWindowRect( &rcWindow );
	nLockXSize = rcWindow.right - rcWindow.left;
	nLockYSize = rcWindow.bottom - rcWindow.top;

	bResize = FALSE;
	ResumeThread( hThread );
	return 1;
}

void CWinFrotzView::OnKillFocus(CWnd* pNewWnd) 
{
	//LOGFUNCTION
	if( hWnd )
	{
		HideCaret();
		DestroyCaret();
	}

	CView::OnKillFocus(pNewWnd);
}

void CWinFrotzView::OnSetFocus(CWnd* pOldWnd) 
{
	//LOGFUNCTION
	if( bStoryInit & show_cursor )
	{
		int	cursorfont = STANDARD;

		if( cwin ) 
		{
			cursorfont = FIXED;
			CreateSolidCaret( cursorWidth, cursorHeight );
		}
		else
		{
			if( cursorWidth && nState == OS_READ )
				CreateSolidCaret( cursorWidth, cursorHeight );
			else if( nState == WAIT_FOR_KEY )
				CreateSolidCaret( cursorWidth, cursorHeight );
		}

		curPos.x = curCoord.X;
		curPos.y = curCoord.Y + fpFonts[cursorfont].nHeight -1;

		SetCaretPos( curPos );

		ShowCaret();
	}
	CView::OnSetFocus(pOldWnd);
}

void CWinFrotzView::OnInitialUpdate() 
{
	//LOGFUNCTION
	ComputeFontWidths();
	adjustScreenSize();

	CView::OnInitialUpdate();
}

void CWinFrotzView::OnViewFont() 
{
	//LOGFUNCTION
	HFONT	hfTemp = NULL;

	CMyFontDialog dlg( &fpFonts[STANDARD].lf );
	dlg.m_cf.Flags |= CF_FORCEFONTEXIST;
	dlg.SetWindowName( "Pick standard display font" );

	//Store the font if the user accepts it, and create points for the emphasis
	//(underline/italic) and bold variants

	if (dlg.DoModal() == IDOK)
	{
		if( (hfTemp = (HFONT)CreateFontIndirect( &fpFonts[STANDARD].lf ))!=NULL )
		{
			if( fpFonts[STANDARD].hfHandle )
				DeleteObject( fpFonts[STANDARD].hfHandle );
			fpFonts[STANDARD].hfHandle = hfTemp;
			WriteRegBinary( NULL, ITEM_DISPLAY_FONT, (unsigned char *)&fpFonts[STANDARD].lf, sizeof( LOGFONT) );
		}
		else
			MessageBox("Could not load new display font");

		memcpy( &fpFonts[BOLD].lf, &fpFonts[STANDARD].lf, sizeof( LOGFONT ) );
		fpFonts[BOLD].lf.lfWeight = 700;
		if( (hfTemp = CreateFontIndirect( &fpFonts[BOLD].lf ))!=NULL )
		{
			if( fpFonts[BOLD].hfHandle )
				DeleteObject( fpFonts[BOLD].hfHandle );
			fpFonts[BOLD].hfHandle = hfTemp;
		}
		else
			MessageBox("Could not load bold font");

		memcpy( &fpFonts[EMPHASIS].lf, &fpFonts[STANDARD].lf, sizeof( LOGFONT ) );
		fpFonts[EMPHASIS].lf.lfItalic = TRUE;
		if( (hfTemp = CreateFontIndirect( &fpFonts[EMPHASIS].lf ))!=NULL )
		{
			if( fpFonts[EMPHASIS].hfHandle )
				DeleteObject( fpFonts[EMPHASIS].hfHandle );
			fpFonts[EMPHASIS].hfHandle = hfTemp;
		}
		else
			MessageBox("Could not load italic font");

		memcpy( &fpFonts[UNDERLINE].lf, &fpFonts[STANDARD].lf, sizeof( LOGFONT ) );
		fpFonts[UNDERLINE].lf.lfUnderline = TRUE;
		if( (hfTemp = CreateFontIndirect( &fpFonts[UNDERLINE].lf ))!=NULL )
		{
			if( fpFonts[UNDERLINE].hfHandle )
				DeleteObject( fpFonts[UNDERLINE].hfHandle );
			fpFonts[UNDERLINE].hfHandle = hfTemp;
		}
		else
			MessageBox("Could not load underline font");

		ComputeFontWidths();

		if( dlg.m_cf.rgbColors != RGB( 0, 0, 0) )
			SetColorFG( dlg.m_cf.rgbColors );

		if( m_pDC )
		{
			int			oldY, cur_last_row_pos;
			CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd( );

			resetScrollPos();

			oldY = nMinY;
			nMinY = fpFonts[STANDARD].nHeight * MIN_VERT_CHARS;

			if( nMinY > h_screen_height )
			{
				h_screen_height = MIN_VERT_CHARS * fpFonts[STANDARD].nHeight;
				pMain->SetWindowPos( &wndTop, 0, 0, 
					h_screen_width + GetSystemMetrics( SM_CXEDGE )*2, 
					h_screen_height + nMinYMod, SWP_NOMOVE );
			}
			pMain->UpdateStatusBar();

			cur_last_row_pos = (h_screen_height / fpFonts[STANDARD].nHeight) 
				* fpFonts[STANDARD].nHeight - fpFonts[STANDARD].nHeight;
			if( curCoord.Y > cur_last_row_pos )
			{
				int i, status = wp[1].y_size + wp[7].y_size;
				i = curCoord.Y - cur_last_row_pos;
				::BitBlt( h_memDC, 0, status , bmapX, h_screen_height - (status + i), 
								   h_memDC, 0, status + i + nScrollPos, SRCCOPY );
				curCoord.Y -= i;
				wp[0].y_cursor -= i;
			}

			if( hgdiOldFont )
				::SelectObject( h_memDC, hgdiOldFont );
			hgdiOldFont = ::SelectObject( h_memDC, fpFonts[STANDARD].hfHandle );

			adjustScreenSize();
			if( nState == OS_READ )
			{
				nState = CHANGE_FONT;
				DestroyCaret();
				ResumeThread( hThread );
			}
		}
	}
}

void CWinFrotzView::OnViewStatusFont() 
{
	//LOGFUNCTION
	int			oldfontwidth, oldfontheight, i;
	HFONT		hfTemp = NULL;
	LOGFONT		lfTemp;

	if( bStoryLoaded )
	{
		if( cwin != 0 || nState!=OS_READ )
		{
			int	bResult;
			bResult = MessageBox( FIXED_FONT_WARNING, "WinFrotz Warning", MB_YESNO|MB_ICONQUESTION|MB_APPLMODAL );
			if( bResult!=IDYES )
				return;
		}
	}
	
	oldfontwidth = fpFonts[FIXED].nWidths['X'];
	oldfontheight = fpFonts[FIXED].nHeight;
	memcpy( &lfTemp, &fpFonts[FIXED].lf, sizeof( LOGFONT ) );

	CMyFontDialog dlg( &fpFonts[FIXED].lf );
	dlg.m_cf.Flags |= CF_FORCEFONTEXIST | CF_FIXEDPITCHONLY;
	dlg.SetWindowName( "Pick status (fixed) font" );

	//Store the font if the user accepts it
	if (dlg.DoModal() == IDOK)
	{
		if( (hfTemp = CreateFontIndirect( &fpFonts[FIXED].lf ))!=NULL )
		{
			if( fpFonts[FIXED].hfHandle )
				DeleteObject( fpFonts[FIXED].hfHandle );
			fpFonts[FIXED].hfHandle = hfTemp;
			WriteRegBinary( NULL, ITEM_FIXED_FONT, (unsigned char *)&fpFonts[FIXED].lf, sizeof( LOGFONT ) );
		}
		else
			MessageBox("Could not load new font!");

		ComputeFontWidths();

		if( dlg.m_cf.rgbColors != user_reverse_fg && dlg.m_cf.rgbColors!=RGB( 0, 0, 0) )
		{
			user_reverse_fg = dlg.m_cf.rgbColors;
			MakeMemPalette( );

			drawStatusArea();
			WriteRegDWORD( NULL, ITEM_INV_FOREGROUND, (DWORD)user_reverse_fg );
			WriteRegBinary( NULL, ITEM_CUSTCOLORS, (unsigned char *)&custColors[0], sizeof( COLORREF ) * 16 );
		}

		if( status_last_char && fpFonts[FIXED].nWidths['X'] * status_last_char > GetSystemMetrics( SM_CXFULLSCREEN ) )
		{
			MessageBox( FIXED_FONT_TOO_LARGE, "WinFrotz Error", MB_ICONSTOP );
			memcpy( &fpFonts[FIXED].lf, &lfTemp, sizeof( LOGFONT ) );

			if( (hfTemp = CreateFontIndirect( &fpFonts[FIXED].lf ))!=NULL )
			{
				DeleteObject( fpFonts[FIXED].hfHandle );
				fpFonts[FIXED].hfHandle = hfTemp;
			}
			else
				MessageBox("Could not re-load old font!");
			ComputeFontWidths();
			return;
		}

		if( h_memDC )
		{
			int oldysize, count = 0, statsize, statwin;
			int	deltax, deltay;
			BOOL	bCursorInStatusWin = FALSE;
			HBITMAP	hbmTemp, hbmTemp2;
			HDC		hdcMem;
			HGDIOBJ	oldBrush, brush;
			CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd( );

			if( wp[1].y_size )
				statwin = 1;
			else
				statwin = 7;

			statsize = wp[statwin].y_size;
			if( statsize )
			{	
				//ARGH! The cursor is in the status window, and therefore has to be repositioned
				//to fit the new font dimensions
				if( curCoord.Y <= wp[1].y_size )
				{
					int tempx, tempy;
					bCursorInStatusWin = TRUE;
					tempx = curCoord.X / oldfontwidth;
					tempy = curCoord.Y / oldfontheight;
					curCoord.X = tempx * fpFonts[FIXED].nWidths['X'];
					curCoord.Y = tempy * fpFonts[FIXED].nHeight;
				}
				oldysize = statsize;
				i = (oldysize / oldfontheight);

				wp[statwin].y_size = (zword)(i * fpFonts[FIXED].nHeight);
				wp[statwin].x_size = fpFonts[FIXED].nWidths['X'] * status_last_char;

				//Need to allocate a new bitmap and fill it with the background color,
				//then what was already in window 0

				deltax = fpFonts[FIXED].nWidths['X'] * status_last_char - bmapX;
				if( deltax > 0 )
					bmapX = fpFonts[FIXED].nWidths['X'] * status_last_char;
				
				deltay = fpFonts[ FIXED].nHeight * i - oldysize;
				if( deltay > 0 )
					bmapY += deltay*2;
				//curCoord.Y += deltay;
				
				//Create a bitmap compatible with screen, select it into mem dc
				hdcMem = ::CreateCompatibleDC( h_memDC );
				if( display < MCGA_MODE )
				{
					hbmTemp = ::CreateBitmap( bmapX, bmapY, 1, 1, NULL );
					brush = ::CreateSolidBrush( RGB( 0, 0, 0) );
				}
				else
				{
					hbmTemp = ::CreateCompatibleBitmap( m_pDC->m_hDC, bmapX, bmapY);
	 				brush = ::CreateSolidBrush( user_background );
				}
				hbmTemp2 = (HBITMAP)::SelectObject( hdcMem, hbmTemp );

				ASSERT( brush );
				oldBrush = ::SelectObject( hdcMem, brush );
				ASSERT( oldBrush );
				::PatBlt( hdcMem, 0, 0, bmapX, bmapY, PATCOPY );
				::SelectObject( hdcMem, oldBrush );
				::DeleteObject( brush );

				statsize = oldysize / oldfontheight * status_last_char;

				if( !bCursorInStatusWin )
				{
					curCoord.Y += deltay;
					if( deltay > 0 )
					{	
						int padspace = (h_screen_height - fpFonts[STANDARD].nHeight) - curCoord.Y;
						//The status bar is larger than it was before
						//Do we have enough space under the cursor to cover the increased size?
						if( padspace < deltay )
							h_screen_height += deltay - padspace;
					}
				}
				else
					h_screen_height += deltay;

				if( deltax > 0 )
					h_screen_width = bmapX;

				//Copy of the contents of the screen other than the status window
				//::BitBlt( hdcMem, 0, wp[statwin].y_size, h_screen_width, h_screen_height - oldysize, h_memDC, 0, oldysize + nScrollPos, SRCCOPY );
				::BitBlt( hdcMem, 0, wp[statwin].y_size, h_screen_width, h_screen_height - wp[statwin].y_size, h_memDC, 0, oldysize + nScrollPos, SRCCOPY );

				nScrollPos = 0;
				curPos.y = curCoord.Y + fpFonts[STANDARD].nHeight -1;
				SetCaretPos( curPos );
				::SelectObject( hdcMem, hbmTemp2 );
				::SelectObject( h_memDC, oldMemBmap );
				::DeleteObject( hbmScreenBuffer );
				::DeleteDC( hdcMem );
				hbmScreenBuffer = hbmTemp;
				oldMemBmap = ::SelectObject( h_memDC, hbmScreenBuffer );

				AddStyleFonts();

				pMain->SetWindowPos( &wndTop, 0, 0, 
					h_screen_width + GetSystemMetrics( SM_CXEDGE )*2 + GetSystemMetrics( SM_CXSIZEFRAME )*2, 
					h_screen_height + nMinYMod, SWP_NOMOVE );
				drawStatusArea();
			}
			adjustScreenSize();
		}
	}
}

void CWinFrotzView::OnColorsUserfg() 
{
	//LOGFUNCTION
	if( GetAsyncKeyState( VK_SHIFT ) < 0 )
		OnColorsUserinvfg();
	else
	{
		CMyColorDialog dlg;

		dlg.m_cc.lpCustColors = custColors;
		dlg.m_cc.Flags |= CC_RGBINIT;
		dlg.m_cc.rgbResult = (COLORREF)user_foreground;
		dlg.SetWindowName( "Pick Foreground Color" );
		if( dlg.DoModal() == IDOK && dlg.m_cc.rgbResult!=user_foreground )
		{
			SetColorFG( dlg.m_cc.rgbResult );
		}
	}
}

void CWinFrotzView::SetColorFG( COLORREF new_foreground_color )
{
	//LOGFUNCTION
	if( user_foreground != new_foreground_color )
	{
		user_foreground = new_foreground_color;
		MakeMemPalette( );
		if( display > CGA_MODE )
		{
			text_fg = user_foreground;
			::SetTextColor( h_memDC, text_fg );
			pc_colour_fg[ 14 ] = user_foreground;
		}
		else
		{
			::SetBkColor( hdc, user_foreground );
			Invalidate();
			UpdateWindow();
		}
		WriteRegDWORD( NULL, ITEM_FOREGROUND, (DWORD)user_foreground );
		WriteRegBinary( NULL, ITEM_CUSTCOLORS, (unsigned char *)&custColors[0], sizeof( COLORREF ) * 16 );
	}
}

void CWinFrotzView::OnColorsUserbg() 
{
	//LOGFUNCTION
	if( GetAsyncKeyState( VK_SHIFT ) < 0 )
		OnColorsUserinvbg();
	else
	{
		CMyColorDialog dlg;
		
		dlg.m_cc.lpCustColors = custColors;
		dlg.m_cc.Flags |= CC_RGBINIT;
		dlg.m_cc.rgbResult = (COLORREF)user_background;
		dlg.SetWindowName( "Pick Background Color" );
		if( dlg.DoModal() == IDOK && dlg.m_cc.rgbResult!=user_background )
		{
			user_background = dlg.m_cc.rgbResult;
			MakeMemPalette( );
			if( display > CGA_MODE )
			{
				text_bg = user_background;
				::SetBkColor( h_memDC, user_background );
				pc_colour_fg[ 15 ] = user_background;
				ASSERT( startBrush );
				SelectObject( h_memDC, startBrush );
				ASSERT( bkgBrush );
				DeleteObject( bkgBrush );
				bkgBrush = CreateSolidBrush( user_background );
				ASSERT( bkgBrush );
				bkgBrushcolor = user_background;
				startBrush = SelectObject( h_memDC, bkgBrush );
				ASSERT( startBrush );
				::PatBlt( h_memDC, curCoord.X, curCoord.Y + nScrollPos, bmapX - curCoord.X, bmapY - curCoord.Y + nScrollPos, PATCOPY );
				::PatBlt( h_memDC, 0, curCoord.Y + nScrollPos + current_font_height, h_screen_width, bmapY - curCoord.Y + nScrollPos + current_font_height, PATCOPY );	
			}
			else
			{
				::SetTextColor( hdc, user_background );
			}
			Invalidate();
			UpdateWindow();
			WriteRegDWORD( NULL, ITEM_BACKGROUND, (DWORD)user_background );
			WriteRegBinary( NULL, ITEM_CUSTCOLORS, (unsigned char *)&custColors[0], sizeof( COLORREF ) * 16 );
		}
	}
}


void CWinFrotzView::OnColorsUserinvfg() 
{
	//LOGFUNCTION
	CMyColorDialog dlg;

	dlg.m_cc.lpCustColors = custColors;
	dlg.m_cc.Flags |= CC_RGBINIT;
	dlg.m_cc.rgbResult = (COLORREF)user_reverse_fg;
	dlg.SetWindowName( "Reverse Foreground Color" );
	if( dlg.DoModal() == IDOK && dlg.m_cc.rgbResult!=user_reverse_fg )
	{
		user_reverse_fg = dlg.m_cc.rgbResult;
		MakeMemPalette( );

		drawStatusArea();
		WriteRegDWORD( NULL, ITEM_INV_FOREGROUND, (DWORD)user_reverse_fg );
		WriteRegBinary( NULL, ITEM_CUSTCOLORS, (unsigned char *)&custColors[0], sizeof( COLORREF ) * 16 );
	}
}

void CWinFrotzView::OnColorsUserinvbg() 
{
	//LOGFUNCTION
	CMyColorDialog dlg;
	
	dlg.m_cc.lpCustColors = custColors;
	dlg.m_cc.Flags |= CC_RGBINIT;
	dlg.m_cc.rgbResult = (COLORREF)user_reverse_bg;
	dlg.SetWindowName( "Reverse Background Color" );
	if( dlg.DoModal() == IDOK && dlg.m_cc.rgbResult!=user_reverse_bg )
	{	
		user_reverse_bg = dlg.m_cc.rgbResult;		
		MakeMemPalette( );

		drawStatusArea();
		WriteRegDWORD( NULL, ITEM_INV_BACKGROUND, (DWORD)user_reverse_bg );
		WriteRegBinary( NULL, ITEM_CUSTCOLORS, (unsigned char *)&custColors[0], sizeof( COLORREF ) * 16 );
	}
}

BOOL CWinFrotzView::OnEraseBkgnd(CDC* pDC) 
{
	//LOGFUNCTION
	if( bStoryInit )
		return 1;
	else
	{
		CBrush	*cbTmp, *old;
		RECT	rc;

		cbTmp = new CBrush( user_background );
		if( cbTmp )
		{
			GetClientRect( &rc );
			old = (CBrush *)pDC->SelectObject( cbTmp );
			pDC->PatBlt( 0, 0, rc.right, rc.bottom, PATCOPY );
			pDC->SelectObject( old );
			delete cbTmp;
		}
		return 1;
	}
	return CView::OnEraseBkgnd(pDC);
}

void CWinFrotzView::OnSize(UINT nType, int cx, int cy) 
{
	//LOGFUNCTION
	int		fullcy = cy*2;
	int		statsize, statxsize;

	statsize = wp[1].y_size + wp[7].y_size;

	if( wp[1].x_size )
		statxsize = wp[1].x_size;
	else
		statxsize = wp[7].x_size;
	
	resetScrollPos();

	if( hbmScreenBuffer )
	{
		HDC	hdcMemTemp;
		HGDIOBJ oldBmap2, cbmNew;
		HGDIOBJ	brush = NULL, oldBrush;

		int	delta;

		HideCaret();

		hdcMemTemp = ::CreateCompatibleDC( m_pDC->m_hDC );

		// Make a new bitmap the size of the screen and select it 
		//into memory context 2
		if( display < MCGA_MODE )
			cbmNew = ::CreateBitmap( cx, fullcy, 1, 1, NULL );
		else
			cbmNew = ::CreateCompatibleBitmap( m_pDC->m_hDC, cx, fullcy);
		if( cbmNew ==NULL )
		{
			MessageBox( "Could not create resized bitmap!" );
			killZThread();
			exit( 1 );
		}
		oldBmap2 = ::SelectObject( hdcMemTemp, cbmNew );

		brush = ::CreateSolidBrush( text_bg );
		oldBrush = ::SelectObject( hdcMemTemp, brush);
		::PatBlt( hdcMemTemp, 0, statsize, cx, fullcy - statsize, PATCOPY);
		::PatBlt( hdcMemTemp, statxsize, 0, cx - statxsize, fullcy, PATCOPY);
		if( bmapY > fullcy )
		{
			//Screen is smaller vertically. If the cursor is off the bottom, 
			//we need to land it on the last character row and scroll the 
			//screen far enough up so the last row shown is aligned on a row position

			if( curCoord.Y >= cy - fpFonts[STANDARD].nHeight )
			{
				int	cur_last_row_pos;
				
				//How many lines of text in window (minus one for the input line)?
				cur_last_row_pos = cy  / fpFonts[STANDARD].nHeight -1;
				
				//Convert to pixel lines 
				cur_last_row_pos *= fpFonts[STANDARD].nHeight;

				//Figure out how many pixels to scroll up from our previous display
				delta = curCoord.Y - cur_last_row_pos;
				
				//We always want the status area the same
				::BitBlt( hdcMemTemp, 0, 0, bmapX, statsize, h_memDC, 0, 0, SRCCOPY );

				::BitBlt( hdcMemTemp, 0, statsize , bmapX, bmapY - (statsize + delta), 
							   h_memDC, 0, statsize + delta, SRCCOPY );

				curCoord.Y -= delta;
				wp[0].y_cursor -= delta;
			}
			else
			{
				::BitBlt( hdcMemTemp, 0, 0, min( cx, h_screen_width), h_screen_height, h_memDC, 0, 0, SRCCOPY );
			}
		}
		else	//Screen is either smaller or larger, but room for entire current display
		{
			::BitBlt( hdcMemTemp, 0, 0, min( cx, h_screen_width), h_screen_height, h_memDC, 0, 0, SRCCOPY );
		}
	
		SelectObject( hdcMemTemp, oldBrush );
		DeleteObject(brush);

		bmapX = cx;
		bmapY = fullcy;
		::SelectObject( hdcMemTemp, oldBmap2 );

		::SelectObject( h_memDC, oldMemBmap );
		::DeleteObject( hbmScreenBuffer );
		hbmScreenBuffer = (HBITMAP)cbmNew;
		oldMemBmap = ::SelectObject( h_memDC, hbmScreenBuffer );

		::DeleteDC( hdcMemTemp );
		curPos.x = curCoord.X;
		if( !cwin )
			curPos.y = curCoord.Y + fpFonts[STANDARD].nHeight -1;
		else
			curPos.y = curCoord.Y + fpFonts[FIXED].nHeight -1;
		SetCaretPos( curPos );
	}
	
	h_screen_width = cx;
	h_screen_height = cy;
	
	adjustScreenSize();

	CView::OnSize(nType, cx, cy);

	if( hWnd )
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd( );
		pMain->UpdateStatusBar( );
	}
}

void CWinFrotzView::OnColorsEmphasis() 
{
	//LOGFUNCTION
	CMyColorDialog dlg;

	dlg.m_cc.lpCustColors = custColors;
	dlg.m_cc.Flags |= CC_RGBINIT;
	dlg.m_cc.rgbResult = (COLORREF)user_emphasis;
	dlg.SetWindowName( "Pick Emphasis Color" );
	if( dlg.DoModal() == IDOK )
		user_emphasis = dlg.m_cc.rgbResult;
	WriteRegDWORD( NULL, ITEM_EMPHASIS, (DWORD)user_emphasis );	
	WriteRegBinary( NULL, ITEM_CUSTCOLORS, (unsigned char *)&custColors[0], sizeof( COLORREF ) * 16 );
}

void CWinFrotzView::OnUpdateColorsEmphasis(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	if( display > CGA_MODE )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );	
}

void CWinFrotzView::OnColorsBold() 
{
	//LOGFUNCTION
	CMyColorDialog dlg;

	dlg.m_cc.lpCustColors = custColors;
	dlg.m_cc.Flags |= CC_RGBINIT;
	dlg.m_cc.rgbResult = (COLORREF)user_bold;
	dlg.SetWindowName( "Pick Bold Color" );
	if( dlg.DoModal() == IDOK )
		user_bold = dlg.m_cc.rgbResult;
	WriteRegDWORD( NULL, ITEM_BOLD, (DWORD)user_bold );
	WriteRegBinary( NULL, ITEM_CUSTCOLORS, (unsigned char *)&custColors[0], sizeof( COLORREF ) * 16 );
}

void CWinFrotzView::OnUpdateColorsBold(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	if( display > CGA_MODE )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );		
}

void CWinFrotzView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//LOGFUNCTION
	if( kbIndex < KEYBOARD_BUFFER_SIZE && (nState == OS_READ || nState==WAIT_FOR_KEY) )
	{
		mouse_x = point.x;
		mouse_y = point.y;
		kbBuffer[kbIndex]=ZC_SINGLE_CLICK;
		kbModals[kbIndex++]=MOUSE_CLICK;
		DestroyCaret();
		ResumeThread( hThread );
	}
	CView::OnLButtonUp(nFlags, point);
}

void CWinFrotzView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//LOGFUNCTION
	if( kbIndex < KEYBOARD_BUFFER_SIZE && (nState == OS_READ || nState==WAIT_FOR_KEY) )
	{
		mouse_x = point.x;
		mouse_y = point.y;
		kbBuffer[kbIndex]=ZC_DOUBLE_CLICK;
		kbModals[kbIndex++]=MOUSE_CLICK;
		DestroyCaret();
		ResumeThread( hThread );
	}	
	CView::OnLButtonDblClk(nFlags, point);
}

LRESULT CWinFrotzView::OnThreadDone(WPARAM wParam, LPARAM lParam)
{
	//LOGFUNCTION
	RECT	rc;
	CDocument *pDoc = GetDocument();
//	char	title[512];

	HideCaret();
	GetClientRect( &rc );
	if( hThread )
	{
		killZThread();
		pDoc->SetTitle( "No story file" );
		pDoc->OnNewDocument();
		InvalidateRect( &rc, TRUE );
	}
	return 1;
}

void CWinFrotzView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	//LOGFUNCTION
//	int	xRes, iMult, statsize;

	//Find the largest integral multiple of width on the page
/*	xRes = pDC->GetDeviceCaps( HORZRES );
	iMult = xRes / h_screen_width;

	statsize = wp[1].y_size + wp[7].y_size;
	if( statsize )
		pDC->BitBlt( 0, 0, h_screen_width * iMult, statsize * iMult, h_memDC, 0, 0, SRCCOPY );
	pDC->BitBlt( 0, statsize, h_screen_width * iMult, (h_screen_height - statsize), h_memDC, 0, nScrollPos + statsize, SRCCOPY );*/
	
	CView::OnPrint(pDC, pInfo);
}


