#include "../include/stdafx.h"
#include "../include/WinFrotz.h"
#include "../include/states.h"
#include "../include/globs.h"
#include "../include/windefs.h"

//---------------------------------------------------------------------------
//| Ini.cpp isn't a class, but it probably should be. These are functions to
//| handle initialization and reading/writing to the registry of persistent
//| data elements
//---------------------------------------------------------------------------
extern	int	bResize;
extern	CSize	csStartSize;
extern	COLORREF	custColors[17];
extern	int	graphics_screen_size;

extern "C"	int	reg_display;
extern "C"	COLORREF	user_reverse_fg;
extern "C"	COLORREF	user_reverse_bg;
extern "C"	COLORREF	user_emphasis;
extern "C"	COLORREF	user_bold;
extern "C"	fontprops	fpFonts[];
extern "C"	int	nUpdates;
extern "C"	int	graphics_mode;
extern "C"	int play_sounds;
extern "C"  int user_max_volume;
unsigned long	misc_flags;

char	szInitFile[ MAX_PATH ];

//These are used externally when a lasting value is changed like a color or the font choice
void WriteRegDWORD( HKEY hkInput, char *item, DWORD value);
void WriteRegBinary( HKEY hkInput, char *item, unsigned char *data, int size );
DWORD ReadRegDWORD( HKEY hkInput, char *item, BOOL bShowError );

DWORD CreateInitialFonts( HKEY hkInput )
{
	//LOGFUNCTION
	HKEY	hkKey;
	DWORD	size, type;

	if( hkInput == NULL )
	{
		if( RegOpenKeyEx(
			HKEY_CURRENT_USER,		// handle of open key 
			"Software\\WinFrotz",	// address of name of subkey to open 
			0,						// reserved 
			KEY_READ,				// security access mask 
			&hkKey 					// address of handle of open key 
			)!=ERROR_SUCCESS )
		{
			AfxGetMainWnd()->MessageBox( "Could not open registry for reading!", "WinFrotz", MB_ICONSTOP );
			return READ_REG_FAIL;
		}
	}
	else
		hkKey = hkInput;

	size = sizeof( LOGFONT );
	if( RegQueryValueEx(
		hkKey,				// handle of key to query 
		ITEM_DISPLAY_FONT,	// address of name of value to query 
		0,					// reserved 
		&type,				// address of buffer for value type 
		(unsigned char *)&fpFonts[STANDARD].lf,	// address of data buffer 
		&size			 	// address of data buffer size 
	)!=ERROR_SUCCESS )
		AfxGetMainWnd()->MessageBox( "Error reading fixed font from the registry!\nIt may have been deleted, select a new\nfont using View/Display Options.", "WinFrotz", MB_ICONSTOP );

	size = sizeof( LOGFONT );
	if( RegQueryValueEx(
		hkKey,				// handle of key to query 
		ITEM_FIXED_FONT,	// address of name of value to query 
		0,					// reserved 
		&type,				// address of buffer for value type 
		(unsigned char *)&fpFonts[FIXED].lf,	// address of data buffer 
		&size			 	// address of data buffer size 
	)!=ERROR_SUCCESS )
		AfxGetMainWnd()->MessageBox( "Error reading fixed font from the registry!\nIt may have been deleted, select a new\nfont using View/Display Options.", "WinFrotz", MB_ICONSTOP );

	if( fpFonts[STANDARD].hfHandle )
		DeleteObject( fpFonts[STANDARD].hfHandle );
	fpFonts[STANDARD].hfHandle = ::CreateFontIndirect( &fpFonts[STANDARD].lf );

	if( fpFonts[FIXED].hfHandle )
		DeleteObject( fpFonts[FIXED].hfHandle );
	fpFonts[FIXED].hfHandle = ::CreateFontIndirect( &fpFonts[FIXED].lf );

	if( hkInput == NULL )
		RegCloseKey( hkKey );

	return TRUE;
}

