// CDebugOpt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDebugOpt dialog

class CDebugOpt : public CDialog
{
// Construction
public:
	CDebugOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDebugOpt)
	enum { IDD = IDD_DEBUGGING };
	BOOL	m_Assignment;
	BOOL	m_Locating;
	BOOL	m_Movement;
	BOOL	m_Testing;
	BOOL	m_Pirate;
	BOOL	m_Tandy;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDebugOpt)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
