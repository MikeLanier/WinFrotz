// thread.cpp : implementation file
//

//*--------------------------------------------------------------------------
//| Mostly this deals with tearing down the thread created for the Z-Machine.
//| There are more elegant ways to do this (semaphores), but for the simple
//| thread model used here (one GUI/one workhorse) this works fine.
//| For a good example of threads (which this is not) see "Multithreading
//| Applications in Win32" by Jum Beveridge and Robert Wiener Addison-Wesley press
//*--------------------------------------------------------------------------


#include "../include/stdafx.h"
#include "../include/WinFrotz.h"
#include "../include/mainfrm.h"
#include "../include/states.h"
#include "../include/globs.h"
#include "../include/windefs.h"
#include "../include/WinFrotzDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

int		nToolBar = 30;
int		nStatusBar = 18;
char	szCurrentStory[256];
CSize	csStartSize( 0, 0);
CSize	csNormalDim( 0, 0);
int		bResize = 1;
int		graphics_screen_size;

extern "C"	fontprops	fpFonts[];
extern "C"	BOOL	bStoryLoaded;
extern "C"	BOOL	bStoryInit;
extern "C"	int		nState;
extern "C"	HBITMAP	hbmScreenBuffer;
extern "C"	HGDIOBJ	oldMemBmap;
extern "C"	HDC		h_memDC;
extern "C"	zbyte	h_version;
extern "C"	COLORREF pc_colour_fg[];
extern "C"	HDC		hdc;
extern "C"	void	init_thread( void );

extern DWORD CreateInitialFonts( HKEY hkInput );
extern	DWORD ReadRegDWORD( HKEY hkInput, char *item, BOOL bShowError );
extern CClientDC	*m_pDC;

static	BOOL	bThreadIsClosing = FALSE;

//Just a convienance function to statisfy the CreateThread call.
//I don't use the LPDWORD but I suppose it could be a useful signal
DWORD Threadinterpret(LPDWORD lpdwParam) 
{  
	//LOGFUNCTION
    if( *lpdwParam )
	    init_thread();
    return 0; 
} 

//Starts the Z-Machine thread 
BOOL	startThread( CDC *pDC )
{
	//LOGFUNCTION
	DWORD dwThrdParam = 1, dwThreadId; 
	unsigned long ExitCode;
	CDC	dcMem;

	hThread = CreateThread( NULL, 0,(LPTHREAD_START_ROUTINE) Threadinterpret,&dwThrdParam,0,&dwThreadId);
	bStoryInit = TRUE;

	// Did it fail to spawn?
	if (!hThread)
	{
		bStoryLoaded = FALSE;
		bStoryInit = FALSE;
		return FALSE;
	}
	else
	{
		//Just check to make sure it kept running
		SleepEx( 25, TRUE );
		GetExitCodeThread( hThread,	&ExitCode );
		if( ExitCode != STILL_ACTIVE )
		{
			bStoryLoaded = FALSE;
			bStoryInit = FALSE;
			return FALSE;
		}
	}
	return TRUE;
}


//Handles the actual killing off of the user thread. Should probably re-write this
//to be more thread savvy instead of using bludgeon method found here.

void	TerminateUserThread( void )
{
	//LOGFUNCTION
	unsigned long ExitCode;

	// Make sure we understand we don't have a story anymore 
	bStoryLoaded = FALSE;
	bStoryInit = FALSE;

	//Don't leave any timers around to accidentally wake up the thread
	if( hWnd )
	{
		::KillTimer( hWnd, 1 );
		::PostMessage( hWnd, WM_CURSOR_HIDE, WM_CURSOR_HIDE, 0 );
	}
	
	if( hThread )
	{
		if( nState!=FINISHED )
		{
			GetExitCodeThread( hThread,	&ExitCode );
			
			if( ExitCode == STILL_ACTIVE )
			{
				int count = 0;

				while( nState <= INIT )
					SleepEx( 25, TRUE );
				
				if( nState != FINISHED )
				{
					nState = SIGNAL_FINISHED;
					
					ResumeThread( hThread );
					
					while( nState != FINISHED && count < 10 )
					{
						count++;
						SleepEx( 25, TRUE );
					}
					
					if( count == 10 && nState!=SIGNAL_FINISHED )
						AfxGetMainWnd()->MessageBox( "Warning: Z-Code thread terminated abnormally", "WinFrotz", MB_ICONSTOP );

				}
			}
		}
		//Terminating a thread that no longer exists isn't harmful, so always do this
		TerminateThread( hThread, 0 );
		hThread = NULL;
	}
}

//PostThreadCleanup sets all the variables back to where they should be after the user
//thread exits. I need to this because the death of the user thread could be one step
//on the way to a completely new story file, which will want to use all of these values
void PostThreadCleanup( void )
{
	//LOGFUNCTION
	int	tmpval;

	if( hWnd )
	{
		if( hdc )
			::ReleaseDC( hWnd, hdc );
		hdc = NULL;
		hWnd = NULL;
	}

	if( h_memDC && hbmScreenBuffer )
	{
		::SelectObject( h_memDC, oldMemBmap );
		::DeleteObject( hbmScreenBuffer );
		hbmScreenBuffer = NULL;
	}

	//If this is a version 6 game, restore all the things we changed
	if( h_version==6 )
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd( );

		pc_colour_fg[9] = RGB( 255, 255, 255 );

		CreateInitialFonts( NULL );

		if( (tmpval = ReadRegDWORD( NULL, ITEM_RESIZE, TRUE))!=READ_REG_FAIL )
			bResize = (int) tmpval;

		if( (tmpval = ReadRegDWORD( NULL, ITEM_FOREGROUND, TRUE ))!=READ_REG_FAIL )
			user_foreground = (COLORREF)tmpval;

		if( (tmpval = ReadRegDWORD( NULL, ITEM_BACKGROUND, TRUE ))!=READ_REG_FAIL )
			user_background = (COLORREF)tmpval;

		h_version = 3;

		if( (tmpval = ReadRegDWORD( NULL, ITEM_COLORMODE, FALSE ))!=READ_REG_FAIL)
			display =(int)tmpval;

		pMain->SetWindowPos( NULL, 0, 0, 
			csNormalDim.cx, 
			csNormalDim.cy, SWP_NOMOVE );

		if( display < MCGA_MODE )
		{
			m_pDC->SetBkColor( user_foreground );
			m_pDC->SetTextColor( user_background );
		}
	}
}

BOOL	killZThread( void )
{
	//LOGFUNCTION
	if( bThreadIsClosing )
		return TRUE;
	bThreadIsClosing = TRUE;
	if( hThread )
		TerminateUserThread( );
	PostThreadCleanup();
	bThreadIsClosing = FALSE;
	return TRUE;
}


