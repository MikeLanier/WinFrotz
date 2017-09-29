//---------------------------------------------------------------------------
//| DisplayOpt.cpp : implementation file handles the Display Options...
//| dialog box, which controls how Bold/Emphasis are shown and whether
//| we are in color/mono mode. I really should have used DDX in this one.
//---------------------------------------------------------------------------

#include "../include/stdafx.h"
#include "../include/WinFrotz.h"
#include "../include/DisplayOpt.h"
#include "../include/windefs.h"
#include "../include/globs.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CClientDC	*m_pDC;
extern	int	bResize;
extern void WriteRegDWORD( HKEY hkInput, char *item, DWORD value);
extern DWORD ReadRegDWORD( HKEY hkInput, char *item, BOOL bShowError );


//Since this dialog is highly interactive with the Z-Machine, it uses a bunch
//of C globals.
extern "C"	HGDIOBJ	oldMemBmap;
extern "C"	zbyte	h_version;
extern "C"	void	drawStatusArea( void );
extern "C"	int		reg_display;
extern "C"	fontprops fpFonts[];
extern "C"	int		nUpdates;
extern "C"	BOOL 	bStoryLoaded;
extern "C"	HDC		h_memDC;
extern "C"	HBITMAP	hbmScreenBuffer;
extern "C"	int		bmapX, bmapY;
extern "C"	HGDIOBJ	bkgBrush, startBrush;
extern "C"	zword	h_screen_width;
extern "C"	zword	h_screen_height;
extern "C"	COLORREF user_reverse_fg;
extern "C"	COLORREF user_reverse_bg;
extern "C"  COLORREF pc_colour_fg[];
extern "C"	int		nScrollPos;
extern "C"  fakecoord curCoord;
extern "C"	void	os_set_text_style (int new_style);
extern "C"	int		current_style;

/////////////////////////////////////////////////////////////////////////////
// CDisplayOpt dialog
/////////////////////////////////////////////////////////////////////////////

CDisplayOpt::CDisplayOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayOpt::IDD, pParent)
{
	//LOGFUNCTION
	//{{AFX_DATA_INIT(CDisplayOpt)
	m_nDisplayColor = -1;
	m_nBoldFont = -1;
	m_nEmphFont = -1;
	m_nUpdates = -1;
	m_IsLocked = FALSE;
	//}}AFX_DATA_INIT
}

//There are some data exchange variables defined here in case I needed them,
//but again most of the work is done right here in the dialog box.
void CDisplayOpt::DoDataExchange(CDataExchange* pDX)
{
	//LOGFUNCTION
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayOpt)
	DDX_Radio(pDX, IDC_DISPLAY_COLOR, m_nDisplayColor);
	DDX_Radio(pDX, IDC_BOLD_FONT, m_nBoldFont);
	DDX_Radio(pDX, IDC_EMPH_FONT, m_nEmphFont);
	DDX_Radio(pDX, IDC_VERY_FREQUENT_UPDATES, m_nUpdates);
	DDX_Check(pDX, IDC_LOCK_SIZE, m_IsLocked);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplayOpt, CDialog)
	//{{AFX_MSG_MAP(CDisplayOpt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayOpt message handlers