//*---------------------------------------------------------------------------------
//| WriteRegDWORD writes out a dword to the preset Registry key 
//| HKEY_CURRENT_USER\Software\WinFrotz. If the HKEY passed in is valid it is used, 
//| otherwise the key is grabbed and released within the function. Note that 
//| RegOpenKey is used here instead of RegCreateKey, which is only used at init time.
//| No calls should be made to this prior to HandleRegistry(). Any write to the 
//| Registry that doesn't work is skipped with user notification
//*---------------------------------------------------------------------------------
void WriteRegDWORD( HKEY hkInput, char *item, DWORD value)
{
	//LOGFUNCTION
	HKEY	hkKey;

	if( hkInput == NULL )
	{
		if( RegOpenKeyEx(
			HKEY_CURRENT_USER,		// handle of open key 
			"Software\\WinFrotz",	// address of name of subkey to open 
			0,						// reserved 
			KEY_WRITE,				// security access mask 
			&hkKey 					// address of handle of open key 
			)!=ERROR_SUCCESS )
		{
			AfxGetMainWnd()->MessageBox( "Could not open Registry for writing!", "WinFrotz", MB_ICONSTOP );
			return;
		}
	}
	else
		hkKey = hkInput;
	
	if( RegSetValueEx(
		hkKey,					// handle of key to set value for  
		item,					// address of value to set 
		0,						// reserved 
		REG_DWORD,				// flag for value type 
		(unsigned char *)&value,// address of value data 
		sizeof( COLORREF )		// data buffer size
		)!=ERROR_SUCCESS )
		AfxGetMainWnd()->MessageBox( "Could not write a value to the Registry!", "WinFrotz", MB_ICONSTOP );
	
	if( hkInput == NULL )
		RegCloseKey( hkKey );
}

//*---------------------------------------------------------------------------------
//| ReadRegDWORD retrieves an existing value. To make it bulletproof the calling routine can 
//| request to display errors or not, depending on how fatal they are considered
//*---------------------------------------------------------------------------------
DWORD ReadRegDWORD( HKEY hkInput, char *item, BOOL bShowError )
{
	//LOGFUNCTION
	DWORD type, size, value;
	HKEY	hkKey;
	
	if( hkInput == NULL )
	{
		if( RegOpenKeyEx(
			HKEY_CURRENT_USER,		// handle of open key 
			"Software\\WinFrotz",	// address of name of subkey to open 
			0,						// reserved 
			KEY_READ,				// security access mask 
			&hkKey 					// address of handle of open key 
			)!=ERROR_SUCCESS )
		{
			if( bShowError )
				AfxGetMainWnd()->MessageBox( "Could not open registry for reading!", "WinFrotz", MB_ICONSTOP );
			return READ_REG_FAIL;
		}
	}
	else
		hkKey = hkInput;
	
	//Win95 is really picky about having this size set; WinNT doesn't care. Go figure.
	size = sizeof( DWORD );
	if( RegQueryValueEx(
		hkKey,					// handle of key to query 
		item,					// address of name of value to query 
		0,						// reserved 
		&type,					// address of buffer for value type 
		(unsigned char *)&value,// address of data buffer 
		&size			 		// address of data buffer size 
		)!=ERROR_SUCCESS )
	{
		if( bShowError )
			AfxGetMainWnd()->MessageBox( "Error reading value from Registry!", "WinFrotz", MB_ICONSTOP );
		else
		{
			if( hkInput == NULL )
				RegCloseKey( hkKey );
			return READ_REG_FAIL;
		}
	}
	
	if( type!=REG_DWORD || size != sizeof( DWORD ) )
		AfxGetMainWnd()->MessageBox( "Error reading value from Registry - wrong size!", "WinFrotz", MB_ICONSTOP );

	if( hkInput == NULL )
		RegCloseKey( hkKey );
	
	return value;
}

