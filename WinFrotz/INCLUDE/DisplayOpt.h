// DisplayOpt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDisplayOpt dialog

class CDisplayOpt : public CDialog
{
// Construction
public:
	CDisplayOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDisplayOpt)
	enum { IDD = IDD_DIALOG_DISPLAY };
	int		m_nDisplayColor;
	int		m_nBoldFont;
	int		m_nEmphFont;
	int		m_nUpdates;
	BOOL	m_IsLocked;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDisplayOpt)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
