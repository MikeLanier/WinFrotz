// AliasList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAliasList dialog

class CAliasList : public CDialog
{
// Construction
public:
	CAliasList(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CAliasList)
	enum { IDD = IDD_ALIAS_LIST };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAliasList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAliasList)
	afx_msg void OnAliasEnable();
	afx_msg void OnAliasDisable();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnAliasDeleteAll();
	afx_msg void OnAliasDelete();
	afx_msg void OnAliasNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
