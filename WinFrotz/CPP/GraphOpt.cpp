// GraphOpt.cpp : implementation file
//

//---------------------------------------------------------------------------
//| GraphOpt.cpp : implementation file handles the Graphics Options...
//| dialog box, which controls how V6 games are handled (what mode is used,
//| how large screen size is, etc. )
//---------------------------------------------------------------------------

#include "../include/stdafx.h"
#include "../include/windefs.h"
#include "../include/WinFrotz.h"
#include "../include/GraphOpt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void WriteRegDWORD( HKEY hkInput, char *item, DWORD value);
extern DWORD ReadRegDWORD( HKEY hkInput, char *item, BOOL bShowError );
extern	void WriteRegBinary( HKEY hkInput, char *item, unsigned char *data, int size );
extern int		graphics_screen_size;
extern "C"	HWND	hWnd;

extern "C"	int		graphics_mode;
extern "C"	fontprops	fpFonts[];
extern "C"	zbyte h_version;

/////////////////////////////////////////////////////////////////////////////
// CGraphOpt dialog


CGraphOpt::CGraphOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CGraphOpt::IDD, pParent)
{
	//LOGFUNCTION
	//{{AFX_DATA_INIT(CGraphOpt)
	m_ScreenSize = -1;
	m_GraphMode = -1;
	//}}AFX_DATA_INIT
}


void CGraphOpt::DoDataExchange(CDataExchange* pDX)
{
	//LOGFUNCTION
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphOpt)
	DDX_Radio(pDX, IDC_SCREEN_640, m_ScreenSize);
	DDX_Radio(pDX, IDC_CGA_MODE, m_GraphMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGraphOpt, CDialog)
	//{{AFX_MSG_MAP(CGraphOpt)
	ON_BN_CLICKED(IDC_GRAPHICS_FONT, OnGraphicsFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphOpt message handlers

BOOL CGraphOpt::OnInitDialog() 
{
	//LOGFUNCTION
	CString tmp;
	CClientDC	dc( this );
	
	SetDlgItemText(IDC_FONT_TYPE, fpFonts[GRAPHICS].lf.lfFaceName );
	//Convert font size into screen pixels
	tmp.Format( "%i", abs( MulDiv( fpFonts[GRAPHICS].lf.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY))) );
	SetDlgItemText(IDC_FONT_SIZE, tmp);

	switch( graphics_mode )
	{
		case	CGA_MODE:
			m_GraphMode = 0;
			break;

		case	EGA_MODE:
			m_GraphMode = 2;
			break;

		default:
			m_GraphMode = 1;
			graphics_mode = MCGA_MODE;
			break;
	}
	m_ScreenSize = graphics_screen_size;

	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CGraphOpt::OnGraphicsFont() 
{
	//LOGFUNCTION
	HFONT		hfTemp = NULL;
	LOGFONT		lfTemp;
	int			height;
	CString		tmp;
	CClientDC	dc( this );

	memcpy( &lfTemp, &fpFonts[GRAPHICS].lf, sizeof( LOGFONT ) );

	CFontDialog dlg( &lfTemp );
	dlg.m_cf.Flags |= CF_FORCEFONTEXIST | CF_FIXEDPITCHONLY;

	//Store the font if the user accepts it
	if (dlg.DoModal() == IDOK)
	{
		//Convert to pixel size
		height = -MulDiv( lfTemp.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY) );
		//V6 games don't seem to like really large or small fonts (integer division errors)
		if( height < 8 || height > 16 )
		{
			if( ( AfxGetMainWnd()->MessageBox( GRAPH_FONT_SIZE_WARN, "WinFrotz", MB_YESNO | MB_ICONQUESTION ) ) == IDNO )
				return;
		}
		memcpy( &fpFonts[GRAPHICS].lf, &lfTemp, sizeof( LOGFONT ) );
		if( (hfTemp = CreateFontIndirect( &fpFonts[GRAPHICS].lf ))!=NULL )
		{
			if( fpFonts[GRAPHICS].hfHandle )
			{
				DeleteObject( fpFonts[GRAPHICS].hfHandle );
				fpFonts[GRAPHICS].hfHandle = hfTemp;
				WriteRegBinary( NULL, ITEM_GRAPHICS_FONT, (unsigned char *)&fpFonts[GRAPHICS].lf, sizeof( LOGFONT ) );
			}
		}
		else
			AfxGetMainWnd()->MessageBox( "Could not load new font!", "WinFrotz", MB_ICONSTOP );
	}
	SetDlgItemText(IDC_FONT_TYPE, fpFonts[GRAPHICS].lf.lfFaceName );
	tmp.Format( "%i", height);
	SetDlgItemText(IDC_FONT_SIZE, tmp);
}

void CGraphOpt::OnOK() 
{
	//LOGFUNCTION
	graphics_screen_size = GetCheckedRadioButton( IDC_SCREEN_640, IDC_SCREEN_1280 );
	graphics_mode = GetCheckedRadioButton( IDC_CGA_MODE, IDC_EGA_MODE );

	if( graphics_screen_size == IDC_SCREEN_640 )
		graphics_screen_size = 0;
	else if( graphics_screen_size == IDC_SCREEN_960 )
		graphics_screen_size = 1;
	else
		graphics_screen_size = 2;

	if( graphics_mode == IDC_CGA_MODE )
		graphics_mode = CGA_MODE;
	else if( graphics_mode == IDC_EGA_MODE )
		graphics_mode = EGA_MODE;
	else
		graphics_mode = MCGA_MODE;

	CDialog::OnOK();

	WriteRegDWORD( NULL, ITEM_GRAPHICS_SCREEN_SIZE, (DWORD)graphics_screen_size );
	WriteRegDWORD( NULL, ITEM_GRAPHICS_MODE, (DWORD)graphics_mode );

	if( h_version == 6)
	{
		AfxGetMainWnd()->MessageBox( "Changes will not take effect until you restart your game\n", "WinFrotz", MB_ICONEXCLAMATION );
	}
}
