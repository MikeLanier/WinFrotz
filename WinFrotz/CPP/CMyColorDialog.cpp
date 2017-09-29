// CMyColorDialog.cpp : implementation file
//

//*--------------------------------------------------------------------------
//| MyColorDialog differs only in that the text of the color dialog box menu
//| bar will be set to an appropriate prompt (instead of just "color")
//*--------------------------------------------------------------------------

#include "../include/stdafx.h"
#include "../include/winfrotz.h"
#include "../include/CMyColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyColorDialog

IMPLEMENT_DYNAMIC(CMyColorDialog, CColorDialog)

CMyColorDialog::CMyColorDialog(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd) :
	CColorDialog(clrInit, dwFlags, pParentWnd)
{
	//LOGFUNCTION
		DlgName = "Color";
}


BEGIN_MESSAGE_MAP(CMyColorDialog, CColorDialog)
	//{{AFX_MSG_MAP(CMyColorDialog)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMyColorDialog::SetWindowName(char *name)
{
	//LOGFUNCTION
	DlgName = name;
}

void CMyColorDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	//LOGFUNCTION
	CColorDialog::OnShowWindow(bShow, nStatus);
	SetWindowText( DlgName );
	// TODO: Add your message handler code here
	
}
