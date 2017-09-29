// MainFrm.cpp : implementation of the CMainFrame class
//

//---------------------------------------------------------------------------
//| MainFrm.cpp : This and the view class are very poorly defined in WinFrotz.
//| This is partially because of the necessity of supporting external C 
//| functions/variables, and partially because I am too lazy to do it right.
//---------------------------------------------------------------------------
#include "../include/stdafx.h"
#include "../include/WinFrotz.h"
#include "../include/WinFrotzDoc.h"
#include "../include/DisplayOpt.h"
#include "../include/GraphOpt.h"
#include "../include/MainFrm.h"
#include "../include/AliasList.h"
#include "../include/CDebugOpt.h"
#include "../include/CSoundOpt.h"
#include "../include/CScrollBack.h"
#include "../include/CAutoLoad.h"

#include "../include/globs.h"
#include "../include/states.h"
#include "../include/windefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL	killZThread( void );

extern "C"	zbyte h_version;
extern "C"	int nState;
extern "C"	fontprops	fpFonts[];
extern "C"	int		kbBuffer[];
extern "C"	int		kbIndex;
extern "C"	char	kbModals[];
extern "C"	char	init_name[];
extern "C"	zword	h_screen_width;
extern "C"	zword	h_screen_height;
extern "C"	BOOL 	bStoryLoaded;
extern "C"	frotzwin wp[];
extern "C"	char aliases[];
extern "C"	char *alias_end;
extern "C"	int	cTranslate[];
extern "C"	int	status_last_char;
extern "C"	int os_read_file_name (char *, char *, int);
extern "C"	void save_aliases(const char* filename);
extern "C"	int load_aliases(const char* filename);

extern "C" char szBigBuffer[];
extern "C" int bbpos;
extern "C" char *ScrollRoll[];
extern "C" int	nScrollBuffs;
extern "C" char stripped_story_name[];

extern void WriteRegDWORD( HKEY hkInput, char *item, DWORD value);
extern DWORD ReadRegDWORD( HKEY hkInput, char *item, BOOL bShowError );
extern	int	bResize;
extern	CSize	csStartSize;
CTime	StartTime;
extern unsigned long	misc_flags;

//nLockXSize and nLockYSize hold screen dimensions if the user has locked
//the screen down (doesn't want to have it accidentally resized)
long	nLockXSize, nLockYSize;

//Just shows whether the app is in the process of closing or not. This is
//used for players exiting who say they want to save the game (complicated,
//because that has to be done in the other thread)
BOOL	bClosing;

