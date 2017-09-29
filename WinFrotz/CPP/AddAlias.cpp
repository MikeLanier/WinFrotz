// AddAlias.cpp : implementation file
//

//*--------------------------------------------------------------------------
//| AddAlias is a simple dialog box displaying two edit boxes, one for the 
//| text to replace and one for the text that will replace it. Not much work
//| is done here; it's all done in the calling function which receives the 
//| edit fields through the member variables m_find and m_replace
//*--------------------------------------------------------------------------

#include "../include/stdafx.h"
#include "../include/winfrotz.h"
#include "../include/AddAlias.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AddAlias dialog


AddAlias::AddAlias(CWnd* pParent /*=NULL*/)
	: CDialog(AddAlias::IDD, pParent)
{
	//LOGFUNCTION
	//{{AFX_DATA_INIT(AddAlias)
	m_find = _T("");
	m_replace = _T("");
	//}}AFX_DATA_INIT
}


void AddAlias::DoDataExchange(CDataExchange* pDX)
{
	//LOGFUNCTION
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddAlias)
	DDX_Text(pDX, IDC_ALIAS_EDIT1, m_find);
	DDX_Text(pDX, IDC_ALIAS_EDIT2, m_replace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddAlias, CDialog)
	//{{AFX_MSG_MAP(AddAlias)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddAlias message handlers
