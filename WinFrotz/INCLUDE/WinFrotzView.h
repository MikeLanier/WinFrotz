// WinFrotzView.h : interface of the CWinFrotzView class
//
/////////////////////////////////////////////////////////////////////////////

class CWinFrotzView : public CView
{
protected: // create from serialization only
	CWinFrotzView();
	DECLARE_DYNCREATE(CWinFrotzView)

// Attributes
public:
	CWinFrotzDoc* GetDocument();
	CFont	*m_pFont, *m_pEmphFont, *m_pFixedFont;
	LOGFONT	m_logfont;
	int		m_nPointSize;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinFrotzView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWinFrotzView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Generated message map functions
protected:
	void SetColorFG( COLORREF new_color_foreground );
	//{{AFX_MSG(CWinFrotzView)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnViewFont();
	afx_msg void OnColorsUserfg();
	afx_msg void OnColorsUserbg();
	afx_msg void OnViewStatusFont();
	afx_msg void OnColorsUserinvbg();
	afx_msg void OnColorsUserinvfg();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnColorsEmphasis();
	afx_msg void OnColorsBold();
	afx_msg void OnUpdateColorsEmphasis(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColorsBold(CCmdUI* pCmdUI);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnCursorPos(WPARAM wParam, LPARAM lParam);
	afx_msg	LRESULT	OnV6Game(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCursorHide(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThreadDone(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WinFrotzView.cpp
inline CWinFrotzDoc* CWinFrotzView::GetDocument()
   { return (CWinFrotzDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