//nMinX and nMinY hold the smallest sizes we will allow based on the
//currently selected fonts. nMinYMod is the total current overhead for the
//Y-axis - the toolbar space, status bar, windows metrics, etc summed up	*/
int			nMinX, nMinY;
int			nMinYMod;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
//	ON_WM_QUERYNEWPALETTE()
//	ON_WM_PALETTECHANGED()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TIME, OnUpdateTime)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_FONT, OnUpdateViewFont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_FONT, OnUpdateViewStatusFont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_COMMAND(ID_VIEW_DISPLAY_OPT, OnViewDisplayOpt)
	ON_UPDATE_COMMAND_UI(ID_COLORS_USERINVBG, OnUpdateColorsUserinvbg)
	ON_UPDATE_COMMAND_UI(ID_COLORS_USERINVFG, OnUpdateColorsUserinvfg)
	ON_COMMAND(ID_OPTIONS_ALIAS, OnAliasManage)
	ON_COMMAND(ID_FILE_SAVE_ALIAS, OnFileSaveAlias)
	ON_COMMAND(ID_FILE_GAME, OnFileGame)
	ON_COMMAND(ID_FILE_ALIAS, OnFileAlias)
	ON_UPDATE_COMMAND_UI(ID_FILE_ALIAS, OnUpdateFileAlias)
	ON_UPDATE_COMMAND_UI(ID_FILE_GAME, OnUpdateFileGame)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ALIAS, OnUpdateFileSaveAlias)
	ON_COMMAND(ID_FILE_REC, OnFileRec)
	ON_UPDATE_COMMAND_UI(ID_FILE_REC, OnUpdateFileRec)
	ON_COMMAND(ID_FILE_SAVE_REC, OnFileSaveRec)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_REC, OnUpdateFileSaveRec)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_VIEW_GRAPHICS, OnViewGraphics)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_OPTIONS_DEBUGGING, OnOptionsDebugging)
	ON_COMMAND(ID_OPTIONS_SOUND, OnOptionsSound)
	ON_COMMAND(ID_VIEW_SCROLLBACK, OnViewScrollback)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE_TRANSCRIPT, OnFileSaveTranscript)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_TRANSCRIPT, OnUpdateFileSaveTranscript)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_OPTIONS_AUTO_LOAD, OnOptionsAutoLoad)
	ON_MESSAGE( WM_SET_ICON_TYPE, OnSetIconType )
	ON_COMMAND(ID_OPTIONS_UNDO_GAME, OnOptionsUndoGame)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_DIM,		// character dimensions
	ID_INDICATOR_LOCK,		// screen lock indicator
	ID_INDICATOR_INS,		// insert/overstrike
	ID_INDICATOR_CAPS,		// CAPS key
	ID_INDICATOR_NUM,		// NUMLOCK
	ID_INDICATOR_SCRL,		// Scroll lock
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	//LOGFUNCTION
	StartTime = CTime::GetCurrentTime();

	//cSpecialKey holds a map that determines if a key sequence requires unusual
	//handling. Almost all of this could be gotten around with regular WM_CHAR
	//parsing...but not the Alt keys that people expect to work from Frotz. 
	memset( &cSpecialKey[0], 0, 256 );
	
	cSpecialKey[13] = (unsigned char)255;
	cSpecialKey[8] = VALID_KEY | CTRL_KEY_PRESSED | SHIFT_KEY_PRESSED;
	cSpecialKey[9] = VALID_KEY;
	cSpecialKey[27] = VALID_KEY | CTRL_KEY_PRESSED;		
	cSpecialKey[37] = VALID_KEY | CTRL_KEY_PRESSED;		// left arrow
	cSpecialKey[38] = VALID_KEY | CTRL_KEY_PRESSED;		// up arrow
	cSpecialKey[39] = VALID_KEY | CTRL_KEY_PRESSED;		// right arrow
	cSpecialKey[40] = VALID_KEY | CTRL_KEY_PRESSED;		// down arrow
	cSpecialKey[33] = VALID_KEY | CTRL_KEY_PRESSED;		// pgup
	cSpecialKey[34] = VALID_KEY | CTRL_KEY_PRESSED;		// pgdown
	cSpecialKey[35] = VALID_KEY;						// end
	cSpecialKey[36] = VALID_KEY;						// home
	cSpecialKey[45] = VALID_KEY;						// insert
	cSpecialKey[46] = VALID_KEY | CTRL_KEY_PRESSED;		// delete

	cSpecialKey['A'] |= ALT_KEY_PRESSED;
	cSpecialKey['D'] |= ALT_KEY_PRESSED;
	cSpecialKey['N'] |= ALT_KEY_PRESSED;
	cSpecialKey['P'] |= ALT_KEY_PRESSED;
	cSpecialKey['R'] |= ALT_KEY_PRESSED;
	cSpecialKey['S'] |= ALT_KEY_PRESSED;
	cSpecialKey['U'] |= ALT_KEY_PRESSED;
	cSpecialKey['X'] |= ALT_KEY_PRESSED;

	cSpecialKey['A'] |= CTRL_KEY_PRESSED;
	cSpecialKey['T'] |= CTRL_KEY_PRESSED;
	cSpecialKey['B'] |= CTRL_KEY_PRESSED;
	cSpecialKey['D'] |= CTRL_KEY_PRESSED;
	cSpecialKey['E'] |= CTRL_KEY_PRESSED;
	cSpecialKey['F'] |= CTRL_KEY_PRESSED;
	cSpecialKey['H'] |= CTRL_KEY_PRESSED;
	cSpecialKey['N'] |= CTRL_KEY_PRESSED;
	cSpecialKey['P'] |= CTRL_KEY_PRESSED;
	cSpecialKey['T'] |= CTRL_KEY_PRESSED;
	cSpecialKey['U'] |= CTRL_KEY_PRESSED;

	bClosing = FALSE;

	nLockXSize = csStartSize.cx;
	nLockYSize = csStartSize.cy;

	//Can't have a locked screen if we don't have sizes 
	if( !bResize && (!nLockXSize || !nLockYSize) )
		bResize = 1;

	bStoryLoaded = FALSE;

	//This is a guess. It could be wrong if ToolTip fonts are changed. We'll straighten it out
	//later if so.
	nStatusBar = 18;

	nMinX = 0;
	nMinY = 0;
	nMinYMod = nStatusBar + nToolBar +
		GetSystemMetrics( SM_CYSIZEFRAME )*2 +
		GetSystemMetrics( SM_CYCAPTION ) +
		GetSystemMetrics( SM_CYMENU ) +
		GetSystemMetrics( SM_CYEDGE ) * 2;
}

