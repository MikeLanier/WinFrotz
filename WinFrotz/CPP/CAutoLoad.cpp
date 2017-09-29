// CAutoLoad.cpp : implementation file
//

#include "../include/stdafx.h"
#include "../include/WinFrotz.h"
#include "../include/CAutoLoad.h"
#include "../include/Windefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char	szInitFile[ ];
void WriteRegBinary( HKEY hkInput, char *item, unsigned char *data, int size );

/////////////////////////////////////////////////////////////////////////////
// CAutoLoad dialog


CAutoLoad::CAutoLoad(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoLoad::IDD, pParent)
{
	//LOGFUNCTION
	//{{AFX_DATA_INIT(CAutoLoad)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAutoLoad::DoDataExchange(CDataExchange* pDX)
{
	//LOGFUNCTION
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoLoad)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoLoad, CDialog)
	//{{AFX_MSG_MAP(CAutoLoad)
	ON_BN_CLICKED(IDC_AUTO_OVERRIDE, OnAutoOverride)
	ON_BN_CLICKED(IDC_AUTO_BUTTON, OnAutoButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoLoad message handlers

BOOL CAutoLoad::OnInitDialog() 
{
	//LOGFUNCTION
	CDialog::OnInitDialog();

	SetDlgItemText( IDC_AUTO_EDIT, szInitFile );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoLoad::OnAutoOverride() 
{
	//LOGFUNCTION
	CButton *pButton = (CButton *)GetDlgItem( IDC_AUTO_OVERRIDE );
	if( !pButton )
		return;

	if( pButton->GetCheck() )
		SetDlgItemText( IDC_AUTO_EDIT, "None" );
}

void CAutoLoad::OnOK() 
{
	//LOGFUNCTION
	char	szTemp[ MAX_PATH ];

	GetDlgItemText( IDC_AUTO_EDIT, szTemp, MAX_PATH );
	if( strcmp( szTemp, szInitFile ) )
	{
		WriteRegBinary( NULL, ITEM_INITIALFILE, (unsigned char *)szTemp, MAX_PATH );		
		strcpy( szInitFile, szTemp );
	}

	CDialog::OnOK();
}

void CAutoLoad::OnAutoButton() 
{
	//LOGFUNCTION
	static char BASED_CODE szFilter[] = "ZMachine stories (*.Z?)|*.Z?|ZMachine data files (*.dat)|*.dat|ZMachine color stories (*.zip)|*.zip|All Files (*.*)|*.*||";
	CString	name;
	CFileDialog	cfFileName( TRUE, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST, szFilter, this );

	if( cfFileName.DoModal() == IDOK )
	{
		name = cfFileName.GetPathName();
		SetDlgItemText( IDC_AUTO_EDIT, name.GetBuffer(0) );
	}
}
