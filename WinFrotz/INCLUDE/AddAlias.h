// AddAlias.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AddAlias dialog

class AddAlias : public CDialog
{
// Construction
public:
	AddAlias(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AddAlias)
	enum { IDD = IDD_ADD_ALIAS };
	CString	m_find;
	CString	m_replace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AddAlias)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AddAlias)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