//*---------------------------------------------------------------------------------
//| WriteRegBinary is similar to WriteRegDWORD except it dumps an arbitrary binary section
//| of data
//*---------------------------------------------------------------------------------
void WriteRegBinary( HKEY hkInput, char *item, unsigned char *data, int size )
{
	//LOGFUNCTION
	HKEY	hkKey;
	
	if( hkInput == NULL )
	{
		if( RegOpenKeyEx(
			HKEY_CURRENT_USER,		// handle of open key 
			"Software\\WinFrotz",	// address of name of subkey to open 
			0,						// reserved 
			KEY_WRITE,				// security access mask 
			&hkKey 				// address of handle of open key 
			)!=ERROR_SUCCESS )
		{
			AfxGetMainWnd()->MessageBox( "Could not open registry for writing!", "WinFrotz", MB_ICONSTOP );
			return;
		}
	}
	else
		hkKey = hkInput;
	
	if( RegSetValueEx(
		hkKey,				// handle of key to set value for  
		item,					// address of value to set 
		0,						// reserved 
		REG_BINARY,				// flag for value type 
		data,					// address of value data 
		size					// data buffer size
		)!=ERROR_SUCCESS )
		AfxGetMainWnd()->MessageBox( "Error writing font to registry!", "WinFrotz", MB_ICONSTOP );
	
	if( hkInput == NULL )
		RegCloseKey( hkKey );
}

