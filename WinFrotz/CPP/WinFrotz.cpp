// WinFrotz.cpp : Defines the class behaviors for the application.
//

#include "../include/stdafx.h"
#include "../include/WinFrotz.h"
#include <io.h>
#include "../include/MainFrm.h"
#include "../include/WinFrotzDoc.h"
#include "../include/WinFrotzView.h"
#include <dos.h>
#include <direct.h>
#include "../include/windefs.h"
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	void HandleRegistry( void );
extern char	szInitFile[ ];

extern COLORREF	custColors[17];
extern "C"	fontprops	fpFonts[];
extern "C"	int	zcodeops;
extern "C"  unsigned long zcodetime;
extern "C"	BOOL 	bStoryLoaded;
extern "C"	char icon_name[];
extern "C"	zbyte h_version;


RECT	rcDlgRect;
RECT	rcTextBox = { 91, 38, 304, 57 };
int		curIcon;
int		txtPos;
int		initialwait;
char	line[64];
char	infotext[]="\
A Z-machine emulator for Win32 based on Frotz 2.32 by Stefan Jokisch. \
WinFrotz will run all Infocom games, and can support newer games written \
with the Inform system. WinFrotz is free; if you enjoy it just drop an \
email to the address below, or if you have old Infocom stuff you don't \
want, I'll take it off your hands. A Z-machine emulator for Win32 based \
on Frotz 2.32 by Stefan";
CFont	*dlgFont;
/////////////////////////////////////////////////////////////////////////////
// CWinFrotzApp

BEGIN_MESSAGE_MAP(CWinFrotzApp, CWinApp)
	//{{AFX_MSG_MAP(CWinFrotzApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzApp construction

