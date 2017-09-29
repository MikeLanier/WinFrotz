// GraphOpt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGraphOpt dialog

class CGraphOpt : public CDialog
{
// Construction
public:
	CGraphOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGraphOpt)
	enum { IDD = IDD_GRAPHICS };
	int		m_ScreenSize;
	int		m_GraphMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGraphOpt)
	virtual BOOL OnInitDialog();
	afx_msg void OnGraphicsFont();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