CMainFrame::~CMainFrame()
{
	//LOGFUNCTION
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//LOGFUNCTION
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS );

	// CG: The following block was inserted by 'Status Bar' component.
	{
		// Find out the size of the static variable 'indicators' defined
		// by AppWizard and copy it
		int nOrigSize = sizeof(indicators) / sizeof(UINT);

		UINT* pIndicators = new UINT[nOrigSize + 2];
		memcpy(pIndicators, indicators, sizeof(indicators));

		// Call the Status Bar Component's status bar creation function
		if (!InitStatusBar(pIndicators, nOrigSize, 1))
		{
			TRACE0("Failed to initialize Status Bar\n");
			return -1;
		}
		delete[] pIndicators;
	}

	if( misc_flags & STATUS_BAR_DISABLED )
		OnViewStatusBar();
	if( misc_flags & TOOL_BAR_DISABLED )
		OnViewToolbar();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//LOGFUNCTION
	CWindowDC	dc(NULL);
	HGDIOBJ		oldFont;
	int			startval, i;
	SIZE		tmpSize;
	char		c[2];
	DWORD		startVal;

	//Load the initial default fonts here because they will determine the default size
	//of the window itself. Each font has a pre-computed size map stored in nWidths[]
	//for all characters (makes building output rectangles a hell of a lot faster)
	c[1]=0;
	if( ! fpFonts[STANDARD].hfHandle )
		AfxGetMainWnd()->MessageBox( "Warning: Font load failure", "WinFrotz", MB_OK );
	oldFont = SelectObject( dc, fpFonts[STANDARD].hfHandle );
	for( i=0; i < 256; i++ )
	{
		c[0] = (char)i;
		GetTextExtentPoint32( dc, &c[0], 1, &tmpSize);
		fpFonts[STANDARD].nWidths[i] = tmpSize.cx;
	}
	SelectObject( dc, oldFont );
	fpFonts[STANDARD].nHeight = tmpSize.cy;
	::GetObject( fpFonts[STANDARD].hfHandle, sizeof( LOGFONT ), &fpFonts[STANDARD].lf);

	oldFont = SelectObject( dc, fpFonts[FIXED].hfHandle );

	for( i=0; i < 256; i++ )
	{
		c[0] = (char)i;
		GetTextExtentPoint32( dc, &c[0], 1, &tmpSize);
		fpFonts[FIXED].nWidths[i] = tmpSize.cx;
	}
	SelectObject( dc, oldFont );
	fpFonts[FIXED].nHeight = tmpSize.cy;
	::GetObject( fpFonts[FIXED].hfHandle, sizeof( LOGFONT ), &fpFonts[FIXED].lf);

	//Start us off with a display that is START_VERT_CHARS Fixed chars wide 
	//and START_HORZ_CHARS display chars tall

	startval = fpFonts[STANDARD].nHeight * START_VERT_CHARS;

	if( csStartSize.cy )
		cs.cy = nMinY = csStartSize.cy;
	else
		csStartSize.cy = cs.cy = nMinY = startval + nMinYMod;

	//Use the standard Windows convention of 'X' being the largest width character
	startval = fpFonts[FIXED].nWidths['X'] * START_HORZ_CHARS;

	if( csStartSize.cx )
		cs.cx = nMinX = csStartSize.cx;
	else
		csStartSize.cx = cs.cx = nMinX = startval + 
			GetSystemMetrics( SM_CXEDGE )*2 + 
			GetSystemMetrics( SM_CXSIZEFRAME )*2;

	startVal = ReadRegDWORD( NULL, ITEM_XPOS, FALSE );
	if( startVal != READ_REG_FAIL )
		cs.x = startVal;

	startVal = ReadRegDWORD( NULL, ITEM_YPOS, FALSE );
	if( startVal != READ_REG_FAIL )
		cs.y = startVal;

	cs.style |= FWS_ADDTOTITLE;
	cs.style |= CS_CLASSDC;
	cs.style |= CS_PARENTDC;
	cs.style |= CS_BYTEALIGNCLIENT;
	//cs.style |= CS_BYTEALIGNWINDOW;
	cs.style |= CS_OWNDC;
	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	//LOGFUNCTION
	RECT rcDesktop, rcWindow;
	CWnd	*lpcWndDesktop = GetDesktopWindow();

	// Guarantee that the minimum size of the window is the smallest rect that will hold
	// MIN_HORZ_CHARS fixed width characters wide and MIN_VERT_CHARS display font characters high

	lpcWndDesktop->GetClientRect( &rcDesktop );
	GetWindowRect( &rcWindow );
	if( bResize )
	{
		//Here's our chance to catch changes on the fly to any of these params
		nMinYMod = nStatusBar + nToolBar +
			GetSystemMetrics( SM_CYSIZEFRAME )*2 +
			GetSystemMetrics( SM_CYCAPTION ) +
			GetSystemMetrics( SM_CYMENU ) +
			GetSystemMetrics( SM_CYEDGE ) * 2;		
		
		if( fpFonts[FIXED].hfHandle )
			nMinX = fpFonts[FIXED].nWidths['X'] * MIN_HORZ_CHARS ;
		else
			nMinX = 8 * MIN_HORZ_CHARS;

		//If there is an active upper window we need to preserve it's x-size, since none
		//of those games understand resizing on the fly (eg, Seastalker and AMFV )
		if( status_last_char && h_version > 3 )
			nMinX = status_last_char * fpFonts[FIXED].nWidths['X'];
		nMinY = fpFonts[FIXED].nHeight * MIN_VERT_CHARS + nMinYMod;

		if( !fpFonts[FIXED].hfHandle )
			nMinY = 12 * MIN_VERT_CHARS;

		lpMMI->ptMaxSize.x = min( GetSystemMetrics( SM_CXFULLSCREEN ), min( fpFonts[STANDARD].nWidths['X'], fpFonts[FIXED].nWidths['X'])*255 );
		lpMMI->ptMaxSize.y = GetSystemMetrics( SM_CYFULLSCREEN );
		lpMMI->ptMaxPosition.x = rcDesktop.left;
		lpMMI->ptMaxPosition.y = rcDesktop.top;
		lpMMI->ptMinTrackSize.x = nMinX + GetSystemMetrics( SM_CXEDGE )*2 + GetSystemMetrics( SM_CXSIZEFRAME )*2;
		lpMMI->ptMinTrackSize.y = nMinY;
		lpMMI->ptMaxTrackSize.x = GetSystemMetrics( SM_CXMAXTRACK );
		lpMMI->ptMaxTrackSize.y = GetSystemMetrics( SM_CYMAXTRACK );
		CFrameWnd::OnGetMinMaxInfo(lpMMI);
	}
	else
	{
		lpMMI->ptMaxSize.x = nLockXSize;
		lpMMI->ptMaxSize.y = nLockYSize;
		lpMMI->ptMaxPosition.x = rcDesktop.left;
		lpMMI->ptMaxPosition.y = rcDesktop.top;
		lpMMI->ptMinTrackSize.x = lpMMI->ptMaxSize.x;
		lpMMI->ptMinTrackSize.y = lpMMI->ptMaxSize.y;
		lpMMI->ptMaxTrackSize.x = lpMMI->ptMaxSize.x;
		lpMMI->ptMaxTrackSize.y = lpMMI->ptMaxSize.y;
	}

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	//LOGFUNCTION
	//Status bar needs updating with a different layout because it has dimensions in it
	UpdateStatusBar();
	CFrameWnd::RecalcLayout(bNotify);
}