CWinFrotzApp::CWinFrotzApp()
{
	tLogfile.restart();
	//LOGFUNCTION
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWinFrotzApp object

CWinFrotzApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzApp initialization

BOOL CWinFrotzApp::InitInstance()
{
	//LOGFUNCTION
	int	i;
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWinFrotzDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWinFrotzView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	memset( &custColors, 0, sizeof( COLORREF ) * 16 );
	for( i=0; i < NUM_FONT_TYPES; i++ )
	{
		memset( &fpFonts[i], 0, sizeof( fontprops ) );
		fpFonts[i].nUsagePerm = DISPLAY_FONT;
	}

	//Go and grab all the initial values, and if there are none create a registry entry for them
	HandleRegistry();

	if( szInitFile[0] && strcmp( szInitFile, "None" ) )
	{
		int	result = _open( szInitFile, _O_BINARY | _O_RDONLY );
		if( result != -1 )
		{
			cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
			_close( result );
			cmdInfo.m_strFileName = szInitFile;
		}
	}
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Register the window for file-manager style drag and drop 
	m_pMainWnd->DragAcceptFiles( TRUE );
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//LOGFUNCTION
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	//LOGFUNCTION
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CWinFrotzApp::OnAppAbout()
{
	//LOGFUNCTION
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzApp commands

BOOL CAboutDlg::OnInitDialog()
{
	//LOGFUNCTION
	// CG: Folowing code is added by System Info Component
	CDialog::OnInitDialog();
	CString strFreeDiskSpace;
	CString strFreeMemory;
	CString strFmt;
	CWnd	*DlgText;
	HANDLE	icon;

	DlgText = (CWnd *)GetDlgItem( IDC_TITLETEXT );
	GetWindowRect( &rcDlgRect );
	DlgText->GetWindowRect( &rcTextBox );
	if( bStoryLoaded )
	{
		CStatic* iconDisp = (CStatic *)GetDlgItem( IDC_ABOUT_ICON );
		icon = LoadImage( 0, icon_name, IMAGE_ICON, 0, 0, LR_LOADFROMFILE );
		if( icon )
		{
			curIcon = -1;
			iconDisp->SetIcon( (HICON)icon );
		}
		else
		{
			curIcon = 0;
			SetTimer( IDC_ABOUT_ICON, 500, NULL );
		}

		iconDisp = (CStatic *)GetDlgItem( IDC_GAME_TYPE );
		icon = NULL;
		switch( h_version )
		{
			case	1:
				icon = AfxGetApp()->LoadIcon( IDI_Z1 );
				break;
			case	2:
				icon = AfxGetApp()->LoadIcon( IDI_Z2 );
				break;
			case	3:
				icon = AfxGetApp()->LoadIcon( IDI_Z3 );
				break;
			case	4:
				icon = AfxGetApp()->LoadIcon( IDI_Z4 );
				break;
			case	5:
				icon = AfxGetApp()->LoadIcon( IDI_Z5 );
				break;
			case	6:
				icon = AfxGetApp()->LoadIcon( IDI_Z6 );
				break;
			case	7:
				icon = AfxGetApp()->LoadIcon( IDI_Z7 );
				break;
			case	8:
				icon = AfxGetApp()->LoadIcon( IDI_Z8 );
				break;
			default:
				icon = AfxGetApp()->LoadIcon( IDI_ZUNKNOWN );
				break;
		}
		if( icon )
			iconDisp->SetIcon( (HICON)icon );
	}
	else
	{
		curIcon = 0;
		SetTimer( IDC_ABOUT_ICON, 500, NULL );
	}

	// Fill available memory
	MEMORYSTATUS MemStat;
	MemStat.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&MemStat);
	strFmt.LoadString(CG_IDS_PHYSICAL_MEM1);
	strFreeMemory.Format(strFmt, (MemStat.dwTotalPhys - MemStat.dwAvailPhys) / 1024L);
	
	SetDlgItemText(IDC_PHYSICAL_MEM, strFreeMemory);

#ifdef BUFFERED_TEXTOUT
	SetDlgItemText( IDC_BUFFERED_TEXTOUT, "Buffered ExtTextOut mode is active" );
#else
	SetDlgItemText( IDC_BUFFERED_TEXTOUT, "Buffered ExtTextOut mode is inactive" );
#endif

	// Fill disk free information
	struct _diskfree_t diskfree;
	int nDrive = _getdrive(); // use current default drive
	if (_getdiskfree(nDrive, &diskfree) == 0)
	{
		strFmt.LoadString(CG_IDS_DISK_SPACE1);
		strFreeDiskSpace.Format(strFmt,
			(DWORD)diskfree.avail_clusters *
			(DWORD)diskfree.sectors_per_cluster *
			(DWORD)diskfree.bytes_per_sector / (DWORD)1024L,
			nDrive-1 + _T('A'));
	}
	else
		strFreeDiskSpace.LoadString(CG_IDS_DISK_SPACE_UNAVAIL1);
	
	SetDlgItemText(IDC_DISK_SPACE, strFreeDiskSpace);
	
	CString	strZcodetext;
	strZcodetext.Format( "%d", zcodeops );
	SetDlgItemText(IDC_ZCODE, strZcodetext );
	if( zcodetime )
		strZcodetext.Format( "%.0f Hz", ((float)zcodeops/zcodetime) * 1000 );
	else
		strZcodetext = "???? Hz";
	SetDlgItemText(IDC_ZCODESPEED, strZcodetext );

	dlgFont = new CFont;
	if( dlgFont )
		if( !dlgFont->CreatePointFont( 80, "MS Sans Serif", NULL ) )
			delete dlgFont;
	SetTimer( UPDATE_ABOUT_BOX, 125, NULL );
	txtPos = 0;
	initialwait = 0;

	return TRUE;
}

void CWinFrotzApp::OnFileOpen() 
{
	//LOGFUNCTION
	// prompt the user (with all document templates)
	CString newName;

	CFileDialog	cfdOpen( TRUE, ".dat", NULL, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, "Z-Machine games (*.dat;*.zip;*.z?)|*.dat; *.zip; *.z?|All Files (*.*)|*.*||", NULL );
	if( cfdOpen.DoModal()==IDOK )
	{
		newName = cfdOpen.GetPathName();

		OpenDocumentFile(newName);
	}
}

void CAboutDlg::OnTimer(UINT nIDEvent) 
{
	//LOGFUNCTION
	if( curIcon != -1 && nIDEvent == UPDATE_ABOUT_ICON )
	{
		HANDLE	icon;
		CStatic* iconDisp = (CStatic *)GetDlgItem( IDC_ABOUT_ICON );
		if( ++curIcon > 9 )
			curIcon = 0;
		switch( curIcon )
		{
			case	0:
				icon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
				break;
			case	1:
				icon = AfxGetApp()->LoadIcon( IDI_INFOCOM );
				break;
			case	2:
				icon = AfxGetApp()->LoadIcon( IDI_Z1 );
				break;
			case	3:
				icon = AfxGetApp()->LoadIcon( IDI_Z2 );
				break;
			case	4:
				icon = AfxGetApp()->LoadIcon( IDI_Z3 );
				break;
			case	5:
				icon = AfxGetApp()->LoadIcon( IDI_Z4 );
				break;
			case	6:
				icon = AfxGetApp()->LoadIcon( IDI_Z5 );
				break;
			case	7:
				icon = AfxGetApp()->LoadIcon( IDI_Z6 );
				break;
			case	8:
				icon = AfxGetApp()->LoadIcon( IDI_Z7 );
				break;
			case	9:
				icon = AfxGetApp()->LoadIcon( IDI_Z8 );
				break;
			default:
				icon = AfxGetApp()->LoadIcon( IDI_INFOCOM );
				break;
		}
		if( icon )
			iconDisp->SetIcon( (HICON)icon );
	}
	else if( nIDEvent == UPDATE_ABOUT_BOX )
	{
		CClientDC	dc( this );
		COLORREF	dlgColor = dc.GetPixel( 10, 10 );
		CRect		rect;
		CFont		*oldFont = NULL;

		rect.left = rcTextBox.left - rcDlgRect.left;
		rect.top = rcTextBox.top - rcDlgRect.top;
		rect.right = rect.left + (rcTextBox.right - rcTextBox.left);
		rect.bottom = rect.top + (rcTextBox.bottom - rcTextBox.top);
		if( dlgFont )
			oldFont = dc.SelectObject( dlgFont );
		dc.SetBkColor( dlgColor );
		if( initialwait++ > 5 )
		{
			memset( line, ' ', 64 );
			memcpy( line, &infotext[txtPos++], 60 );
			dc.DrawText( line, 60, rect, DT_LEFT | DT_SINGLELINE);

			if( txtPos >= (int)(strlen( infotext)-60) )
				txtPos = 0;
		}
		else
		{
			dc.DrawText( infotext, 60, rect, DT_LEFT | DT_SINGLELINE );
		}
		if( oldFont )
			dc.SelectObject( oldFont );
	}

	CDialog::OnTimer(nIDEvent);
}

void CAboutDlg::OnOK() 
{
	//LOGFUNCTION
	KillTimer( UPDATE_ABOUT_ICON );
	KillTimer( UPDATE_ABOUT_BOX );
	if( dlgFont )
		delete( dlgFont );
	CDialog::OnOK();
}
