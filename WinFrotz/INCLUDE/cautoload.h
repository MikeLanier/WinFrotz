#if !defined(AFX_CAUTOLOAD_H__97B08001_1E87_11D1_88EF_44455354616F__INCLUDED_)
#define AFX_CAUTOLOAD_H__97B08001_1E87_11D1_88EF_44455354616F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CAutoLoad.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoLoad dialog

class CAutoLoad : public CDialog
{
// Construction
public:
	CAutoLoad(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoLoad)
	enum { IDD = IDD_AUTO_LOAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoLoad)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoLoad)
	virtual BOOL OnInitDialog();
	afx_msg void OnAutoOverride();
	virtual void OnOK();
	afx_msg void OnAutoButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAUTOLOAD_H__97B08001_1E87_11D1_88EF_44455354616F__INCLUDED_)
