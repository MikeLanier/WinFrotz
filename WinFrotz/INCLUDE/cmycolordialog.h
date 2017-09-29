// CMyColorDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyColorDialog dialog

class CMyColorDialog : public CColorDialog
{
	DECLARE_DYNAMIC(CMyColorDialog)

public:
	CMyColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0,
			CWnd* pParentWnd = NULL);
	void SetWindowName( char *name );

protected:
	CString	DlgName;
	//{{AFX_MSG(CMyColorDialog)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