void CMainFrame::UpdateStatusBar( void )
{
	//LOGFUNCTION
	char	dimen[12];
	int		nTemp;
	RECT	rcWindow;

	nTemp= wp[1].y_size + wp[7].y_size;
	GetWindowRect( &rcWindow );
	if( h_version == 6 )
		sprintf( dimen, "%dx%d", h_screen_width, h_screen_height  );
	else
		sprintf( dimen, "%dx%d", (rcWindow.right - rcWindow.left - GetSystemMetrics( SM_CXEDGE )*2 - GetSystemMetrics( SM_CXSIZEFRAME )*2) / fpFonts[STANDARD].nWidths['X'], (rcWindow.bottom - rcWindow.top - nMinYMod - nTemp) / fpFonts[STANDARD].nHeight + 1 );
	SetStatusPane( ID_INDICATOR_DIM, dimen );
}

void CMainFrame::OnUpdateTime(CCmdUI* pCmdUI)
{
	////LOGFUNCTION
	//Show the elapsed time playing this game
	CString strTime = " 0:00:00";

	if( hThread )
	{
		CTimeSpan timespent = CTime::GetCurrentTime() - StartTime;
		strTime = timespent.Format(_T("%H:%M:%S"));
		if( strTime.GetAt( 0 )=='0' )
			strTime.SetAt( 0, ' ');
	}
	// BLOCK: compute the width of the date string
	CSize size;
	{
		HGDIOBJ hOldFont = NULL;
		HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
		CClientDC dc(NULL);
		if (hFont != NULL) 
			hOldFont = dc.SelectObject(hFont);
		size = dc.GetTextExtent(strTime);
		if (hOldFont != NULL) 
			dc.SelectObject(hOldFont);
	}

	// Update the pane to reflect the current time spent
	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(m_nTimePaneNo, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(m_nTimePaneNo, nID, nStyle, size.cx);
	pCmdUI->SetText(strTime);
	pCmdUI->Enable(TRUE);

}

BOOL CMainFrame::InitStatusBar(UINT *pIndicators, int nSize, int nSeconds)
{
	//LOGFUNCTION
	// CG: This function was inserted by 'Status Bar' component.

	// Create an index for the TIME pane
	m_nTimePaneNo = nSize++;
	if( !nSeconds )
		nSeconds = 30;
	pIndicators[m_nTimePaneNo] = ID_INDICATOR_TIME;

	m_wndStatusBar.SetTimer(0x1000, nSeconds * 1000, NULL);

	return m_wndStatusBar.SetIndicators(pIndicators, nSize);
}

void CMainFrame::OnClose() 
{
	//LOGFUNCTION
	RECT	rc;

	killZThread();

	//If the user has changed size, save out the new dimensions for starting
	//values next time.
	GetWindowRect( &rc );
	CSize	csTmp( rc.right - rc.left, rc.bottom - rc.top );
	if( csTmp != csStartSize )
	{
		WriteRegDWORD( NULL, ITEM_XSIZE, (DWORD)csTmp.cx );
		WriteRegDWORD( NULL, ITEM_YSIZE, (DWORD)csTmp.cy );
	}
	//Save out position of window
	WriteRegDWORD( NULL, ITEM_XPOS, (DWORD)rc.left );
	WriteRegDWORD( NULL, ITEM_YPOS, (DWORD)rc.top );
	CFrameWnd::OnClose();
}

void CMainFrame::OnViewStatusBar() 
{
	//LOGFUNCTION
	RECT	rcTemp;

	m_wndStatusBar.ShowWindow((m_wndStatusBar.GetStyle() & WS_VISIBLE) == 0);

	if( !nStatusBar )
	{
		m_wndStatusBar.GetWindowRect( &rcTemp );
		nStatusBar = rcTemp.bottom - rcTemp.top;
		nMinYMod += nStatusBar;
		misc_flags &= ~STATUS_BAR_DISABLED;
	}
	else
	{
		nMinYMod -= nStatusBar;
		nStatusBar = 0;
		misc_flags |= STATUS_BAR_DISABLED;
	}

	WriteRegDWORD( NULL, ITEM_MISC_FLAGS, (DWORD)misc_flags );
	//This will update the dimensions box as well
	RecalcLayout();
}

void CMainFrame::OnViewToolbar() 
{
	//LOGFUNCTION
	RECT	rcTemp;

	m_wndToolBar.ShowWindow((m_wndToolBar.GetStyle() & WS_VISIBLE) == 0);

	if( !nToolBar )
	{
		m_wndToolBar.GetWindowRect( &rcTemp );
		nToolBar = rcTemp.bottom - rcTemp.top;
		nMinYMod += nToolBar;
		misc_flags &= ~TOOL_BAR_DISABLED;
	}
	else
	{
		nMinYMod -= nToolBar;
		nToolBar = 0;
		misc_flags |= TOOL_BAR_DISABLED;
	}
	
	WriteRegDWORD( NULL, ITEM_MISC_FLAGS, (DWORD)misc_flags );
	RecalcLayout();
}

void CMainFrame::ChangeStatus( unsigned long overwrite )
{
	//LOGFUNCTION
	if( overwrite )
		SetStatusPane( ID_INDICATOR_INS, "OVR" );
	else
		SetStatusPane( ID_INDICATOR_INS, "INS " );

	if( !bResize )
		SetStatusPane( ID_INDICATOR_LOCK, "LOCKED" );
	else
		SetStatusPane( ID_INDICATOR_LOCK, "UNLOCK" );
}

void CMainFrame::SetStatusPane( UINT uID, LPSTR text )
{
	//LOGFUNCTION
	if( nStatusBar )
	{
		int	nIndex;
		int nTemp;
		
		nIndex = m_wndStatusBar.CommandToIndex( uID );
		if( nIndex != -1 )
		{
			HGDIOBJ hOldFont = NULL;
			UINT nID, nStyle;
			CSize	size;
			HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
			CClientDC dc(NULL);
			
			if( hFont ) 
				hOldFont = dc.SelectObject( hFont );
			size = dc.GetTextExtent( text );
			if (hOldFont != NULL) 
				dc.SelectObject(hOldFont);
			
			m_wndStatusBar.GetPaneInfo(nIndex, nID, nStyle, nTemp);
			m_wndStatusBar.SetPaneInfo(nIndex, nID, nStyle, size.cx);
			m_wndStatusBar.SetPaneText( nIndex, text, TRUE );
		}
	}
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	////LOGFUNCTION
	//This is very bad form, but to support Alt keys it was more or less required. Also,
	//it made several things easier, and fixed one bug in VisC (allowing the SetIconType
	//to flow through MFC caused release builds to choke but had no problems under debug)
	switch( pMsg->message )
	{
		case	WM_SYSKEYDOWN:
		{
			if( ! (cSpecialKey[pMsg->wParam] & ALT_KEY_PRESSED ) )
				return CFrameWnd::PreTranslateMessage(pMsg);
			if( kbIndex < KEYBOARD_BUFFER_SIZE )
			{
				kbModals[ kbIndex ] |= ALT_KEY_PRESSED;
				kbBuffer[ kbIndex++ ] = pMsg->wParam;

				if( nState > INIT && nState < CHANGE_FONT )
				{
					DestroyCaret();
					ResumeThread( hThread );
				}
			}
			return TRUE;
			break;
		}

		case	WM_KEYDOWN:
		{
			if( cSpecialKey[pMsg->wParam] )
			{
				if( GetAsyncKeyState( VK_CONTROL ) < 0 )
				{
					if( ! (cSpecialKey[pMsg->wParam] & CTRL_KEY_PRESSED ) )
						return CFrameWnd::PreTranslateMessage(pMsg);
					if( kbIndex < KEYBOARD_BUFFER_SIZE )
					{
						kbModals[ kbIndex ] |= CTRL_KEY_PRESSED;
						kbBuffer[ kbIndex++ ] = pMsg->wParam;
						if( nState > INIT && nState < CHANGE_FONT )
						{
							DestroyCaret();
							ResumeThread( hThread );
						}
					}
					return TRUE;
				}
				else if( cSpecialKey[pMsg->wParam] & VALID_KEY )
				{
					if( kbIndex < KEYBOARD_BUFFER_SIZE )
					{
						kbBuffer[ kbIndex ] = cTranslate[ pMsg->wParam ];
						kbModals[ kbIndex++ ] = 0;
						if( nState > INIT && nState < CHANGE_FONT )
						{
							DestroyCaret();
							ResumeThread( hThread );
						}
					}
					return TRUE;
				}
			}
			return CFrameWnd::PreTranslateMessage(pMsg);
			break;
		}
		
		case	WM_CHAR:
		{
			if( kbIndex < KEYBOARD_BUFFER_SIZE )
			{
				char c;

				c = kbBuffer[ kbIndex ] = (char)pMsg->wParam;
				kbModals[ kbIndex ] = 0;
				if( c < ZC_ASCII_MIN || c > ZC_ASCII_MAX && c < ZC_LATIN1_MIN || c > ZC_LATIN1_MAX )
					kbBuffer[ kbIndex ] = (int)'?';
				kbIndex++;
				if( nState > INIT && nState < CHANGE_FONT )
				{
					DestroyCaret();
					ResumeThread( hThread );
				}
			}
			return TRUE;
		}

		case	WM_CHANGE_STATUS:
		{
			ChangeStatus( (unsigned long)pMsg->wParam );
			return TRUE;
			break;
		}

		case	WM_TIMER:
		{			
			if( pMsg->hwnd == hWnd && (nState == WAIT_FOR_KEY || nState == OS_READ) )
			{
				nState = TIME_ELAPSE;
				DestroyCaret();
				ResumeThread( hThread );
				return TRUE;
			}
			return CFrameWnd::PreTranslateMessage(pMsg);
			break;
		}

		case	WM_BUFFER_READY:
		{
			if( bClosing )
				PostMessage( WM_CLOSE );
			else
				UpdateStatusBar();
			return TRUE;
			break;
		}
		
		case	WM_SET_ICON_TYPE:
		{
			OnSetIconType(0,0);
			return TRUE;
		}

		default:
			return CFrameWnd::PreTranslateMessage(pMsg);
			break;
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnViewDisplayOpt() 
{
	//LOGFUNCTION
	RECT	rcWindow;
	CDisplayOpt	DisplayOpt( this );

	//Put up display options dialog, and handle the immediate consequences of change
	DisplayOpt.DoModal();

	if( !bResize )
	{
		SetStatusPane( ID_INDICATOR_LOCK, "LOCKED" );
		GetWindowRect( &rcWindow );
		nLockXSize = rcWindow.right - rcWindow.left;
		nLockYSize = rcWindow.bottom - rcWindow.top;
	}
	else
		SetStatusPane( ID_INDICATOR_LOCK, "UNLOCK" );
}

void CMainFrame::OnAliasManage() 
{
	//LOGFUNCTION
	CAliasList	AliasList( this );

	AliasList.DoModal();
}

void CMainFrame::OnFileSaveAlias() 
{
	//LOGFUNCTION
	char	file_save[256];

	memset( file_save, 0, 256 );
	if( alias_end == aliases )
	{
		MessageBox( "There are no aliases to save.", "WinFrotz", MB_ICONSTOP );
		return;
	}

	if( os_read_file_name( file_save, init_name, FILE_SAVE_INIT ) )
	{
		strcpy( init_name, file_save );
		save_aliases(init_name);
	}
}

void CMainFrame::OnFileGame() 
{
	//LOGFUNCTION
	//If we're not at an OS_READ prompt, this won't work
	if( nState != OS_READ )
	{
		MessageBox( FILE_WARNING, "WinFrotz", MB_ICONSTOP );
		return;
	}
	else
	{
		//Fill the keyboard buffer with restore command and restart Z-Machine
		kbBuffer[0] = 'r';
		kbBuffer[1] = 'e';
		kbBuffer[2] = 's';
		kbBuffer[3] = 't';
		kbBuffer[4] = 'o';
		kbBuffer[5] = 'r';
		kbBuffer[6] = 'e';
		kbBuffer[7] = 13;
		kbIndex = 8;
		memset( kbModals, 0, kbIndex );
		DestroyCaret();
		ResumeThread( hThread );
	}
}

void CMainFrame::OnFileAlias() 
{
	//LOGFUNCTION
	char	file_load[256];

	if( os_read_file_name( file_load, init_name, FILE_LOAD_INIT ) )
	{
		strcpy( init_name, file_load );
		load_aliases(init_name);
	}
}

void CMainFrame::OnFileRec() 
{
	//LOGFUNCTION
	//If we're not at an OS_READ prompt, this won't work
	if( nState != OS_READ )
	{
		MessageBox( FILE_WARNING, "WinFrotz", MB_ICONSTOP );
		return;
	}
	else
	{
		//Fill the keyboard buffer with save command and restart Z-Machine
		if( kbIndex < KEYBOARD_BUFFER_SIZE )
		{
			kbBuffer[ kbIndex ] = 'P';
			kbModals[ kbIndex++ ] = ALT_KEY_PRESSED;
			DestroyCaret();
			if( nState > INIT && nState < CHANGE_FONT )
				ResumeThread( hThread );
		}
	}
}


void CMainFrame::OnFileSaveRec() 
{
	//LOGFUNCTION
	//If we're not at an OS_READ prompt, this won't work
	if( nState != OS_READ )
	{
		MessageBox( FILE_WARNING, "WinFrotz", MB_ICONSTOP );
		return;
	}
	else
	{
		//Fill the keyboard buffer with save command and restart Z-Machine
		if( kbIndex < KEYBOARD_BUFFER_SIZE )
		{
			kbBuffer[ kbIndex ] = 'R';
			kbModals[ kbIndex++ ] = ALT_KEY_PRESSED;
			DestroyCaret();
			if( nState > INIT && nState < CHANGE_FONT )
				ResumeThread( hThread );
		}
	}
}


void CMainFrame::OnAppExit() 
{
	//LOGFUNCTION
	if( nState == OS_READ )
	{
		int	nResult;
		nResult = MessageBox( "Do you want to save this game?", "WinFrotz", MB_ICONQUESTION | MB_YESNOCANCEL );
		//Saving the game is a pain, because it can only easily be done when the Z-machine is asked to do it.
		//So if the user wants to save we take note of the fact we're closing, send the Z-machine a save 
		//command, and then when the Z-machine is finished it will send a buffer ready message which will 
		//result in the actual close.
		if( nResult == IDYES )
		{
			kbBuffer[0] = 's';
			kbBuffer[1] = 'a';
			kbBuffer[2] = 'v';
			kbBuffer[3] = 'e';
			kbBuffer[4] = 13;
			kbIndex = 5;
			memset( kbModals, 0, 5 );
			bClosing = TRUE;
			DestroyCaret();
			ResumeThread( hThread );
		}
		else if( nResult == IDNO )
			PostMessage( WM_CLOSE );
	}
	else
		PostMessage( WM_CLOSE );
}

void CMainFrame::OnViewGraphics() 
{
	//LOGFUNCTION
	CGraphOpt	GraphOpt( this );

	GraphOpt.DoModal();
}

void CMainFrame::OnEditPaste() 
{
	//LOGFUNCTION
	CEdit	tmpEdit;
	CRect	rc( 0, 0, 200, 20 );
	char	tmpStr[ KEYBOARD_BUFFER_SIZE ];

	tmpEdit.Create( WS_CHILD | ES_LEFT, rc, this, 1 );

	// Check if there is text available
	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		unsigned int i;

		tmpEdit.Paste();
		tmpEdit.GetWindowText( tmpStr, KEYBOARD_BUFFER_SIZE - kbIndex );
		for( i=0; i < strlen( tmpStr ); i++ )
		{
			kbBuffer[ kbIndex ] = (unsigned char)tmpStr[i];
			kbModals[ kbIndex++ ] = 0;
		}
		if( nState > INIT && nState < CHANGE_FONT )
		{
			HideCaret();
			DestroyCaret();
			ResumeThread( hThread );
		}
	}
	else
		MessageBeep(0); // Beep on illegal request
}

void CMainFrame::OnOptionsDebugging() 
{
	//LOGFUNCTION
	CDebugOpt	DebugOpt( this );

	DebugOpt.DoModal();
}

void CMainFrame::OnOptionsSound() 
{
	//LOGFUNCTION
	CSoundOpt	SoundOpt( this );

	SoundOpt.DoModal();
}

void CMainFrame::OnViewScrollback() 
{
	//LOGFUNCTION
	CScrollBack	ScrollBack( this );

	ScrollBack.DoModal();
}

void CMainFrame::OnFileSaveTranscript() 
{
	//LOGFUNCTION
	CString		name;
	szBigBuffer[bbpos]=0;
	name.GetBuffer(256);
	name.Format("%s", stripped_story_name );
	name = name.Left( name.ReverseFind( '.' ) );
	name = name + ".txt";
	CFileDialog	cfdTransScript( FALSE, ".txt", name, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, NULL, this );
	if( cfdTransScript.DoModal()==IDOK )
	{
		name = cfdTransScript.GetPathName();
		CFile	TransScript( name, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite );
		if( TransScript.m_hFile )
		{
			int	i;
			for( i=0; i < nScrollBuffs; i++ )
				TransScript.Write( ScrollRoll[i], strlen( ScrollRoll[i] ));
			if( bbpos )
				TransScript.Write( szBigBuffer, bbpos );
			TransScript.Close();
		}
	}
}

LRESULT CMainFrame::OnSetIconType(WPARAM wparam, LPARAM lparam)
{
	//LOGFUNCTION
	HICON	icon = NULL;

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
		::SetClassLong( this->m_hWnd, GCL_HICON, (LONG)icon ); 

	return 0;
}


//The rest of the functions here all do essentially the same thing: disable menu entries when
//condition exist that prevent them from being used.
void CMainFrame::OnUpdateViewStatusBar(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	pCmdUI->SetCheck( !(misc_flags & STATUS_BAR_DISABLED) );

	if( nState == BUSY || !bResize )
		pCmdUI->Enable( FALSE );
	else
		pCmdUI->Enable( TRUE );
}


void CMainFrame::OnUpdateViewToolbar(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	pCmdUI->SetCheck( !(misc_flags & TOOL_BAR_DISABLED) );

	if( nState == BUSY || !bResize )
		pCmdUI->Enable( FALSE );
	else
		pCmdUI->Enable( TRUE );
}


void CMainFrame::OnUpdateViewFont(CCmdUI* pCmdUI) 
{
	////LOGFUNCTION
	if( nState == BUSY || h_version==6 )
		pCmdUI->Enable( FALSE );
	else
		pCmdUI->Enable( TRUE );
}

void CMainFrame::OnUpdateViewStatusFont(CCmdUI* pCmdUI) 
{
	////LOGFUNCTION
	if( nState == BUSY || !bResize )
		pCmdUI->Enable( FALSE );
	else
		pCmdUI->Enable( TRUE );
	
}

void CMainFrame::OnUpdateColorsUserinvbg(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	if( display < MCGA_MODE )
		pCmdUI->Enable( FALSE );
	else
		pCmdUI->Enable( TRUE );
}

void CMainFrame::OnUpdateColorsUserinvfg(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	if( display < MCGA_MODE )
		pCmdUI->Enable( FALSE );
	else
		pCmdUI->Enable( TRUE );
}

void CMainFrame::OnUpdateFileSaveAlias(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	if( bStoryLoaded )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );
}

void CMainFrame::OnUpdateFileGame(CCmdUI* pCmdUI) 
{
	////LOGFUNCTION
	if( bStoryLoaded )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );
}

