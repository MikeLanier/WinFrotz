// CScrollBack.cpp : implementation file
//

//*--------------------------------------------------------------------------
//| CScrollBack is basically a simple edit box window that is created full
//| size (covers entire underlying window) and is automatically positioned
//| at the end of input text. This is used in WinFrotz to cut review/copy 
//| text that has gone to the primary window (win 0 in Z-Machine terms).
//*--------------------------------------------------------------------------

#include "../include/stdafx.h"
#include "../include/winfrotz.h"
#include "../include/CScrollBack.h"
#include "../include/windefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C"	fontprops	fpFonts[];
extern "C"	char	szBigBuffer[];
extern "C"	int		bbpos;
extern "C"	int		start_buffer_point;
extern "C"	HWND	hWnd;

/////////////////////////////////////////////////////////////////////////////
// CScrollBack dialog


CScrollBack::CScrollBack(CWnd* pParent /*=NULL*/)
	: CDialog(CScrollBack::IDD, pParent)
{
	//LOGFUNCTION
	//{{AFX_DATA_INIT(CScrollBack)
	//}}AFX_DATA_INIT
	pEditBox = NULL;
	stdFont = NULL;
}


void CScrollBack::DoDataExchange(CDataExchange* pDX)
{
	//LOGFUNCTION
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScrollBack)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScrollBack, CDialog)
	//{{AFX_MSG_MAP(CScrollBack)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrollBack message handlers

BOOL CScrollBack::OnInitDialog() 
{
	//LOGFUNCTION
	CDialog::OnInitDialog();

	pEditBox = (CEdit *)GetDlgItem( IDC_SCROLLEDIT );

	ASSERT( pEditBox );
	
	if( hWnd )
	{
		RECT	rc;

		::GetWindowRect( hWnd, &rc );
		SetWindowPos( &wndTop, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOOWNERZORDER );
	}
	
	stdFont = new CFont;
	stdFont->CreateFontIndirect( &fpFonts[STANDARD].lf );
	
	szBigBuffer[bbpos] = 0;
	if( start_buffer_point )
		szBigBuffer[ start_buffer_point ] =0;
	
	pEditBox->SetFont( stdFont, TRUE );
	pEditBox->SetWindowText( szBigBuffer );
	
	// What the hell? Why doesn't SetSel work here? I have to force this keyboard
	// kludge to make the dialog scroll all the way to the last line. SetSel always
	// positions on the NEXT to last line.

	//pEditBox->SetSel( -1, 0, FALSE);
	pEditBox->PostMessage( WM_KEYDOWN, (WPARAM)VK_END, (LPARAM)21954561L );
	pEditBox->PostMessage( WM_KEYUP, (WPARAM)VK_END, (LPARAM)-1051787263L );

	pEditBox->PostMessage( WM_VSCROLL, (WPARAM)SB_BOTTOM, (LPARAM)0L );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScrollBack::OnSize(UINT nType, int cx, int cy) 
{
	//LOGFUNCTION
	CDialog::OnSize(nType, cx, cy);
	
	//This assures that the edit box always fills the entire area of
	//the dialog box in general
	if( pEditBox )
	{
		pEditBox->SetWindowPos( &wndTop, 0, 0, 
			cx , cy , 
			SWP_NOMOVE );
	}
}

BOOL CScrollBack::DestroyWindow() 
{
	//LOGFUNCTION
	//More properly put in the destructor but I wanted to catch the
	//pEditBox pointer here.
	pEditBox = NULL;
	delete stdFont;
	stdFont = NULL;
	return CDialog::DestroyWindow();
}
