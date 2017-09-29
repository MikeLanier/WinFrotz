#if !defined(AFX_CMYFONTDIALOG_H__A19A2301_FD62_11D0_8B6B_444553540000__INCLUDED_)
#define AFX_CMYFONTDIALOG_H__A19A2301_FD62_11D0_8B6B_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CMyFontDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyFontDialog dialog

class CMyFontDialog : public CFontDialog
{
	DECLARE_DYNAMIC(CMyFontDialog)

public:
	CMyFontDialog(LPLOGFONT lplfInitial = NULL,
		DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
	void SetWindowName( char *name );
#ifndef _AFX_NO_RICHEDIT_SUPPORT
	CMyFontDialog(const CHARFORMAT& charformat,
		DWORD dwFlags = CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
#endif

protected:
	CString	DlgName;
	//{{AFX_MSG(CMyFontDialog)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMYFONTDIALOG_H__A19A2301_FD62_11D0_8B6B_444553540000__INCLUDED_)
