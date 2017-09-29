// AliasList.cpp : implementation file
//

//*--------------------------------------------------------------------------
//| AliasList is a dialog box that manages and displays the aliases in use. 
//| All work regarding actual insertion/deletion etc is done right here, it is
//| a self contained class. The original data is preserved so if the user
//| selects "cancel" we can put everything back in place
//*--------------------------------------------------------------------------

#include "../include/stdafx.h"
#include "../include/winfrotz.h"
#include "../include/AliasList.h"
#include "../include/AddAlias.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CListBox	*pBox;


//1024 = ALIAS_SIZE from alias.c
char	localalias[1024];
char	*localalias_end;

//Variables and procedures we need to references from alias.c. Since the alias
//data itself is global this can be shared between threads without problems.
extern "C"	int	use_aliases;
extern "C"	char aliases[];
extern "C"	char *alias_end;
extern "C"	void delete_alias(const char* alias);
extern "C"	void add_alias(const char* alias, const char* expansion);

/////////////////////////////////////////////////////////////////////////////
// CAliasList dialog

CAliasList::CAliasList(CWnd* pParent /*=NULL*/)
	: CDialog(CAliasList::IDD, pParent)
{
	//LOGFUNCTION
	//{{AFX_DATA_INIT(CAliasList)
	//}}AFX_DATA_INIT
}

//There's no need to do this work through data exchange - we're operating on 
//globals and all of the logic can be contained in the dialog box itself.
void CAliasList::DoDataExchange(CDataExchange* pDX)
{
	//LOGFUNCTION
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAliasList)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAliasList, CDialog)
	//{{AFX_MSG_MAP(CAliasList)
	ON_BN_CLICKED(IDC_ALIAS_ENABLE, OnAliasEnable)
	ON_BN_CLICKED(IDC_ALIAS_DISABLE, OnAliasDisable)
	ON_BN_CLICKED(IDC_ALIAS_DELETE_ALL, OnAliasDeleteAll)
	ON_BN_CLICKED(IDC_ALIAS_DELETE, OnAliasDelete)
	ON_BN_CLICKED(IDC_ALIAS_NEW, OnAliasNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAliasList message handlers

//Clear, then insert all the aliases into the listbox of the dialog
void ShowAliases( void )
{
	//LOGFUNCTION
	char	*c, szAliasText[80];

	ASSERT( pBox );
	pBox->ResetContent();

	c = &aliases[0];
	while( c < alias_end )
	{
		strcpy( szAliasText, c );
		c += strlen( c ) + 1;
		strcat( szAliasText, " -> " );
		strcat( szAliasText, c );
		c += strlen( c ) + 1;
		pBox->AddString( szAliasText );
	}
}

//Initialization etc.
BOOL CAliasList::OnInitDialog()
{
	//LOGFUNCTION
	pBox = (CListBox *)GetDlgItem( IDC_ALIAS_LIST );
	ASSERT( pBox );

	if( use_aliases )
		SetDlgItemText( IDC_ALIAS_STATE, "enabled" );

	memset( &localalias[0], 0, 1024 );
	memcpy( &localalias[0], &aliases[0], 1024 );
	localalias_end = alias_end;

	ShowAliases();
	return TRUE;
}

//Corresponds to the "Enable" dialog button
void CAliasList::OnAliasEnable() 
{
	//LOGFUNCTION
	use_aliases = 1;	
	SetDlgItemText( IDC_ALIAS_STATE, "enabled" );
}

//Corresponds to the "Disable" dialog button
void CAliasList::OnAliasDisable() 
{
	//LOGFUNCTION
	use_aliases = 0;	
	SetDlgItemText( IDC_ALIAS_STATE, "disabled" );
}

//Corresponds to the "Cancel" dialog button
void CAliasList::OnCancel() 
{
	//LOGFUNCTION
	memcpy( aliases, localalias, 1024 );	
	alias_end = localalias_end;
	CDialog::OnCancel();
}

//Corresponds to the "Delete All" dialog button
void CAliasList::OnAliasDeleteAll() 
{
	//LOGFUNCTION
	memset( aliases, 0, 1024 );
	alias_end = aliases;
	use_aliases = 0;
	ShowAliases();
}

//Corresponds to the "Delete" dialog button
void CAliasList::OnAliasDelete() 
{
	//LOGFUNCTION
	int		i;
	char	text[256], *c;
	
	memset( text, 0, 256 );
	i = pBox->GetCurSel();
	if( pBox->GetTextLen( i ) < 256 )
	{
		pBox->GetText( i, &text[0] );
		if( text[0] )
		{
			c = strstr( text, " ->" );
			if( c )
				*c = 0;
			delete_alias( text );
			ShowAliases();
		}
	}
}

//Corresponds to the "New" dialog button. This brings up a different dialog
//(the AddAlias class) which returns the alias/expansion text through member
//variables m_find and m_replace.
void CAliasList::OnAliasNew() 
{
	//LOGFUNCTION
	AddAlias	addDlg( this );
	
	if( addDlg.DoModal() == IDOK )
	{
		if( addDlg.m_find.GetLength() && addDlg.m_replace.GetLength() )
		{
			add_alias( (const char *)addDlg.m_find.GetBuffer(1), (const char *)addDlg.m_replace.GetBuffer(1) );
			ShowAliases();
		}
	}
}
