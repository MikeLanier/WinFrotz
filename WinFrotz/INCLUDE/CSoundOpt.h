// CSoundOpt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSoundOpt dialog

class CSoundOpt : public CDialog
{
// Construction
public:
	CSoundOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSoundOpt)
	enum { IDD = IDD_SOUND };
	CSliderCtrl	m_VolSlider;
	BOOL	m_SoundEnabled;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSoundOpt)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