void CMainFrame::OnUpdateFileAlias(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	if( bStoryLoaded )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );
}

void CMainFrame::OnUpdateFileRec(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	if( bStoryLoaded )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );
}

void CMainFrame::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	if( bStoryLoaded )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );
}

void CMainFrame::OnUpdateFileSaveTranscript(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	if( hWnd )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );
}

void CMainFrame::OnUpdateFileSaveRec(CCmdUI* pCmdUI) 
{
	//LOGFUNCTION
	if( bStoryLoaded )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );
}

void CMainFrame::OnEditUndo() 
{
	//LOGFUNCTION
	if( nState == OS_READ )
	{
		nState = UNDO_EDIT;
		DestroyCaret();
		ResumeThread( hThread );
	}
}

void CMainFrame::OnOptionsAutoLoad() 
{
	//LOGFUNCTION
	CAutoLoad	autofile;
	autofile.DoModal();
}

void CMainFrame::OnOptionsUndoGame() 
{
	//LOGFUNCTION
	if( (kbIndex < KEYBOARD_BUFFER_SIZE) && (nState > INIT) && (nState < CHANGE_FONT) )
	{
		kbModals[ kbIndex ] |= ALT_KEY_PRESSED;
		kbBuffer[ kbIndex++ ] = 'U';
		DestroyCaret();
		ResumeThread( hThread );
	}
}
