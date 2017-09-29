// WinFrotz.h : main header file for the WINFROTZ application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "../resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWinFrotzApp:
// See WinFrotz.cpp for the implementation of this class
//

class CWinFrotzApp : public CWinApp
{
public:
	CWinFrotzApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinFrotzApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWinFrotzApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
