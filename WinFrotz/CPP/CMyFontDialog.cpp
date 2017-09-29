// CMyFontDialog.cpp : implementation file
//

//*--------------------------------------------------------------------------
//| MyFontDialog differs only in that the text of the font dialog box menu
//| bar will be set to an appropriate prompt (instead of just "Font")
//*--------------------------------------------------------------------------

#include "../include/stdafx.h"
#include "../include/winfrotz.h"
#include "../include/CMyFontDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyFontDialog

IMPLEMENT_DYNAMIC(CMyFontDialog, CFontDialog)

CMyFontDialog::CMyFontDialog(LPLOGFONT lplfInitial, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd) : 
	CFontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd)
{
	//LOGFUNCTION
		DlgName = "Font";
}


BEGIN_MESSAGE_MAP(CMyFontDialog, CFontDialog)
	//{{AFX_MSG_MAP(CMyFontDialog)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMyFontDialog::SetWindowName(char *name)
{
	//LOGFUNCTION
	DlgName = name;
}

void CMyFontDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	//LOGFUNCTION
	CFontDialog::OnShowWindow(bShow, nStatus);
	SetWindowText( DlgName );
	// TODO: Add your message handler code here
	
}
