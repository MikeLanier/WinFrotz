// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
//	afx_msg BOOL OnQueryNewPalette();
//	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	CPalette* SetPalette(CPalette* pPalette);
	CPalette* m_pPalette;
	afx_msg void OnUpdateTime(CCmdUI* pCmdUI);
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	int m_nTimePaneNo;
// Operations
public:
	void	UpdateStatusBar( void );
	LRESULT	OnSetIconType( WPARAM, LPARAM );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	char	cSpecialKey[512]; 

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnClose();
	afx_msg void OnUpdateViewFont(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewStatusFont(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewStatusBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewStatusBar();
	afx_msg void OnViewToolbar();
	afx_msg void OnViewDisplayOpt();
	afx_msg void OnUpdateColorsUserinvbg(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColorsUserinvfg(CCmdUI* pCmdUI);
	afx_msg void OnAliasManage();
	afx_msg void OnFileSaveAlias();
	afx_msg void OnFileGame();
	afx_msg void OnFileAlias();
	afx_msg void OnUpdateFileAlias(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileGame(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAlias(CCmdUI* pCmdUI);
	afx_msg void OnFileRec();
	afx_msg void OnUpdateFileRec(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveRec();
	afx_msg void OnUpdateFileSaveRec(CCmdUI* pCmdUI);
	afx_msg void OnAppExit();
	afx_msg void OnViewGraphics();
	afx_msg void OnEditPaste();
	afx_msg void OnOptionsDebugging();
	afx_msg void OnOptionsSound();
	afx_msg void OnViewScrollback();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveTranscript();
	afx_msg void OnUpdateFileSaveTranscript(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnOptionsAutoLoad();
	afx_msg void OnOptionsUndoGame();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL InitStatusBar(UINT *pIndicators, int nSize, int nSeconds);
	void ChangeStatus( unsigned long overwrite);
	void SetStatusPane( UINT nID, LPSTR text );
};

/////////////////////////////////////////////////////////////////////////////