//Just set all the check boxes to the right positions
BOOL CDisplayOpt::OnInitDialog() 
{
	//LOGFUNCTION
	if( display < MCGA_MODE )
		m_nDisplayColor = 1;
	else
		m_nDisplayColor = 0;
	m_nBoldFont = fpFonts[BOLD].nUsagePerm;
	if( nUpdates == 2)
		m_nUpdates = 0;
	else if( nUpdates == 0)
		m_nUpdates = 2;
	else
		m_nUpdates = 1;
	m_IsLocked = !bResize;
	switch( fpFonts[EMPHASIS].nUsagePerm )
	{
		case DISPLAY_COLOR:
			m_nEmphFont = 1;
			break;

		case DISPLAY_FONT_UNDERLINE:
			m_nEmphFont = 2;
			break;

		case DISPLAY_NO_EFFECT:
			m_nEmphFont = 3;
			break;

		default:
			m_nEmphFont = 0;
			break;
	}

	switch( fpFonts[BOLD].nUsagePerm )
	{
		case DISPLAY_COLOR:
			m_nBoldFont = 1;
			break;

		case DISPLAY_NO_EFFECT:
			m_nBoldFont = 2;
			break;

		default:
			m_nBoldFont = 0;
			break;
	}

	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDisplayOpt::OnOK() 
{
	//LOGFUNCTION
	int	nResult, nEmphResult, nBoldResult;

	if( GetCheckedRadioButton( IDC_VERY_FREQUENT_UPDATES,IDC_VERY_FREQUENT_UPDATES ))
		nUpdates = 2;
	else if( GetCheckedRadioButton( IDC_FREQUENT_UPDATES,IDC_FREQUENT_UPDATES ))
		nUpdates = 1;
	else
		nUpdates = 0;
	nEmphResult = GetCheckedRadioButton( IDC_EMPH_FONT, IDC_EMPH_NO_EFFECT );
	nBoldResult = GetCheckedRadioButton( IDC_BOLD_FONT, IDC_BOLD_NOEFFECT );
	bResize = !IsDlgButtonChecked( IDC_LOCK_SIZE );
	if( h_version == 6 && bResize )
	{
		AfxGetMainWnd()->MessageBox( VER_6_RESIZE, "WinFrotz", MB_ICONSTOP );
		bResize = FALSE;
	}
	nResult = GetCheckedRadioButton( IDC_DISPLAY_COLOR, IDC_DISPLAY_MONOCHROME );

	//I've grabbed everything I need with the GetCheckedRadioButton calls above, so I process
	//the ok here to get rid of the dialog box. This is required because moving from mono to
	//color means I need to copy the current display into a bitmap (and I can't do that while
	//a dialog is over top of it). Doesn't hurt to acknowledge this here anyway.
	CDialog::OnOK();

	if( nResult == IDC_DISPLAY_MONOCHROME && (display > CGA_MODE) )
	{
		//Switching from color to mono isn't possible because the source (color) bitmap can't
		//be copied in any easy way to the destination (mono) bitmap.
		if( bStoryLoaded )
		{
			AfxGetMainWnd()->MessageBox( MONO_WARNING, "WinFrotz", MB_ICONSTOP );
			return;
		}
		display = reg_display = CGA_MODE;

		if( nEmphResult == IDC_EMPH_COLOR )
			nEmphResult = IDC_EMPH_FONT;
		if( nBoldResult == IDC_BOLD_COLOR )
			nBoldResult = IDC_BOLD_FONT;
	}

	//User is switching from mono to color mode, or has selected an option that requires color
	//mode (individual colors for bold, etc).
	if(	(nResult==IDC_DISPLAY_COLOR || nEmphResult == IDC_EMPH_COLOR || nBoldResult == IDC_BOLD_COLOR ) && display < MCGA_MODE )
	{
		if( h_version==6 )
		{
			AfxGetMainWnd()->MessageBox( COLOR_WARNING_V6, "WinFrotz", MB_ICONSTOP );
		}
		else
		{
			nResult = AfxGetMainWnd()->MessageBox( COLOR_WARNING, "WinFrotz", MB_ICONQUESTION | MB_YESNO );
			if( nResult == IDNO )
				nEmphResult = nBoldResult = 0;
			else 
			{	
				//If the user is switching from mono to full color, there's a lot of work to be done - all
				//the internal DDBs have to be recreated in the new mode, the old info has to be copied 
				//over, etc.
				HBITMAP	hbmTemp, hbmTemp2;
				HDC		hdcMem;
				HGDIOBJ	oldBrush;
				DWORD	dwTmp;

				DestroyCaret();
				//First, need to grab the real inverse colors from the registry, because the
				//current ones were set to mono values at init
				dwTmp = ReadRegDWORD( NULL, ITEM_INV_FOREGROUND, FALSE );
				if( dwTmp!=READ_REG_FAIL )
					user_reverse_fg = (COLORREF) dwTmp;

				dwTmp = ReadRegDWORD( NULL, ITEM_INV_BACKGROUND, FALSE );
				if( dwTmp!=READ_REG_FAIL )
					user_reverse_bg = (COLORREF)dwTmp;

				//Create a bitmap compatible with screen, select it into mem dc
				hdcMem = ::CreateCompatibleDC( m_pDC->m_hDC );
				hbmTemp = ::CreateCompatibleBitmap( m_pDC->m_hDC, bmapX, bmapY);
				hbmTemp2 = (HBITMAP)::SelectObject( hdcMem, hbmTemp );

				//Make the global memory brush current with user_background
				::SelectObject( h_memDC, startBrush );
				DeleteObject( bkgBrush );
				bkgBrush = ::CreateSolidBrush( user_background );

				//Select into our temp DC and set the background
				oldBrush = ::SelectObject( hdcMem, bkgBrush );
				::PatBlt( hdcMem, 0, 0, bmapX, bmapY, PATCOPY );

				//Select it out then select it into the global memory DC
				::SelectObject( hdcMem, oldBrush );
				startBrush = SelectObject( h_memDC, bkgBrush );

				//Blit the current screen over to the bitmap, delete old bmap, select new one
				::BitBlt( hdcMem, 0, 0, h_screen_width, h_screen_height, m_pDC->m_hDC, 0, 0, SRCCOPY );
				::SelectObject( hdcMem, hbmTemp2 );
				::SelectObject( h_memDC, oldMemBmap );
				::DeleteObject( hbmScreenBuffer );
				::DeleteDC( hdcMem );
				hbmScreenBuffer = hbmTemp;
				oldMemBmap = ::SelectObject( h_memDC, hbmScreenBuffer );
				nScrollPos = 0;
				display = reg_display = MCGA_MODE;
				drawStatusArea();
				::PostMessage( hWnd, WM_CURSOR_POS, WM_CURSOR_POS, (LPARAM)&curCoord );
				::SetTextColor( h_memDC, user_foreground );
				::SetBkColor( h_memDC, user_background );
				pc_colour_fg[14]= user_foreground;
				pc_colour_fg[15]= user_background;
				os_set_text_style( current_style );
			}
		}
	}

	switch( nEmphResult )
	{
		case	IDC_EMPH_FONT:
			fpFonts[EMPHASIS].nUsagePerm = DISPLAY_FONT;
			break;

		case	IDC_EMPH_COLOR:
			fpFonts[EMPHASIS].nUsagePerm = DISPLAY_COLOR;
			break;

		case	IDC_EMPH_UNDERLINE:
			fpFonts[EMPHASIS].nUsagePerm = DISPLAY_FONT_UNDERLINE;
			break;

		case	IDC_EMPH_NO_EFFECT:
			fpFonts[EMPHASIS].nUsagePerm = DISPLAY_NO_EFFECT;
			break;

		default:
			fpFonts[EMPHASIS].nUsagePerm = DISPLAY_FONT;
			break;
	}

	switch( nBoldResult )
	{
		case	IDC_BOLD_FONT:
			fpFonts[BOLD].nUsagePerm = DISPLAY_FONT;
			break;

		case	IDC_BOLD_COLOR:
			fpFonts[BOLD].nUsagePerm = DISPLAY_COLOR;
			break;

		case	IDC_BOLD_NOEFFECT:
			fpFonts[BOLD].nUsagePerm = DISPLAY_NO_EFFECT;
			break;

		default:
			fpFonts[BOLD].nUsagePerm = DISPLAY_FONT;
			break;
	}

	if( h_version != 6 )
	{
		WriteRegDWORD( NULL, ITEM_RESIZE, (DWORD)bResize );
	}
	WriteRegDWORD( NULL, ITEM_COLORMODE, (DWORD)reg_display );
	WriteRegDWORD( NULL, ITEM_UPDATES, (DWORD)nUpdates );
	WriteRegDWORD( NULL, ITEM_EMPHASIS_FONT_USAGE, (DWORD)fpFonts[EMPHASIS].nUsagePerm );
	WriteRegDWORD( NULL, ITEM_BOLD_FONT_USAGE, (DWORD)fpFonts[BOLD].nUsagePerm );
}
