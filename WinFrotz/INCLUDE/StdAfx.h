// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0502           // Change this to the appropriate value to target other versions of Windows.
#endif

#include "logfile.h"
static LOGfile tLogfile( "c:\\Mike\\Temp\\LOG.txt", true );
