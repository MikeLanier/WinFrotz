// WinFrotzDoc.cpp : implementation of the CWinFrotzDoc class
//

#include "../include/stdafx.h"
#include "../include/WinFrotz.h"

#include "../include/WinFrotzDoc.h"
#include "../include/WinFrotzView.h"
#include "../include/windefs.h"
#include "../include/globs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	BOOL	killZThread( void );
extern	BOOL	startThread( CDC*);

extern "C" BOOL 	bStoryLoaded;
extern "C" BOOL		bStoryInit;
extern "C" const char		*story_name;
extern "C"	int nState;
extern "C"	int		kbBuffer[];
extern "C"	int		kbIndex;
extern "C"	char	kbModals[];
extern "C"	HWND	hWnd;
extern "C"	int	os_process_arguments( int argc, char **argv );
extern "C"	char stripped_story_name[];
extern char	szCurrentStory[];
extern	CTime	StartTime;

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzDoc

IMPLEMENT_DYNCREATE(CWinFrotzDoc, CDocument)

BEGIN_MESSAGE_MAP(CWinFrotzDoc, CDocument)
	//{{AFX_MSG_MAP(CWinFrotzDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzDoc construction/destruction

CWinFrotzDoc::CWinFrotzDoc()
{
	//LOGFUNCTION
}

CWinFrotzDoc::~CWinFrotzDoc()
{
	//LOGFUNCTION
}

BOOL CWinFrotzDoc::OnNewDocument() 
{
	//LOGFUNCTION
	if( bStoryLoaded && bStoryInit )
	{
		if( (AfxGetMainWnd()->MessageBox( "This will end your current game. Are you sure you want to do that?", "WinFrotz", MB_YESNO | MB_ICONQUESTION ))!=IDYES )
		{
			SetTitle( stripped_story_name );
			return FALSE;
		}
		CWinFrotzDoc::OnOpenDocument(szCurrentStory);
	}

	return CDocument::OnNewDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzDoc serialization

void CWinFrotzDoc::Serialize(CArchive& ar)
{
	//LOGFUNCTION
	return;
/*
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
*/
}

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzDoc diagnostics

#ifdef _DEBUG
void CWinFrotzDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWinFrotzDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzDoc commands

BOOL CWinFrotzDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	//LOGFUNCTION
	char	*fakeargv[2];

	if( bStoryLoaded && !bStoryInit )
		return TRUE;
	
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if( bStoryLoaded && strcmp( story_name, lpszPathName) )
	{
		if( (AfxGetMainWnd()->MessageBox(  "This will end your current game. Are you sure you want to do that?", "WinFrotz", MB_YESNO | MB_ICONQUESTION ))!=IDYES )
		{
			SetTitle( stripped_story_name );
			return FALSE;
		}
	}

	strcpy( szCurrentStory, lpszPathName );

	if( hThread )
		killZThread();

	if( szCurrentStory[0] )
	{
		char	tmp[256];
		int i;

		strcpy( tmp, szCurrentStory );
		for( i=strlen( tmp ); i > 0 && tmp[i]!='\\'; i-- );
		tmp[i] = 0;
		SetCurrentDirectory( tmp );
	}
	else
		return FALSE;

	fakeargv[1]= (char *)szCurrentStory;

//	if( !os_process_arguments(2, fakeargv) )
//	{
//		AfxGetMainWnd()->MessageBox( "Couldn't figure out the command line!", "WinFrotz", MB_ICONSTOP );
//		return FALSE;
//	}
//	else
//	{
		os_process_arguments( 2, fakeargv );

		StartTime = CTime::GetCurrentTime();

		bStoryLoaded = TRUE;

		bStoryInit = FALSE;

		SetTitle( szCurrentStory );
//	}
	return TRUE;
}

BOOL CWinFrotzDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	//LOGFUNCTION
	//If we're not at an OS_READ prompt, this won't work
	if( nState != OS_READ )
	{
		if( hThread )
			AfxGetMainWnd()->MessageBox( FILE_WARNING, "WinFrotz", MB_ICONSTOP );
		return FALSE;
	}
	else
	{
		//Fill the keyboard buffer with save command and restart Z-Machine
		kbBuffer[0] = 's';
		kbBuffer[1] = 'a';
		kbBuffer[2] = 'v';
		kbBuffer[3] = 'e';
		kbBuffer[4] = 13;
		kbIndex = 5;
		memset( kbModals, 0, 5 );
		DestroyCaret();
		ResumeThread( hThread );
	}
	//return CDocument::OnSaveDocument(lpszPathName);
	return TRUE;
}