//*---------------------------------------------------------------------------------
//| HandleRegistry() - Create the Registry entries if they don't exist and read all
//|	the defaults in at runtime (this is called from MainFrame)
//*---------------------------------------------------------------------------------
VOID HandleRegistry()
{
	//LOGFUNCTION
	DWORD	disposition, type, size, tmpval;
	HKEY	hkInitKey;
	CClientDC	dc( NULL );

	if( RegCreateKeyEx(
		HKEY_CURRENT_USER,		// handle of an open key 
		"Software\\WinFrotz",	// address of subkey name 
		0,						// reserved 
		"WinFrotz",				// address of class string 
		REG_OPTION_NON_VOLATILE,// special options flag 
		KEY_ALL_ACCESS,			// desired security access 
		NULL,					// address of key security structure 
		&hkInitKey,				// address of buffer for opened handle  
		&disposition 			// address of disposition value buffer 
						)!=ERROR_SUCCESS )
	{
		AfxGetMainWnd()->MessageBox( "Error opening registry!", "WinFrotz", MB_ICONSTOP );
		//Probably should make this fault in a more elegant manner
		exit( 1 );
	}
	
	// If the key doesn't exist, fill in defaults. This is the only time these
	// will be written all at once. From here on out we trust the Registry to hold them
	// (yeah, right) and update when key dialogs are used to configure them
	if( disposition == REG_CREATED_NEW_KEY )
	{
		WriteRegDWORD( hkInitKey, ITEM_COLORMODE, (DWORD)MONO_MODE );
		WriteRegDWORD( hkInitKey, ITEM_UPDATES, (DWORD)1 );	//Frequent updates
		WriteRegDWORD( hkInitKey, ITEM_EMPHASIS_FONT_USAGE, (DWORD)DISPLAY_FONT );
		WriteRegDWORD( hkInitKey, ITEM_BOLD_FONT_USAGE, (DWORD)DISPLAY_FONT );
		WriteRegDWORD( hkInitKey, ITEM_FOREGROUND, (DWORD)user_foreground );
		WriteRegDWORD( hkInitKey, ITEM_BACKGROUND, (DWORD)user_background );
		WriteRegDWORD( hkInitKey, ITEM_INV_FOREGROUND, (DWORD)user_background );
		WriteRegDWORD( hkInitKey, ITEM_INV_BACKGROUND, (DWORD)user_foreground );
		WriteRegDWORD( hkInitKey, ITEM_EMPHASIS, (DWORD)user_emphasis );
		WriteRegDWORD( hkInitKey, ITEM_BOLD, (DWORD)user_bold );
		WriteRegDWORD( hkInitKey, ITEM_RESIZE, (DWORD)bResize );
		WriteRegDWORD( hkInitKey, ITEM_GRAPHICS_SCREEN_SIZE, (DWORD)0 );
		WriteRegDWORD( hkInitKey, ITEM_GRAPHICS_MODE, (DWORD)MCGA_MODE );
		WriteRegDWORD( hkInitKey, ITEM_SOUND_PLAYED, (DWORD)1 );
		WriteRegDWORD( hkInitKey, ITEM_SOUND_VOLUME, (DWORD)user_max_volume );
		
		// Start out with regular system font for display. Grab the LOGFONT info
		// so we can read it in later and get a HANDLE from CreateFontIndirect
		memset( &fpFonts[STANDARD].lf, 0, sizeof( LOGFONT ) );
		strcpy( fpFonts[STANDARD].lf.lfFaceName, "Courier New" );
		fpFonts[STANDARD].lf.lfWeight = FW_NORMAL;
		fpFonts[STANDARD].lf.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
		fpFonts[STANDARD].lf.lfHeight = -MulDiv(10, dc.GetDeviceCaps(LOGPIXELSY), 72);
		fpFonts[STANDARD].hfHandle = CreateFontIndirect( &fpFonts[STANDARD].lf );

		WriteRegBinary( hkInitKey, ITEM_DISPLAY_FONT, (unsigned char *)&fpFonts[STANDARD].lf, sizeof( LOGFONT ) );

		// Same for Fixed Font
		memset( &fpFonts[FIXED].lf, 0, sizeof( LOGFONT ) );
		strcpy( fpFonts[FIXED].lf.lfFaceName, "Courier New" );
		fpFonts[FIXED].lf.lfWeight = FW_NORMAL;
		fpFonts[FIXED].lf.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
		fpFonts[FIXED].lf.lfHeight = -MulDiv(10, dc.GetDeviceCaps(LOGPIXELSY), 72);
		fpFonts[FIXED].hfHandle = CreateFontIndirect( &fpFonts[FIXED].lf );

		WriteRegBinary( hkInitKey, ITEM_FIXED_FONT, (unsigned char *)&fpFonts[FIXED].lf, sizeof( LOGFONT ) );
		WriteRegBinary( hkInitKey, ITEM_CUSTCOLORS, (unsigned char *)&custColors[0], sizeof( COLORREF ) * 16 );
	}

	//Read in the values from the Registry. Only fail and return error when it
	//is REALLY fatal (you never know what somebody might do with their registry)
	//For most of these an error will result in the value being the default run-time

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_COLORMODE, FALSE ))!=READ_REG_FAIL)
		reg_display = display =(int)tmpval;
	else
		reg_display = display = MCGA_MODE;
	
	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_UPDATES, FALSE ))!=READ_REG_FAIL)
		nUpdates = (int)tmpval;
	else
		nUpdates = 1;
	
	//It's ok to fail these two because AddStyleFonts will deal with it later
	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_EMPHASIS_FONT_USAGE, FALSE ))!=READ_REG_FAIL)
		fpFonts[EMPHASIS].nUsagePerm = (int)tmpval;
	
	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_BOLD_FONT_USAGE, FALSE ))!=READ_REG_FAIL)
		fpFonts[BOLD].nUsagePerm = (int)tmpval;

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_FOREGROUND, FALSE ))!=READ_REG_FAIL )
		user_foreground = (COLORREF)tmpval;
	else
		user_foreground = RGB( 192, 192, 192 );

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_BACKGROUND, FALSE ))!=READ_REG_FAIL )
		user_background = (COLORREF)tmpval;
	else
		user_background = RGB( 0, 0, 128);

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_INV_FOREGROUND, FALSE ))!=READ_REG_FAIL )
		user_reverse_fg = (COLORREF)tmpval;
	else
		user_reverse_fg = user_background;

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_INV_BACKGROUND, FALSE ))!=READ_REG_FAIL )
		user_reverse_bg = (COLORREF)tmpval;
	else
		user_reverse_bg = user_foreground;

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_EMPHASIS, FALSE ))!=READ_REG_FAIL )
		user_emphasis = (COLORREF)tmpval;
	else
		user_emphasis = RGB( 255, 255, 0 );

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_BOLD, FALSE ))!=READ_REG_FAIL )
		user_bold = (COLORREF)tmpval;
	else
		user_bold = RGB( 255, 255, 255 );

	//csStartSize default has already been set, as has bResize
	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_XSIZE, FALSE ))!=READ_REG_FAIL )
		csStartSize.cx = (int)tmpval;

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_YSIZE, FALSE ))!=READ_REG_FAIL )
		csStartSize.cy = (int)tmpval;

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_RESIZE, FALSE))!=READ_REG_FAIL )
		bResize = (int) tmpval;

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_GRAPHICS_SCREEN_SIZE, FALSE))!=READ_REG_FAIL )
		graphics_screen_size = (int) tmpval;
	else
		graphics_screen_size = 0;
	
	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_GRAPHICS_MODE, FALSE))!=READ_REG_FAIL )
		graphics_mode = (int) tmpval;
	else
		graphics_mode = 1;

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_SOUND_PLAYED, FALSE))!=READ_REG_FAIL )
		play_sounds = tmpval;

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_SOUND_VOLUME, FALSE))!=READ_REG_FAIL )
		user_max_volume = tmpval;

	if( (tmpval = ReadRegDWORD( hkInitKey, ITEM_MISC_FLAGS, FALSE))!=READ_REG_FAIL )
		misc_flags = tmpval;
	else
		misc_flags = 0;

	//Don't use user_foreground and user_background here. With monochrome bitmaps
	//RGB(0,0,0) _means_ user_background
	if( display < MCGA_MODE )
	{
		user_reverse_fg = RGB( 255, 255, 255 );
		user_reverse_bg = RGB( 0, 0, 0 );
	}

	CreateInitialFonts( hkInitKey );

	size = sizeof( LOGFONT );
	if( RegQueryValueEx(
		hkInitKey,			// handle of key to query 
		ITEM_GRAPHICS_FONT,	// address of name of value to query 
		0,					// reserved 
		&type,				// address of buffer for value type 
		(unsigned char *)&fpFonts[GRAPHICS].lf,	// address of data buffer 
		&size			 	// address of data buffer size 
	)!=ERROR_SUCCESS )
	{
		memset( &fpFonts[GRAPHICS].lf, 0, sizeof( LOGFONT ) );
		strcpy( fpFonts[GRAPHICS].lf.lfFaceName, "Courier New" );
		fpFonts[GRAPHICS].lf.lfWeight = FW_NORMAL;
		fpFonts[GRAPHICS].lf.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
		fpFonts[GRAPHICS].lf.lfHeight = -MulDiv(8, dc.GetDeviceCaps(LOGPIXELSY), 72);
		fpFonts[GRAPHICS].hfHandle = CreateFontIndirect( &fpFonts[GRAPHICS].lf );
	}
	else
		fpFonts[GRAPHICS].hfHandle = CreateFontIndirect( &fpFonts[GRAPHICS].lf );

	if( !fpFonts[GRAPHICS].hfHandle )
		AfxGetMainWnd()->MessageBox( "Couldn't choose a Graphics font. To use Version 6\ngames, select a graphics font from the\nView/Graphics... menu option", "WinFrotz", MB_ICONINFORMATION );

	size = sizeof( COLORREF ) * 16;
	if( RegQueryValueEx( 
		hkInitKey,			// handle of key to query 
		ITEM_CUSTCOLORS,	// address of name of value to query 
		0,					// reserved 
		&type,				// address of buffer for value type 
		(unsigned char *)&custColors[0],	// address of data buffer 
		&size			 	// address of data buffer size 
		)) {}

	size = MAX_PATH;
	if( RegQueryValueEx( 
		hkInitKey,			// handle of key to query 
		ITEM_INITIALFILE,	// address of name of value to query 
		0,					// reserved 
		&type,				// address of buffer for value type 
		(unsigned char *)szInitFile,			// address of data buffer 
		&size			 	// address of data buffer size 
	)!=ERROR_SUCCESS )
	{
		GetCurrentDirectory( MAX_PATH, szInitFile );
		if( szInitFile[ strlen( szInitFile ) -1 ] == '\\' )
			strcat( szInitFile, "ZTUU.z5" );
		else
			strcat( szInitFile, "\\ZTUU.z5" );
	}

	RegCloseKey( hkInitKey );
	custColors[0] = user_foreground;
	custColors[1] = user_background;
	custColors[2] = user_reverse_fg;
	custColors[3] = user_reverse_bg;
	custColors[4] = user_emphasis;
	custColors[5] = user_bold;
}

