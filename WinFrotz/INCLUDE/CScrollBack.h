// CScrollBack.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScrollBack dialog

class CScrollBack : public CDialog
{
// Construction
public:
	CEdit	*pEditBox;
	CFont	*stdFont;
	CScrollBack(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CScrollBack)
	enum { IDD = IDD_SCROLLBACK };
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrollBack)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScrollBack)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
