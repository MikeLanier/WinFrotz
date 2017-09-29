// strings.h : defines all extern const strings

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "crosscomp.h"

#define MAX_CMD_LINE	512
#define	MAX_ARGS		20
#define	MIN_HORZ_CHARS	32
#define	MIN_VERT_CHARS	18
#define START_HORZ_CHARS 80
#define START_VERT_CHARS 25


extern int nMinX, nMinY;
extern int	nMinYMod;

extern "C" HWND hWnd;
extern "C" HANDLE	hThread;
extern "C"	int display;
extern "C"	COLORREF	user_foreground;
extern "C"	COLORREF	user_background;
extern int		nToolBar;
extern int		nStatusBar;

#define LINE_SIG_STYLE	255
#define LINE_SIG_FG		254
#define LINE_SIG_BG		253
#define LINE_SIG_NEWLINE 252
