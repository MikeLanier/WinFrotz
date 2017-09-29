// CDebugOpt.cpp : implementation file
//

//*--------------------------------------------------------------------------
//| DebugOpt is a simple dialog box displaying a list of checkboxes for the
//| various supported debug options. As I often do I basically cheat and 
//| check and set options in the OnOK handler. This method works fine as long
//| as you are dealing in global variables (which I am, as they originate 
//| on the C side and I have no choice). You're supposed to use DDX instead of
//| of this method of course.
//*--------------------------------------------------------------------------

#include "../include/stdafx.h"
#include "../include/winfrotz.h"
#include "../include/CDebugOpt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" int option_attribute_assignment;
extern "C" int option_attribute_testing;
extern "C" int option_object_movement;
extern "C" int option_object_locating;
extern "C" int option_piracy;
extern "C" int user_tandy_bit;

/////////////////////////////////////////////////////////////////////////////
// CDebugOpt dialog


CDebugOpt::CDebugOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CDebugOpt::IDD, pParent)
{
	//LOGFUNCTION
	//{{AFX_DATA_INIT(CDebugOpt)
	m_Assignment = FALSE;
	m_Locating = FALSE;
	m_Movement = FALSE;
	m_Testing = FALSE;
	m_Pirate = FALSE;
	m_Tandy = FALSE;
	//}}AFX_DATA_INIT
}


void CDebugOpt::DoDataExchange(CDataExchange* pDX)
{
	//LOGFUNCTION
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDebugOpt)
	DDX_Check(pDX, IDC_ASSIGN_YES, m_Assignment);
	DDX_Check(pDX, IDC_LOCATING_YES, m_Locating);
	DDX_Check(pDX, IDC_MOVEMENT_YES, m_Movement);
	DDX_Check(pDX, IDC_TESTING_YES, m_Testing);
	DDX_Check(pDX, IDC_PIRACY, m_Pirate);
	DDX_Check(pDX, IDC_TANDY, m_Tandy);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDebugOpt, CDialog)
	//{{AFX_MSG_MAP(CDebugOpt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDebugOpt message handlers

BOOL CDebugOpt::OnInitDialog() 
{
	//LOGFUNCTION
	m_Assignment = option_attribute_assignment;
	m_Locating = option_object_locating;
	m_Movement = option_object_movement;
	m_Testing = option_attribute_testing;
	m_Pirate = option_piracy;
	m_Tandy = user_tandy_bit;
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDebugOpt::OnOK() 
{
	//LOGFUNCTION
	if( GetCheckedRadioButton( IDC_ASSIGN_YES, IDC_ASSIGN_YES ))
		option_attribute_assignment = 1;
	else
		option_attribute_assignment = 0;

	if( GetCheckedRadioButton( IDC_TESTING_YES, IDC_TESTING_YES ))
		option_attribute_testing = 1;
	else
		option_attribute_testing = 0;

	if( GetCheckedRadioButton( IDC_MOVEMENT_YES, IDC_MOVEMENT_YES ))
		option_object_movement = 1;
	else
		option_object_movement = 0;

	if( GetCheckedRadioButton( IDC_LOCATING_YES, IDC_LOCATING_YES ))
		option_object_locating = 1;
	else
		option_object_locating = 0;

	if( GetCheckedRadioButton( IDC_PIRACY, IDC_PIRACY ))
		option_piracy = 1;
	else
		option_piracy = 0;

	if( GetCheckedRadioButton( IDC_TANDY, IDC_TANDY ))
		user_tandy_bit = 1;
	else
		user_tandy_bit = -1;

	CDialog::OnOK();
}
