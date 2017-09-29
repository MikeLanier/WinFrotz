#ifndef LOGFILE_H
#define LOGFILE_H
#pragma warning( disable: 4996 )
#pragma warning( disable: 4244 )
#pragma warning( disable: 4102 )

	//<<<<Member definition header template
	//-----------------------------------------------------------------------
	// Summary:
	//     Defines the orientation of the Cell Button within the Cell
	//
	// Remarks:
	//     It can be one of the following states:
	//        * <b>kLeft</b>: Cell Button displays on the left side of the Cell
	//        *                hidden.
	//        *                
	//-----------------------------------------------------------------------
//#include <windows.h> 
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <afx.h>
//#include "stdio.h"
//#include "string.h"
//#ifndef __EXTRAS_H__
//#include "uansi.h"
//#endif

#define NXCOUNT	20000
static int xcount[NXCOUNT];

#ifndef _MAX_PATH
#define	_MAX_PATH	512
#endif

#ifndef BOOL
#define BOOL int
#endif

#ifndef FALSE
#define	FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

// LOGfile class declarations
class LOGfile
{
public:
	char	m_filename[_MAX_PATH];
	char	m_directory[_MAX_PATH];
	char	m_junkname[_MAX_PATH];
	int		m_indent;
	char	buff[819200];
	wchar_t	wbuff[819200];

public:
	void reset()
	{
		enable();
		WritePrivateProfileStringA( m_filename, "Indent", "0", m_junkname );
		m_indent = 0;
	}

	void initialize( char* filename, BOOL bAppend=FALSE )
	{
		for( int i=0; i<NXCOUNT; i++ )	xcount[i] = 0;

		strcpy( m_filename, filename );

		struct _stat	buf;
		int				result = 0;

		result = _stat(	m_filename, &buf );

		strcpy( m_junkname, m_directory );
		strcat( m_junkname, "\\junk.ini" );

		m_indent = 0;
		if( !bAppend || result != 0 )
		{
			FILE* fp = fopen( m_filename, "wt" );
			if( fp )
			{
				enable();
				WritePrivateProfileStringA( m_filename, "Indent", "0", m_junkname );
				m_indent = 0;

				char	datestr[512];
				char	timestr[512];
#				pragma warning( disable: 6202 )
				_strdate( datestr );
#				pragma warning( disable: 6202 )
				_strtime( timestr );
				fprintf( fp, "Logfile: %s: %s: %s\n", m_filename, datestr, timestr );
				fclose( fp );
			}
		}
	}

	LOGfile( char* filename, BOOL bAppend=FALSE )
	{
		strcat( m_directory, getenv("TEMP") );
		initialize( filename, bAppend );
	}

	LOGfile( char* directory, char*filename, BOOL bAppend=FALSE )
	{
		char	name[_MAX_PATH];

		strcpy( m_directory, directory );
		strcpy( name, directory );
		strcat( name, "\\" );
		strcat( name, filename );

		initialize( name, bAppend );
	}

	void restart( bool doit = true )
	{
		for( int i=0; i<NXCOUNT; i++ )	xcount[i] = 0;

		if( doit )
		{
			FILE* fp = fopen( m_filename, "wt" );
			if( fp )
			{
				enable();
				WritePrivateProfileStringA( m_filename, "Indent", "0", m_junkname );

				m_indent = 0;

				fprintf( fp, "Logfile(restart): %s\n", m_filename );
				fclose( fp );
			}
		}
	}

	FILE* open()
	{
		return fopen( m_filename, "at" );
	}

	void close( FILE* fp )
	{
		fclose( fp );
	}

	void write( wchar_t* wstring )
	{
		//if( IsEnabled() == false ) return;
		FILE* fp = fopen( m_filename, "at" );   
		if( fp )
		{
			fwprintf( fp, L"%s", wstring );
			fclose( fp );
		}
	}

	//void write( LPCTSTR wstring )
	//{
	//	//if( IsEnabled() == false ) return;
	//	FILE* fp = fopen( m_filename, "at" );   
	//	if( fp )
	//	{
	//		fwprintf( fp, L"%s", wstring );
	//		fclose( fp );
	//	}
	//}

	void write( const char* string )
	{
		//if( IsEnabled() == false ) return;
		FILE* fp = fopen( m_filename, "at" );   
		if( fp )
		{
			fprintf( fp, "%s", string );
			fclose( fp );
		}
	}

	void writedent( int indent )
	{
		switch( indent )
		{
		case 0:	break;
		case 1: write( "   " );	break;
		case 2:	write( "      " );	break;
		case 3: write( "         " );	break;
		case 4: write( "            " );	break;
		case 5:	write( "               " );	break;
		case 6: write( "                  " );	break;

		default:
			for( int i=0; i<(indent); i++ ) write( "   " );
			break;
		}
	}

	void fwrite( const char *format, ... )
	{
		va_list	argptr;

		va_start( argptr, format );
		vsprintf( buff, format, argptr );

		m_indent = GetPrivateProfileIntA( m_filename, "Indent", 0, m_junkname );
		writedent( m_indent );
		write( buff );
	}

	void fwriteln( const char *format, ... )
	{
		va_list	argptr;

		va_start( argptr, format );
		vsprintf( buff, format, argptr );

		strcat( buff, "\n" );
		m_indent = GetPrivateProfileIntA( m_filename, "Indent", 0, m_junkname );
		writedent( m_indent );
		write( buff );
	}

	void fwriteln(const wchar_t *format, ...)
	{
		va_list	argptr;

		va_start(argptr, format);
		vswprintf(wbuff, format, argptr);

		wcscat(wbuff, L"\n");
		m_indent = GetPrivateProfileIntA(m_filename, "Indent", 0, m_junkname);
		writedent(m_indent);
		write(wbuff);
	}

	void fwrite(const wchar_t *format, ...)
	{
		va_list	argptr;

		va_start(argptr, format);
		vswprintf(wbuff, format, argptr);

		m_indent = GetPrivateProfileIntA(m_filename, "Indent", 0, m_junkname);
		writedent(m_indent);
		write(wbuff);
	}

	void addindent(int i=0) 
	{ 
		m_indent = GetPrivateProfileIntA( m_filename, "Indent", 0, m_junkname );

		writedent( m_indent++ );

		if( i == 0 )
		{
			write( "{\n" );
		}
		else
		{
			sprintf( buff, "{ : %1d\n", i );
			write( buff );
		}

		sprintf( buff, "%1d", m_indent );
		WritePrivateProfileStringA( m_filename, "Indent", buff, m_junkname );
	}

	void subindent(int i=0) 
	{ 
		m_indent = GetPrivateProfileIntA( m_filename, "Indent", 0, m_junkname );

		if( --m_indent < 0 )
		{
			m_indent = 0; 

			sprintf( buff, "%1d", m_indent );
			WritePrivateProfileStringA( m_filename, "Indent", buff, m_junkname );

			return;
		}

		writedent( m_indent );

		if( i==0 )
		{
			write( "}\n" );
		}
		else
		{
			sprintf( buff, "} : %1d\n", i );
			write( buff );
		}

		sprintf( buff, "%1d", m_indent );
		WritePrivateProfileStringA( m_filename, "Indent", buff, m_junkname );
	}

	int		getindent()
	{
		m_indent = GetPrivateProfileIntA( m_filename, "Indent", 0, m_junkname );
		return m_indent;
	}

	void	setindent( int indent )
	{
		char buff[127];
		m_indent = (indent < 0) ? 0 : indent;
		sprintf( buff, "%1d", m_indent );
		WritePrivateProfileStringA( m_filename, "Indent", buff, m_junkname );
	}

	void indent() 
	{ 
		m_indent = GetPrivateProfileIntA( m_filename, "Indent", 0, m_junkname );
		sprintf( buff, "%1d", ++m_indent );
		WritePrivateProfileStringA( m_filename, "Indent", buff, m_junkname );
	}

	void exdent() 
	{ 
		m_indent = GetPrivateProfileIntA( m_filename, "Indent", 0, m_junkname );

		if( --m_indent < 0 )
		{
			m_indent = 0; 

			sprintf( buff, "%1d", m_indent );
			WritePrivateProfileStringA( m_filename, "Indent", buff, m_junkname );

			return;
		}

		sprintf( buff, "%1d", m_indent );
		WritePrivateProfileStringA( m_filename, "Indent", buff, m_junkname );
	}

	bool IsEnabled()
	{
		int enabled = GetPrivateProfileIntA( m_filename, "Enabled", 0, m_junkname );
		return enabled == 1 ? true : false;
	}

	void	enable()
	{
		WritePrivateProfileStringA( m_filename, "Enabled", "1", m_junkname );
	}

	void	disable()
	{
		WritePrivateProfileStringA( m_filename, "Enabled", "0", m_junkname );
	}

};

static char	xbuff[81920000];
static char* xNarrow( LPCWSTR wc )
{
	if( wc )
	{
		WideCharToMultiByte(	CP_ACP,	0,
								wc,		-1,
								xbuff,	sizeof(xbuff),
								NULL,	NULL );

		xbuff[wcslen(wc)] = 0;
	}
	else
		strcpy( (char *)xbuff, "NULL" );

	return xbuff;
}

static char	ybuff[8192];
static char* yNarrow( LPCWSTR wc )
{
	if( wc )
	{
		WideCharToMultiByte(	CP_ACP,	0,
								wc,		-1,
								ybuff,	sizeof(ybuff),
								NULL,	NULL );

		ybuff[wcslen(wc)] = 0;
	}
	else
		strcpy( (char *)ybuff, "NULL" );

	return ybuff;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
#define	PROBEi(i)		tLogfile.fwriteln( "PROBE(%1d): %s[%1d]", i, __FUNCTION__, __LINE__ );
#define	PROBE		tLogfile.fwriteln( "PROBE: %s[%1d]", __FUNCTION__, __LINE__ );
//#define	P		tLogfile.fwriteln( "PROBE: %s[%1d]", __FUNCTION__, __LINE__ );
#define	PROBEx(s)	tLogfile.fwriteln( "PROBE: [%s]      | %s[%1d]", (char*)(s), __FUNCTION__, __LINE__, (char*)(s) );

#ifdef __UTOOLS_H__
//////////////////////////////////////////////////////////////////////////////////////////////////
class	XTimer
{
public:
	LOGfile*	m_pLogfile;

public:
	int tstart;
	int tcurr;
	int tprev;
	char	fname[512];
	//JTimer timer;

	static int etime()
	{
		SYSTEMTIME	systemTime;
		GetSystemTime( &systemTime );
		return ((int)systemTime.wSecond)*1000 + ((int)systemTime.wMilliseconds);
	}

	XTimer( LOGfile* pLogfile, char* functionName )
	{
		m_pLogfile = pLogfile;
		tprev = tcurr = tstart = etime();
		strcpy( fname, functionName );
		//timer.Start();
	}

	~XTimer()
	{
		tcurr = etime(); m_pLogfile->fwriteln( "TIME: %s: %1d msec", fname, tcurr-tstart );
		//m_pLogfile->fwriteln( "TIME: %lf msec", timer.Elapsed() );
	}

	void XTimerMark( UINT line )
	{
		tcurr = etime(); m_pLogfile->fwriteln( "TIME[%s(%1d)]: %1d msec: total %1d msec", fname, line, tcurr-tprev, tcurr-tstart );	tprev = tcurr;
		//m_pLogfile->fwriteln( "TIME[%1d]: %lf msec", line, timer.Elapsed() );
	}
};

#define	XTIMER			XTimer	xTimerx( &tLogfile, __FUNCTION__ );
#define XTIMERMARK		xTimerx.XTimerMark(__LINE__);
//#define	XTIMESTART	time( &tstart ); tprev = tcurr = tstart;
//#define	XTIMEMARK	time( &tcurr ); tLogfile.fwriteln( "TIME[%1d]: %lf seconds", __LINE__, difftime( tcurr, tprev ) );	tprev = tcurr;
//#define	XTIMESTOP	time( &tcurr ); m_pLogfile->fwriteln( "TOTAL-TIME[%1d]: %lf seconds", __LINE__, difftime( tcurr, tstart ) );
//#define	XTIMESTART	tprev = tcurr = tstart = etime();
//#define	XTIMEMARK	tcurr = etime(); tLogfile.fwriteln( "TIME[%1d]: %1d msec: total %1d msec", __LINE__, tcurr-tprev, tcurr-tstart );	tprev = tcurr;
//#define	XTIMESTOP	tcurr = etime(); tLogfile.fwriteln( "TIME: %1d msec", tcurr-tstart );

#define	XTIME	\
{	\
	SYSTEMTIME	systemTime;	\
	GetSystemTime( &systemTime );	\
	tLogfile.fwriteln( "TIME: %02d:%02d:%02d", systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds );	\
}

#define	XWAIT(NSEC)	\
{	\
	for( int k=0; k<(NSEC); k++ )	\
	{	\
		for( int i=0; i<1000000; i++ )	\
		{	\
			for( int j=0; j<300; j++ )	\
			{	\
			}	\
		}	\
	}	\
}
#else
static int etime()
{
	SYSTEMTIME	systemTime;
	GetSystemTime( &systemTime );
	return ((int)systemTime.wSecond)*1000 + ((int)systemTime.wMilliseconds);
}
static	int tstart;
static	int tcurr;
static	int tprev;
#define	XTIMESTART	tprev = tcurr = tstart = etime();
#define	XTIMEMARK	tcurr = etime(); tLogfile.fwriteln( "TIME[%1d]: %1d msec: total %1d msec", __LINE__, tcurr-tprev, tcurr-tstart );	tprev = tcurr;
#define	XTIMESTOP	tcurr = etime(); tLogfile.fwriteln( "TIME: %1d msec", tcurr-tstart );
#endif

class	LOGfunction
{
public:
	LOGfile*	m_pLogfile;
	char		m_functionName[512];
	char		m_fileName[512];
	int			m_count;
	int			m_line;
	int			m_start;
	int			m_end;

public:
	LOGfunction( LOGfile* pLogfile, char* functionName, int count, int line, char* file, int start, int end=1000 )
	{
		m_count = count;
		m_start = start;
		m_end = end;

		char* fname = NULL;
		if( file )
			fname = strrchr( file, '\\' );

		if( (m_count >= m_start && m_count <= m_end) || m_count == -1 )
		{
			m_pLogfile = pLogfile;
			//if( pLogfile->IsEnabled() == false ) return;
			if( functionName )
				strcpy( m_functionName, functionName );
			else
				strcpy( m_functionName, "" );

			m_line = line;

			if( functionName )
			{
				//m_pLogfile->fwriteln( "" );
				if( fname )
					m_pLogfile->fwriteln( "%s(%1d) [%1d] %s", m_functionName, m_line, m_count, fname );
				else
				if( file )
					m_pLogfile->fwriteln( "%s(%1d) [%1d] %s", m_functionName, m_line, m_count, file );
				else
				if( m_count >= 0 )
					m_pLogfile->fwriteln( "%s(%1d) [%1d]", m_functionName, m_line, m_count );
				else
					m_pLogfile->fwriteln( "%s(%1d)", m_functionName, m_line );
				m_pLogfile->addindent();

				//if( who != NULL )
				//	m_pLogfile->fwriteln( "this: %x", who );
			}
			else
			{
				m_pLogfile->fwriteln( "BLOCKENTER [%1d]", m_line );
				m_pLogfile->addindent();
			}

//			DWORD n = GetGuiResources( GetCurrentProcess(), 0 );
//			m_pLogfile->fwriteln( "GDI objects: %1d", n );
		}
	}

	~LOGfunction()
	{
//		DWORD n = GetGuiResources( GetCurrentProcess(), 0 );
//		m_pLogfile->fwriteln( "GDI objects: %1d", n );
		if( strlen(m_functionName) > 0 )
		{
			if( (m_count >= m_start && m_count <= m_end) || m_count == -1 )
			{
				//if( m_pLogfile->IsEnabled() == false ) return;
				//if( difftime( tcurr, tstart ) > 0.0 ) time( &tcurr ); m_pLogfile->fwriteln( "%lf seconds", difftime( tcurr, tstart ) );
				m_pLogfile->subindent();
				//m_pLogfile->fwriteln( "" );
			}
		}
		else
		{
//			m_pLogfile->fwriteln( "BLOCKEXIT  [%1d]", m_line );
			m_pLogfile->subindent();
		}
	}
};

static std::wstring LOGMSG( UINT msg )
{
	std::wstring	s;
//static CString LOGMSG( UINT msg )
//{
//	CString	s;
	switch( msg )
	{
//	case XTP_TTM_WINDOWFROMPOINT:	s.Format( "XTP_TTM_WINDOWFROMPOINT(0x%x)", (int)XTP_TTM_WINDOWFROMPOINT );	break;
#ifdef _WINUSER_
	case WM_NULL:					s = L"WM_NULL(0x0000)";						break;
	case WM_CREATE:					s = L"WM_CREATE(0x0001)";					break;
	case WM_DESTROY:				s = L"WM_DESTROY(0x0002)";					break;
	case WM_MOVE:					s = L"WM_MOVE(0x0003)";						break;
	case WM_SIZE:					s = L"WM_SIZE(0x0005)";						break;
	case WM_ACTIVATE:				s = L"WM_ACTIVATE(0x0006)";					break;
	case WM_SETFOCUS:				s = L"WM_SETFOCUS(0x0007)";					break;
	case WM_KILLFOCUS:				s = L"WM_KILLFOCUS(0x0008)";				break;
	case WM_ENABLE:					s = L"WM_ENABLE(0x000A)";					break;
	case WM_SETREDRAW:				s = L"WM_SETREDRAW(0x000B)";				break;
	case WM_SETTEXT:				s = L"WM_SETTEXT(0x000C)";					break;
	case WM_GETTEXT:				s = L"WM_GETTEXT(0x000D)";					break;
	case WM_GETTEXTLENGTH:			s = L"WM_GETTEXTLENGTH(0x000E)";			break;
	case WM_PAINT:					s = L"WM_PAINT(0x000F)";					break;
	case WM_CLOSE:					s = L"WM_CLOSE(0x0010)";					break;
	case WM_QUERYENDSESSION:		s = L"WM_QUERYENDSESSION(0x0011)";			break;
	case WM_QUERYOPEN:				s = L"WM_QUERYOPEN(0x0013)";				break;
	case WM_ENDSESSION:				s = L"WM_ENDSESSION(0x0016)";				break;
	case WM_QUIT:					s = L"WM_QUIT(0x0012)";						break;
	case WM_ERASEBKGND:				s = L"WM_ERASEBKGND(0x0014)";				break;
	case WM_SYSCOLORCHANGE:			s = L"WM_SYSCOLORCHANGE(0x0015)";			break;
	case WM_SHOWWINDOW:				s = L"WM_SHOWWINDOW(0x0018)";				break;
	case WM_WININICHANGE:			s = L"WM_WININICHANGE(0x001A)";				break;
	case WM_DEVMODECHANGE:			s = L"WM_DEVMODECHANGE(0x001B)";			break;
	case WM_ACTIVATEAPP:			s = L"WM_ACTIVATEAPP(0x001C)";				break;
	case WM_FONTCHANGE:				s = L"WM_FONTCHANGE(0x001D)";				break;
	case WM_TIMECHANGE:				s = L"WM_TIMECHANGE(0x001E)";				break;
	case WM_CANCELMODE:				s = L"WM_CANCELMODE(0x001F)";				break;
	case WM_SETCURSOR:				s = L"WM_SETCURSOR(0x0020)";				break;
	case WM_MOUSEACTIVATE:			s = L"WM_MOUSEACTIVATE(0x0021)";			break;
	case WM_CHILDACTIVATE:			s = L"WM_CHILDACTIVATE(0x0022)";			break;
	case WM_QUEUESYNC:				s = L"WM_QUEUESYNC(0x0023)";				break;
	case WM_GETMINMAXINFO:			s = L"WM_GETMINMAXINFO(0x0024)";			break;
	case WM_PAINTICON:				s = L"WM_PAINTICON(0x0026)";				break;
	case WM_ICONERASEBKGND:			s = L"WM_ICONERASEBKGND(0x0027)";			break;
	case WM_NEXTDLGCTL:				s = L"WM_NEXTDLGCTL(0x0028)";				break;
	case WM_SPOOLERSTATUS:			s = L"WM_SPOOLERSTATUS(0x002A)";			break;
	case WM_DRAWITEM:				s = L"WM_DRAWITEM(0x002B)";					break;
	case WM_MEASUREITEM:			s = L"WM_MEASUREITEM(0x002C)";				break;
	case WM_DELETEITEM:				s = L"WM_DELETEITEM(0x002D)";				break;
	case WM_VKEYTOITEM:				s = L"WM_VKEYTOITEM(0x002E)";				break;
	case WM_CHARTOITEM:				s = L"WM_CHARTOITEM(0x002F)";				break;
	case WM_SETFONT:				s = L"WM_SETFONT(0x0030)";					break;
	case WM_GETFONT:				s = L"WM_GETFONT(0x0031)";					break;
	case WM_SETHOTKEY:				s = L"WM_SETHOTKEY(0x0032)";				break;
	case WM_GETHOTKEY:				s = L"WM_GETHOTKEY(0x0033)";				break;
	case WM_QUERYDRAGICON:			s = L"WM_QUERYDRAGICON(0x0037)";			break;
	case WM_COMPAREITEM:			s = L"WM_COMPAREITEM(0x0039)";				break;
//	case WM_GETOBJECT:				s = L"WM_GETOBJECT(0x003D)";				break;
	case 0x003D:					s = L"WM_GETOBJECT(0x003D)";				break;
	case WM_COMPACTING:				s = L"WM_COMPACTING(0x0041)";				break;
	case WM_COMMNOTIFY:				s = L"WM_COMMNOTIFY(0x0044)";				break;
	case WM_WINDOWPOSCHANGING:		s = L"WM_WINDOWPOSCHANGING(0x0046)";		break;
	case WM_WINDOWPOSCHANGED:		s = L"WM_WINDOWPOSCHANGED(0x0047)";			break;
	case WM_POWER:					s = L"WM_POWER(0x0048)";					break;
	case WM_COPYDATA:				s = L"WM_COPYDATA(0x004A)";					break;
	case WM_CANCELJOURNAL:			s = L"WM_CANCELJOURNAL(0x004B)";			break;
	case WM_NOTIFY:					s = L"WM_NOTIFY(0x004E)";					break;
	case WM_INPUTLANGCHANGEREQUEST:	s = L"WM_INPUTLANGCHANGEREQUEST(0x0050)";	break;
	case WM_INPUTLANGCHANGE:		s = L"WM_INPUTLANGCHANGE(0x0051)";			break;
	case WM_TCARD:					s = L"WM_TCARD(0x0052)";					break;
	case WM_HELP:					s = L"WM_HELP(0x0053)";						break;
	case WM_USERCHANGED:			s = L"WM_USERCHANGED(0x0054)";				break;
	case WM_NOTIFYFORMAT:			s = L"WM_NOTIFYFORMAT(0x0055)";				break;
	case WM_CONTEXTMENU:			s = L"WM_CONTEXTMENU(0x007B)";				break;
	case WM_STYLECHANGING:			s = L"WM_STYLECHANGING(0x007C)";			break;
	case WM_STYLECHANGED:			s = L"WM_STYLECHANGED(0x007D)";				break;
	case WM_DISPLAYCHANGE:			s = L"WM_DISPLAYCHANGE(0x007E)";			break;
	case WM_GETICON:				s = L"WM_GETICON(0x007F)";					break;
	case WM_SETICON:				s = L"WM_SETICON(0x0080)";					break;
	case WM_NCCREATE:				s = L"WM_NCCREATE(0x0081)";					break;
	case WM_NCDESTROY:				s = L"WM_NCDESTROY(0x0082)";				break;
	case WM_NCCALCSIZE:				s = L"WM_NCCALCSIZE(0x0083)";				break;
	case WM_NCHITTEST:				s = L"WM_NCHITTEST(0x0084)";				break;
	case WM_NCPAINT:				s = L"WM_NCPAINT(0x0085)";					break;
	case WM_NCACTIVATE:				s = L"WM_NCACTIVATE(0x0086)";				break;
	case WM_GETDLGCODE:				s = L"WM_GETDLGCODE(0x0087)";				break;
	case WM_SYNCPAINT:				s = L"WM_SYNCPAINT(0x0088)";				break;
	case WM_NCMOUSEMOVE:			s = L"WM_NCMOUSEMOVE(0x00A0)";				break;
	case WM_NCLBUTTONDOWN:			s = L"WM_NCLBUTTONDOWN(0x00A1)";			break;
	case WM_NCLBUTTONUP:			s = L"WM_NCLBUTTONUP(0x00A2)";				break;
	case WM_NCLBUTTONDBLCLK:		s = L"WM_NCLBUTTONDBLCLK(0x00A3)";			break;
	case WM_NCRBUTTONDOWN:			s = L"WM_NCRBUTTONDOWN(0x00A4)";			break;
	case WM_NCRBUTTONUP:			s = L"WM_NCRBUTTONUP(0x00A5)";				break;
	case WM_NCRBUTTONDBLCLK:		s = L"WM_NCRBUTTONDBLCLK(0x00A6)";			break;
	case WM_NCMBUTTONDOWN:			s = L"WM_NCMBUTTONDOWN(0x00A7)";			break;
	case WM_NCMBUTTONUP:			s = L"WM_NCMBUTTONUP(0x00A8)";				break;
	case WM_NCMBUTTONDBLCLK:		s = L"WM_NCMBUTTONDBLCLK(0x00A9)";			break;
//	case WM_NCXBUTTONDOWN:			s = L"WM_NCXBUTTONDOWN(0x00AB)";			break;
	case 0x00AB:					s = L"WM_NCXBUTTONDOWN(0x00AB)";			break;
//	case WM_NCXBUTTONUP:			s = L"WM_NCXBUTTONUP(0x00AC)";				break;
	case 0x00AC:					s = L"WM_NCXBUTTONUP(0x00AC)";				break;
//	case WM_NCXBUTTONDBLCLK:		s = L"WM_NCXBUTTONDBLCLK(0x00AD)";			break;
	case 0x00AD:					s = L"WM_NCXBUTTONDBLCLK(0x00AD)";			break;
//	case WM_INPUT:					s = L"WM_INPUT(0x00FF)";					break;
	case 0x00FF:					s = L"WM_INPUT(0x00FF)";					break;
	case WM_KEYDOWN:				s = L"WM_KEYDOWN(0x0100)";					break;
	case WM_KEYUP:					s = L"WM_KEYUP(0x0101)";					break;
	case WM_CHAR:					s = L"WM_CHAR(0x0102)";						break;
	case WM_DEADCHAR:				s = L"WM_DEADCHAR(0x0103)";					break;
	case WM_SYSKEYDOWN:				s = L"WM_SYSKEYDOWN(0x0104)";				break;
	case WM_SYSKEYUP:				s = L"WM_SYSKEYUP(0x0105)";					break;
	case WM_SYSCHAR:				s = L"WM_SYSCHAR(0x0106)";					break;
	case WM_SYSDEADCHAR:			s = L"WM_SYSDEADCHAR(0x0107)";				break;
	case WM_UNICHAR:				s = L"WM_UNICHAR(0x0109)";					break;
//	case WM_KEYLAST:				s = L"WM_KEYLAST(0x0108)";					break;
	case 0x0108:					s = L"WM_KEYLAST(0x0108)";					break;
	case WM_IME_STARTCOMPOSITION:	s = L"WM_IME_STARTCOMPOSITION(0x010D)";		break;
	case WM_IME_ENDCOMPOSITION:		s = L"WM_IME_ENDCOMPOSITION(0x010E)";		break;
	case WM_IME_KEYLAST:			s = L"WM_IME_KEYLAST(0x010F)";				break;
	case WM_INITDIALOG:				s = L"WM_INITDIALOG(0x0110)";				break;
	case WM_COMMAND:				s = L"WM_COMMAND(0x0111)";					break;
	case WM_SYSCOMMAND:				s = L"WM_SYSCOMMAND(0x0112)";				break;
	case WM_TIMER:					s = L"WM_TIMER(0x0113)";					break;
	case WM_HSCROLL:				s = L"WM_HSCROLL(0x0114)";					break;
	case WM_VSCROLL:				s = L"WM_VSCROLL(0x0115)";					break;
	case WM_INITMENU:				s = L"WM_INITMENU(0x0116)";					break;
	case WM_INITMENUPOPUP:			s = L"WM_INITMENUPOPUP(0x0117)";			break;
	case WM_MENUSELECT:				s = L"WM_MENUSELECT(0x011F)";				break;
	case WM_MENUCHAR:				s = L"WM_MENUCHAR(0x0120)";					break;
	case WM_ENTERIDLE:				s = L"WM_ENTERIDLE(0x0121)";				break;
//	case WM_MENURBUTTONUP:			s = L"WM_MENURBUTTONUP(0x0122)";			break;
	case 0x0122:					s = L"WM_MENURBUTTONUP(0x0122)";			break;
//	case WM_MENUDRAG:				s = L"WM_MENUDRAG(0x0123)";					break;
	case 0x0123:					s = L"WM_MENUDRAG(0x0123)";					break;
//	case WM_MENUGETOBJECT:			s = L"WM_MENUGETOBJECT(0x0124)";			break;
	case 0x0124:					s = L"WM_MENUGETOBJECT(0x0124)";			break;
//	case WM_UNINITMENUPOPUP:		s = L"WM_UNINITMENUPOPUP(0x0125)";			break;
	case 0x0125:					s = L"WM_UNINITMENUPOPUP(0x0125)";			break;
//	case WM_MENUCOMMAND:			s = L"WM_MENUCOMMAND(0x0126)";				break;
	case 0x0126:					s = L"WM_MENUCOMMAND(0x0126)";				break;
//	case WM_CHANGEUISTATE:			s = L"WM_CHANGEUISTATE(0x0127)";			break;
	case 0x0127:					s = L"WM_CHANGEUISTATE(0x0127)";			break;
//	case WM_UPDATEUISTATE:			s = L"WM_UPDATEUISTATE(0x0128)";			break;
	case 0x0128:					s = L"WM_UPDATEUISTATE(0x0128)";			break;
//	case WM_QUERYUISTATE:			s = L"WM_QUERYUISTATE(0x0129)";				break;
	case 0x0129:					s = L"WM_QUERYUISTATE(0x0129)";				break;
	case WM_CTLCOLORMSGBOX:			s = L"WM_CTLCOLORMSGBOX(0x0132)";			break;
	case WM_CTLCOLOREDIT:			s = L"WM_CTLCOLOREDIT(0x0133)";				break;
	case WM_CTLCOLORLISTBOX:		s = L"WM_CTLCOLORLISTBOX(0x0134)";			break;
	case WM_CTLCOLORBTN:			s = L"WM_CTLCOLORBTN(0x0135)";				break;
	case WM_CTLCOLORDLG:			s = L"WM_CTLCOLORDLG(0x0136)";				break;
	case WM_CTLCOLORSCROLLBAR:		s = L"WM_CTLCOLORSCROLLBAR(0x0137)";		break;
	case WM_CTLCOLORSTATIC:			s = L"WM_CTLCOLORSTATIC(0x0138)";			break;
	case MN_GETHMENU:				s = L"MN_GETHMENU(0x01E1)";					break;
	case WM_MOUSEMOVE:				s = L"WM_MOUSEMOVE(0x0200)";				break;
	case WM_LBUTTONDOWN:			s = L"WM_LBUTTONDOWN(0x0201)";				break;
	case WM_LBUTTONUP:				s = L"WM_LBUTTONUP(0x0202)";				break;
	case WM_LBUTTONDBLCLK:			s = L"WM_LBUTTONDBLCLK(0x0203)";			break;
	case WM_RBUTTONDOWN:			s = L"WM_RBUTTONDOWN(0x0204)";				break;
	case WM_RBUTTONUP:				s = L"WM_RBUTTONUP(0x0205)";				break;
	case WM_RBUTTONDBLCLK:			s = L"WM_RBUTTONDBLCLK(0x0206)";			break;
	case WM_MBUTTONDOWN:			s = L"WM_MBUTTONDOWN(0x0207)";				break;
	case WM_MBUTTONUP:				s = L"WM_MBUTTONUP(0x0208)";				break;
//	case WM_MOUSELAST:				s = L"WM_MOUSELAST(0x0209)";				break;
	case WM_MBUTTONDBLCLK:			s = L"WM_MBUTTONDBLCLK(0x0209)";			break;
	case WM_MOUSEWHEEL:				s = L"WM_MOUSEWHEEL(0x020A)";				break;
//	case WM_XBUTTONDOWN:			s = L"WM_XBUTTONDOWN(0x020B)";				break;
	case 0x020B:					s = L"WM_XBUTTONDOWN(0x020B)";				break;
//	case WM_XBUTTONUP:				s = L"WM_XBUTTONUP(0x020C)";				break;
	case 0x020C:					s = L"WM_XBUTTONUP(0x020C)";				break;
//	case WM_XBUTTONDBLCLK:			s = L"WM_XBUTTONDBLCLK(0x020D)";			break;
	case 0x020D:					s = L"WM_XBUTTONDBLCLK(0x020D)";			break;
	case WM_PARENTNOTIFY:			s = L"WM_PARENTNOTIFY(0x0210)";				break;
	case WM_ENTERMENULOOP:			s = L"WM_ENTERMENULOOP(0x0211)";			break;
	case WM_EXITMENULOOP:			s = L"WM_EXITMENULOOP(0x0212)";				break;
	case WM_NEXTMENU:				s = L"WM_NEXTMENU(0x0213)";					break;
	case WM_SIZING:					s = L"WM_SIZING(0x0214)";					break;
	case WM_CAPTURECHANGED:			s = L"WM_CAPTURECHANGED(0x0215)";			break;
	case WM_MOVING:					s = L"WM_MOVING(0x0216)";					break;
	case WM_POWERBROADCAST:			s = L"WM_POWERBROADCAST(0x0218)";			break;
	case WM_DEVICECHANGE:			s = L"WM_DEVICECHANGE(0x0219)";				break;
	case WM_MDICREATE:				s = L"WM_MDICREATE(0x0220)";				break;
	case WM_MDIDESTROY:				s = L"WM_MDIDESTROY(0x0221)";				break;
	case WM_MDIACTIVATE:			s = L"WM_MDIACTIVATE(0x0222)";				break;
	case WM_MDIRESTORE:				s = L"WM_MDIRESTORE(0x0223)";				break;
	case WM_MDINEXT:				s = L"WM_MDINEXT(0x0224)";					break;
	case WM_MDIMAXIMIZE:			s = L"WM_MDIMAXIMIZE(0x0225)";				break;
	case WM_MDITILE:				s = L"WM_MDITILE(0x0226)";					break;
	case WM_MDICASCADE:				s = L"WM_MDICASCADE(0x0227)";				break;
	case WM_MDIICONARRANGE:			s = L"WM_MDIICONARRANGE(0x0228)";			break;
	case WM_MDIGETACTIVE:			s = L"WM_MDIGETACTIVE(0x0229)";				break;
	case WM_MDISETMENU:				s = L"WM_MDISETMENU(0x0230)";				break;
	case WM_ENTERSIZEMOVE:			s = L"WM_ENTERSIZEMOVE(0x0231)";			break;
	case WM_EXITSIZEMOVE:			s = L"WM_EXITSIZEMOVE(0x0232)";				break;
	case WM_DROPFILES:				s = L"WM_DROPFILES(0x0233)";				break;
	case WM_MDIREFRESHMENU:			s = L"WM_MDIREFRESHMENU(0x0234)";			break;
	case WM_IME_SETCONTEXT:			s = L"WM_IME_SETCONTEXT(0x0281)";			break;
	case WM_IME_NOTIFY:				s = L"WM_IME_NOTIFY(0x0282)";				break;
	case WM_IME_CONTROL:			s = L"WM_IME_CONTROL(0x0283)";				break;
	case WM_IME_COMPOSITIONFULL:	s = L"WM_IME_COMPOSITIONFULL(0x0284)";		break;
	case WM_IME_SELECT:				s = L"WM_IME_SELECT(0x0285)";				break;
	case WM_IME_CHAR:				s = L"WM_IME_CHAR(0x0286)";					break;
//	case WM_IME_REQUEST:			s = L"WM_IME_REQUEST(0x0288)";				break;
	case 0x0288:					s = L"WM_IME_REQUEST(0x0288)";				break;
	case WM_IME_KEYDOWN:			s = L"WM_IME_KEYDOWN(0x0290)";				break;
	case WM_IME_KEYUP:				s = L"WM_IME_KEYUP(0x0291)";				break;
	case WM_MOUSEHOVER:				s = L"WM_MOUSEHOVER(0x02A1)";				break;
	case WM_MOUSELEAVE:				s = L"WM_MOUSELEAVE(0x02A3)";				break;
//	case WM_NCMOUSEHOVER:			s = L"WM_NCMOUSEHOVER(0x02A0)";				break;
	case 0x02A0:					s = L"WM_NCMOUSEHOVER(0x02A0)";				break;
//	case WM_NCMOUSELEAVE:			s = L"WM_NCMOUSELEAVE(0x02A2)";				break;
	case 0x02A2:					s = L"WM_NCMOUSELEAVE(0x02A2)";				break;
//	case WM_WTSSESSION_CHANGE:		s = L"WM_WTSSESSION_CHANGE(0x02B1)";		break;
	case 0x02B1:					s = L"WM_WTSSESSION_CHANGE(0x02B1)";		break;
//	case WM_TABLET_FIRST:			s = L"WM_TABLET_FIRST(0x02c0)";				break;
	case 0x02c0:					s = L"WM_TABLET_FIRST(0x02c0)";				break;
//	case WM_TABLET_LAST:			s = L"WM_TABLET_LAST(0x02df)";				break;
	case 0x02df:					s = L"WM_TABLET_LAST(0x02df)";				break;
	case WM_CUT:					s = L"WM_CUT(0x0300)";						break;
	case WM_COPY:					s = L"WM_COPY(0x0301)";						break;
	case WM_PASTE:					s = L"WM_PASTE(0x0302)";					break;
	case WM_CLEAR:					s = L"WM_CLEAR(0x0303)";					break;
	case WM_UNDO:					s = L"WM_UNDO(0x0304)";						break;
	case WM_RENDERFORMAT:			s = L"WM_RENDERFORMAT(0x0305)";				break;
	case WM_RENDERALLFORMATS:		s = L"WM_RENDERALLFORMATS(0x0306)";			break;
	case WM_DESTROYCLIPBOARD:		s = L"WM_DESTROYCLIPBOARD(0x0307)";			break;
	case WM_DRAWCLIPBOARD:			s = L"WM_DRAWCLIPBOARD(0x0308)";			break;
	case WM_PAINTCLIPBOARD:			s = L"WM_PAINTCLIPBOARD(0x0309)";			break;
	case WM_VSCROLLCLIPBOARD:		s = L"WM_VSCROLLCLIPBOARD(0x030A)";			break;
	case WM_SIZECLIPBOARD:			s = L"WM_SIZECLIPBOARD(0x030B)";			break;
	case WM_ASKCBFORMATNAME:		s = L"WM_ASKCBFORMATNAME(0x030C)";			break;
	case WM_CHANGECBCHAIN:			s = L"WM_CHANGECBCHAIN(0x030D)";			break;
	case WM_HSCROLLCLIPBOARD:		s = L"WM_HSCROLLCLIPBOARD(0x030E)";			break;
	case WM_QUERYNEWPALETTE:		s = L"WM_QUERYNEWPALETTE(0x030F)";			break;
	case WM_PALETTEISCHANGING:		s = L"WM_PALETTEISCHANGING(0x0310)";		break;
	case WM_PALETTECHANGED:			s = L"WM_PALETTECHANGED(0x0311)";			break;
	case WM_HOTKEY:					s = L"WM_HOTKEY(0x0312)";					break;
	case WM_PRINT:					s = L"WM_PRINT(0x0317)";					break;
	case WM_PRINTCLIENT:			s = L"WM_PRINTCLIENT(0x0318)";				break;
//	case WM_APPCOMMAND:				s = L"WM_APPCOMMAND(0x0319)";				break;
	case 0x0319:					s = L"WM_APPCOMMAND(0x0319)";				break;
//	case WM_THEMECHANGED:			s = L"WM_THEMECHANGED(0x031A)";				break;
	case 0x031A:					s = L"WM_THEMECHANGED(0x031A)";				break;
	case WM_HANDHELDFIRST:			s = L"WM_HANDHELDFIRST(0x0358)";			break;
	case WM_HANDHELDLAST:			s = L"WM_HANDHELDLAST(0x035F)";				break;
	case WM_AFXFIRST:				s = L"WM_AFXFIRST(0x0360)";					break;
	case WM_AFXLAST:				s = L"WM_AFXLAST(0x037F)";					break;
	case WM_PENWINFIRST:			s = L"WM_PENWINFIRST(0x0380)";				break;
	case WM_PENWINLAST:				s = L"WM_PENWINLAST(0x038F)";				break;
#endif
	default:
#ifdef __UWINDOWS_H__
//	if( msg == WM_ACTIVATEPART_COMMANDCOMPLETED )	s.Format( _T("WM_ACTIVATEPART_COMMANDCOMPLETED(0x%x)"),	msg );	else
//	if( msg == WM_AUTO_BACKUPSAVE_CLEANUP )			s.Format( _T("WM_AUTO_BACKUPSAVE_CLEANUP(0x%x)"),		msg );	else
//	if( msg == WM_AUTOMATION_COMMAND )				s.Format( _T("WM_AUTOMATION_COMMAND(0x%x)"),			msg );	else
//	if( msg == WM_BEGIN_VIEWTRANSFORM )				s.Format( _T("WM_BEGIN_VIEWTRANSFORM(0x%x)"),			msg );	else
//	if( msg == WM_DMHOLE_UPDATE_FEATURE )			s.Format( _T("WM_DMHOLE_UPDATE_FEATURE(0x%x)"),			msg );	else
//	if( msg == WM_DTA_CHECK_COMMAND_ACTIVE )		s.Format( _T("WM_DTA_CHECK_COMMAND_ACTIVE(0x%x)"),		msg );	else
//	if( msg == WM_DTA_CHECK_NAME )					s.Format( _T("WM_DTA_CHECK_NAME(0x%x)"),				msg );	else
//	if( msg == WM_DTA_EMPTY_SELECT_LIST )			s.Format( _T("WM_DTA_EMPTY_SELECT_LIST(0x%x)"),			msg );	else
//	if( msg == WM_DTA_UPDATE_TABLE )				s.Format( _T("WM_DTA_UPDATE_TABLE(0x%x)"),				msg );	else
//	if( msg == WM_END_VIEWTRANSFORM )				s.Format( _T("WM_END_VIEWTRANSFORM(0x%x)"),				msg );	else
//	if( msg == WM_ENDPROFILE_COMMAND_COMPLETED )	s.Format( _T("WM_ENDPROFILE_COMMAND_COMPLETED(0x%x)"),	msg );	else
//	if( msg == WM_FOA_DISABLE_CONTROLS )			s.Format( _T("WM_FOA_DISABLE_CONTROLS(0x%x)"),			msg );	else
//	if( msg == WM_FOA_REENABLE_CONTROLS )			s.Format( _T("WM_FOA_REENABLE_CONTROLS(0x%x)"),			msg );	else
//	if( msg == WM_INIT_TOOLTIPCONTEXT )				s.Format( _T("WM_INIT_TOOLTIPCONTEXT(0x%x)"),			msg );	else
//	if( msg == WM_IS_SOLIDEDGE )					s.Format( _T("WM_IS_SOLIDEDGE(0x%x)"),					msg );	else
//	if( msg == WM_JACTIVEENVIRONMENTTRANSITION )	s.Format( _T("WM_JACTIVEENVIRONMENTTRANSITION(0x%x)"),	msg );	else
//	if( msg == WM_JACTIVESHEETTRANSITION )			s.Format( _T("WM_JACTIVESHEETTRANSITION(0x%x)"),		msg );	else
//	if( msg == WM_JCONTEXTHELPBYHELPID )			s.Format( _T("WM_JCONTEXTHELPBYHELPID(0x%x)"),			msg );	else
//	if( msg == WM_JCOPILOT )						s.Format( _T("WM_JCOPILOT(0x%x)"),						msg );	else
//	if( msg == WM_JDOCMARKEDEDITOPEN )				s.Format( _T("WM_JDOCMARKEDEDITOPEN(0x%x)"),			msg );	else
//	if( msg == WM_JDOCUMENTTRANSITION )				s.Format( _T("WM_JDOCUMENTTRANSITION(0x%x)"),			msg );	else
//	if( msg == WM_JENABLEAUTOMATION )				s.Format( _T("WM_JENABLEAUTOMATION(0x%x)"),				msg );	else
//	if( msg == WM_JENABLEHELPAUTOMATION )			s.Format( _T("WM_JENABLEHELPAUTOMATION(0x%x)"),			msg );	else
//	if( msg == WM_JENVIRONMENTTRANSITION )			s.Format( _T("WM_JENVIRONMENTTRANSITION(0x%x)"),		msg );	else
//	if( msg == WM_JIDLE )							s.Format( _T("WM_JIDLE(0x%x)"),							msg );	else
//	if( msg == WM_JPROCESSDDEREQUESTS )				s.Format( _T("WM_JPROCESSDDEREQUESTS(0x%x)"),			msg );	else
//	if( msg == WM_JQUERYAUTOMATION )				s.Format( _T("WM_JQUERYAUTOMATION(0x%x)"),				msg );	else
//	if( msg == WM_JQUERYHELPAUTOMATION )			s.Format( _T("WM_JQUERYHELPAUTOMATION(0x%x)"),			msg );	else
//	if( msg == WM_JSELECTSETCHANGE )				s.Format( _T("WM_JSELECTSETCHANGE(0x%x)"),				msg );	else
//	if( msg == WM_JSETFRAMEMENU )					s.Format( _T("WM_JSETFRAMEMENU(0x%x)"),					msg );	else
//	if( msg == WM_JSTARTSPECIFICHELPTUTORIAL )		s.Format( _T("WM_JSTARTSPECIFICHELPTUTORIAL(0x%x)"),	msg );	else
//	if( msg == WM_JVA )								s.Format( _T("WM_JVA(0x%x)"),							msg );	else
//	if( msg == WM_JVARIABLECHANGE )					s.Format( _T("WM_JVARIABLECHANGE(0x%x)"),				msg );	else
//	if( msg == WM_JVIEW_UPDATED_WITH_MOUSEBUTTON )	s.Format( _T("WM_JVIEW_UPDATED_WITH_MOUSEBUTTON(0x%x)"),msg );	else
//	if( msg == WM_JVIEWSETTRANSITION )				s.Format( _T("WM_JVIEWSETTRANSITION(0x%x)"),			msg );	else
//	if( msg == WM_JVK )								s.Format( _T("WM_JVK(0x%x)"),							msg );	else
//	if( msg == WM_MEASUREMENT_COMMANDCOMPLETED )	s.Format( _T("WM_MEASUREMENT_COMMANDCOMPLETED(0x%x)"),	msg );	else
//	if( msg == WM_MEASUREMENT_VARIABLE_CREATED )	s.Format( _T("WM_MEASUREMENT_VARIABLE_CREATED(0x%x)"),	msg );	else
//	if( msg == WM_MODIFYBALLOON_CMDCOMPLETED )		s.Format( _T("WM_MODIFYBALLOON_CMDCOMPLETED(0x%x)"),	msg );	else
//	if( msg == WM_MODIFYBALLOON_CMDSTARTED )		s.Format( _T("WM_MODIFYBALLOON_CMDSTARTED(0x%x)"),		msg );	else
//	if( msg == WM_MODIFYBALLOON_NOPOPUP )			s.Format( _T("WM_MODIFYBALLOON_NOPOPUP(0x%x)"),			msg );	else
//	if( msg == WM_OPTIONVALUE_MODIFIED )			s.Format( _T("WM_OPTIONVALUE_MODIFIED(0x%x)"),			msg );	else
//	if( msg == WM_REFRESH_SELECT_LIST )				s.Format( _T("WM_REFRESH_SELECT_LIST(0x%x)"),			msg );	else
//	if( msg == WM_RUNNING_LOW_ON_MEMORY )			s.Format( _T("WM_RUNNING_LOW_ON_MEMORY(0x%x)"),			msg );	else
	if( msg == WM_SCREENUPDATING )					s = L"WM_SCREENUPDATING";	else
//	if( msg == WM_SE_LOW_MEMORY_WARNING )			s.Format( _T("WM_SE_LOW_MEMORY_WARNING(0x%x)"),			msg );	else
//	if( msg == WM_SECRETAGENTMAN )					s.Format( _T("WM_SECRETAGENTMAN(0x%x)"),				msg );	else
//	if( msg == WM_START_DIM_TEXT_EDIT )				s.Format( _T("WM_START_DIM_TEXT_EDIT(0x%x)"),			msg );	else
//	if( msg == WM_TANGENT_REF_PLANE_REFRESH )		s.Format( _T("WM_TANGENT_REF_PLANE_REFRESH(0x%x)"),		msg );	else
//	if( msg == WM_TERMINAL_BALLOON_MODIFIED )		s.Format( _T("WM_TERMINAL_BALLOON_MODIFIED(0x%x)"),		msg );	else
//	if( msg == WM_VIEW_TRANSFORM_CHANGED )			s.Format( _T("WM_VIEW_TRANSFORM_CHANGED(0x%x)"),		msg );	else
	if( msg == WM_VWBACTIVATE )						s = L"WM_VWBACTIVATE";	else
	if( msg == WM_VWBDEACTIVATE )					s = L"WM_VWBDEACTIVATE";	else
	if( msg == WM_VWBDRAWHANDLES )					s = L"WM_VWBDRAWHANDLES";	else
	if( msg == WM_VWBMOUSEACTIVATE )				s = L"WM_VWBMOUSEACTIVATE";	else
	if( msg == WM_VWBSCANSELECTSET )				s = L"WM_VWBSCANSELECTSET";	else
	if( msg == WM_VWBSLEEP )						s = L"WM_VWBSLEEP";	else
	if( msg == WM_VWBTRANSITION )					s = L"WM_VWBTRANSITION";	else
	if( msg == WM_VWBWAKEUP )						s = L"WM_VWBWAKEUP";	else
//	if( msg == WM_WIREPATH_COMMANDCOMPLETED )		s = L"WM_WIREPATH_COMMANDCOMPLETED(0x%x)"),		msg );	else
#endif
	if( msg == 0x0360 )	s = L"WM_QUERYAFXWNDPROC";		else
	if( msg == 0x0361 )	s = L"WM_SIZEPARENT";			else
	if( msg == 0x0362 )	s = L"WM_SETMESSAGESTRING";		else
	if( msg == 0x0363 )	s = L"WM_IDLEUPDATECMDUI";		else
	if( msg == 0x0364 )	s = L"WM_INITIALUPDATE";			else
	if( msg == 0x0365 )	s = L"WM_COMMANDHELP";			else
	if( msg == 0x0366 )	s = L"WM_HELPHITTEST";			else
	if( msg == 0x0367 )	s = L"WM_EXITHELPMODE";			else
	if( msg == 0x0368 )	s = L"WM_RECALCPARENT";			else
	if( msg == 0x0369 )	s = L"WM_SIZECHILD";				else
	if( msg == 0x036A )	s = L"WM_KICKIDLE";				else
	if( msg == 0x036B )	s = L"WM_QUERYCENTERWND";		else
	if( msg == 0x036C )	s = L"WM_DISABLEMODAL";			else
	if( msg == 0x036D )	s = L"WM_FLOATSTATUS";			else
	if( msg == 0x036E )	s = L"WM_ACTIVATETOPLEVEL";		else
	if( msg == 0x036F )	s = L"WM_RESERVED_036F";			else
	if( msg == 0x0370 )	s = L"WM_RESERVED_0370";			else
	if( msg == 0x0371 )	s = L"WM_RESERVED_0371";			else
	if( msg == 0x0372 )	s = L"WM_RESERVED_0372";			else
	if( msg == 0x0373 )	s = L"WM_SOCKET_NOTIFY";			else
	if( msg == 0x0374 )	s = L"WM_SOCKET_DEAD";			else
	if( msg == 0x0375 )	s = L"WM_POPMESSAGESTRING";		else
	if( msg == 0x0376 )	s = L"WM_HELPPROMPTADDR";		else
	if( msg == 0x0376 )	s = L"WM_OCC_LOADFROMSTREAM";	else
	if( msg == 0x0377 )	s = L"WM_OCC_LOADFROMSTORAGE";	else
	if( msg == 0x0378 )	s = L"WM_OCC_INITNEW";			else
	if( msg == 0x037A )	s = L"WM_OCC_LOADFROMSTREAM_EX";	else
	if( msg == 0x037B )	s = L"WM_OCC_LOADFROMSTORAGE_EX";	else
	if( msg == 0x0379 )	s = L"WM_QUEUE_SENTINEL";		else
	if( msg == 0x037C )	s = L"WM_RESERVED_037C";			else
	if( msg == 0x037D )	s = L"WM_RESERVED_037D";			else
	if( msg == 0x037E )	s = L"WM_RESERVED_037E";			else
	if( msg == 0x037F )	s = L"WM_FORWARDMSG";			else
			s = L"Unknown";
		break;
	}

	return s;
}

static CString MAPVK(UINT vk)
{
	if(vk == VK_LBUTTON) return L"VK_LBUTTON";
	if(vk == VK_RBUTTON) return L"VK_RBUTTON";
	if(vk == VK_CANCEL) return L"VK_CANCEL";
	if(vk == VK_MBUTTON) return L"VK_MBUTTON";
	if(vk == VK_XBUTTON1) return L"VK_XBUTTON1";
	if(vk == VK_XBUTTON2) return L"VK_XBUTTON2";
	if(vk == VK_BACK) return L"VK_BACK";
	if(vk == VK_TAB) return L"VK_TAB";
	if(vk == VK_CLEAR) return L"VK_CLEAR";
	if(vk == VK_RETURN) return L"VK_RETURN";
	if(vk == VK_SHIFT) return L"VK_SHIFT";
	if(vk == VK_CONTROL) return L"VK_CONTROL";
	if(vk == VK_MENU) return L"VK_MENU";
	if(vk == VK_PAUSE) return L"VK_PAUSE";
	if(vk == VK_CAPITAL) return L"VK_CAPITAL";
	if(vk == VK_KANA) return L"VK_KANA";
	if(vk == VK_HANGEUL) return L"VK_HANGEUL";
	if(vk == VK_HANGUL) return L"VK_HANGUL";
	if(vk == VK_JUNJA) return L"VK_JUNJA";
	if(vk == VK_FINAL) return L"VK_FINAL";
	if(vk == VK_HANJA) return L"VK_HANJA";
	if(vk == VK_KANJI) return L"VK_KANJI";
	if(vk == VK_ESCAPE) return L"VK_ESCAPE";
	if(vk == VK_CONVERT) return L"VK_CONVERT";
	if(vk == VK_NONCONVERT) return L"VK_NONCONVERT";
	if(vk == VK_ACCEPT) return L"VK_ACCEPT";
	if(vk == VK_MODECHANGE) return L"VK_MODECHANGE";
	if(vk == VK_SPACE) return L"VK_SPACE";
	if(vk == VK_PRIOR) return L"VK_PRIOR";
	if(vk == VK_NEXT) return L"VK_NEXT";
	if(vk == VK_END) return L"VK_END";
	if(vk == VK_HOME) return L"VK_HOME";
	if(vk == VK_LEFT) return L"VK_LEFT";
	if(vk == VK_UP) return L"VK_UP";
	if(vk == VK_RIGHT) return L"VK_RIGHT";
	if(vk == VK_DOWN) return L"VK_DOWN";
	if(vk == VK_SELECT) return L"VK_SELECT";
	if(vk == VK_PRINT) return L"VK_PRINT";
	if(vk == VK_EXECUTE) return L"VK_EXECUTE";
	if(vk == VK_SNAPSHOT) return L"VK_SNAPSHOT";
	if(vk == VK_INSERT) return L"VK_INSERT";
	if(vk == VK_DELETE) return L"VK_DELETE";
	if(vk == VK_HELP) return L"VK_HELP";
	if(vk == VK_LWIN) return L"VK_LWIN";
	if(vk == VK_RWIN) return L"VK_RWIN";
	if(vk == VK_APPS) return L"VK_APPS";
	if(vk == VK_SLEEP) return L"VK_SLEEP";
	if(vk == VK_NUMPAD0) return L"VK_NUMPAD0";
	if(vk == VK_NUMPAD1) return L"VK_NUMPAD1";
	if(vk == VK_NUMPAD2) return L"VK_NUMPAD2";
	if(vk == VK_NUMPAD3) return L"VK_NUMPAD3";
	if(vk == VK_NUMPAD4) return L"VK_NUMPAD4";
	if(vk == VK_NUMPAD5) return L"VK_NUMPAD5";
	if(vk == VK_NUMPAD6) return L"VK_NUMPAD6";
	if(vk == VK_NUMPAD7) return L"VK_NUMPAD7";
	if(vk == VK_NUMPAD8) return L"VK_NUMPAD8";
	if(vk == VK_NUMPAD9) return L"VK_NUMPAD9";
	if(vk == VK_MULTIPLY) return L"VK_MULTIPLY";
	if(vk == VK_ADD) return L"VK_ADD";
	if(vk == VK_SEPARATOR) return L"VK_SEPARATOR";
	if(vk == VK_SUBTRACT) return L"VK_SUBTRACT";
	if(vk == VK_DECIMAL) return L"VK_DECIMAL";
	if(vk == VK_DIVIDE) return L"VK_DIVIDE";
	if(vk == VK_F1) return L"VK_F1";
	if(vk == VK_F2) return L"VK_F2";
	if(vk == VK_F3) return L"VK_F3";
	if(vk == VK_F4) return L"VK_F4";
	if(vk == VK_F5) return L"VK_F5";
	if(vk == VK_F6) return L"VK_F6";
	if(vk == VK_F7) return L"VK_F7";
	if(vk == VK_F8) return L"VK_F8";
	if(vk == VK_F9) return L"VK_F9";
	if(vk == VK_F10) return L"VK_F10";
	if(vk == VK_F11) return L"VK_F11";
	if(vk == VK_F12) return L"VK_F12";
	if(vk == VK_F13) return L"VK_F13";
	if(vk == VK_F14) return L"VK_F14";
	if(vk == VK_F15) return L"VK_F15";
	if(vk == VK_F16) return L"VK_F16";
	if(vk == VK_F17) return L"VK_F17";
	if(vk == VK_F18) return L"VK_F18";
	if(vk == VK_F19) return L"VK_F19";
	if(vk == VK_F20) return L"VK_F20";
	if(vk == VK_F21) return L"VK_F21";
	if(vk == VK_F22) return L"VK_F22";
	if(vk == VK_F23) return L"VK_F23";
	if(vk == VK_F24) return L"VK_F24";
	if(vk == VK_NUMLOCK) return L"VK_NUMLOCK";
	if(vk == VK_SCROLL) return L"VK_SCROLL";
	if(vk == VK_OEM_NEC_EQUAL) return L"VK_OEM_NEC_EQUAL";
	if(vk == VK_OEM_FJ_JISHO) return L"VK_OEM_FJ_JISHO";
	if(vk == VK_OEM_FJ_MASSHOU) return L"VK_OEM_FJ_MASSHOU";
	if(vk == VK_OEM_FJ_TOUROKU) return L"VK_OEM_FJ_TOUROKU";
	if(vk == VK_OEM_FJ_LOYA) return L"VK_OEM_FJ_LOYA";
	if(vk == VK_OEM_FJ_ROYA) return L"VK_OEM_FJ_ROYA";
	if(vk == VK_LSHIFT) return L"VK_LSHIFT";
	if(vk == VK_RSHIFT) return L"VK_RSHIFT";
	if(vk == VK_LCONTROL) return L"VK_LCONTROL";
	if(vk == VK_RCONTROL) return L"VK_RCONTROL";
	if(vk == VK_LMENU) return L"VK_LMENU";
	if(vk == VK_RMENU) return L"VK_RMENU";
	if(vk == VK_BROWSER_BACK) return L"VK_BROWSER_BACK";
	if(vk == VK_BROWSER_FORWARD) return L"VK_BROWSER_FORWARD";
	if(vk == VK_BROWSER_REFRESH) return L"VK_BROWSER_REFRESH";
	if(vk == VK_BROWSER_STOP) return L"VK_BROWSER_STOP";
	if(vk == VK_BROWSER_SEARCH) return L"VK_BROWSER_SEARCH";
	if(vk == VK_BROWSER_FAVORITES) return L"VK_BROWSER_FAVORITES";
	if(vk == VK_BROWSER_HOME) return L"VK_BROWSER_HOME";
	if(vk == VK_VOLUME_MUTE) return L"VK_VOLUME_MUTE";
	if(vk == VK_VOLUME_DOWN) return L"VK_VOLUME_DOWN";
	if(vk == VK_VOLUME_UP) return L"VK_VOLUME_UP";
	if(vk == VK_MEDIA_NEXT_TRACK) return L"VK_MEDIA_NEXT_TRACK";
	if(vk == VK_MEDIA_PREV_TRACK) return L"VK_MEDIA_PREV_TRACK";
	if(vk == VK_MEDIA_STOP) return L"VK_MEDIA_STOP";
	if(vk == VK_MEDIA_PLAY_PAUSE) return L"VK_MEDIA_PLAY_PAUSE";
	if(vk == VK_LAUNCH_MAIL) return L"VK_LAUNCH_MAIL";
	if(vk == VK_LAUNCH_MEDIA_SELECT) return L"VK_LAUNCH_MEDIA_SELECT";
	if(vk == VK_LAUNCH_APP1) return L"VK_LAUNCH_APP1";
	if(vk == VK_LAUNCH_APP2) return L"VK_LAUNCH_APP2";
	if(vk == VK_OEM_1) return L"VK_OEM_1";
	if(vk == VK_OEM_PLUS) return L"VK_OEM_PLUS";
	if(vk == VK_OEM_COMMA) return L"VK_OEM_COMMA";
	if(vk == VK_OEM_MINUS) return L"VK_OEM_MINUS";
	if(vk == VK_OEM_PERIOD) return L"VK_OEM_PERIOD";
	if(vk == VK_OEM_2) return L"VK_OEM_2";
	if(vk == VK_OEM_3) return L"VK_OEM_3";
	if(vk == VK_OEM_4) return L"VK_OEM_4";
	if(vk == VK_OEM_5) return L"VK_OEM_5";
	if(vk == VK_OEM_6) return L"VK_OEM_6";
	if(vk == VK_OEM_7) return L"VK_OEM_7";
	if(vk == VK_OEM_8) return L"VK_OEM_8";
	if(vk == VK_OEM_AX) return L"VK_OEM_AX";
	if(vk == VK_OEM_102) return L"VK_OEM_102";
	if(vk == VK_ICO_HELP) return L"VK_ICO_HELP";
	if(vk == VK_ICO_00) return L"VK_ICO_00";
	if(vk == VK_PROCESSKEY) return L"VK_PROCESSKEY";
	if(vk == VK_ICO_CLEAR) return L"VK_ICO_CLEAR";
	if(vk == VK_PACKET) return L"VK_PACKET";
	if(vk == VK_OEM_RESET) return L"VK_OEM_RESET";
	if(vk == VK_OEM_JUMP) return L"VK_OEM_JUMP";
	if(vk == VK_OEM_PA1) return L"VK_OEM_PA1";
	if(vk == VK_OEM_PA2) return L"VK_OEM_PA2";
	if(vk == VK_OEM_PA3) return L"VK_OEM_PA3";
	if(vk == VK_OEM_WSCTRL) return L"VK_OEM_WSCTRL";
	if(vk == VK_OEM_CUSEL) return L"VK_OEM_CUSEL";
	if(vk == VK_OEM_ATTN) return L"VK_OEM_ATTN";
	if(vk == VK_OEM_FINISH) return L"VK_OEM_FINISH";
	if(vk == VK_OEM_COPY) return L"VK_OEM_COPY";
	if(vk == VK_OEM_AUTO) return L"VK_OEM_AUTO";
	if(vk == VK_OEM_ENLW) return L"VK_OEM_ENLW";
	if(vk == VK_OEM_BACKTAB) return L"VK_OEM_BACKTAB";
	if(vk == VK_ATTN) return L"VK_ATTN";
	if(vk == VK_CRSEL) return L"VK_CRSEL";
	if(vk == VK_EXSEL) return L"VK_EXSEL";
	if(vk == VK_EREOF) return L"VK_EREOF";
	if(vk == VK_PLAY) return L"VK_PLAY";
	if(vk == VK_ZOOM) return L"VK_ZOOM";
	if(vk == VK_NONAME) return L"VK_NONAME";
	if(vk == VK_PA1) return L"VK_PA1";
	if(vk == VK_OEM_CLEAR) return L"VK_OEM_CLEAR";

	return L"";
}

#ifdef commentout
#pragma warning( disable: 4065 )
//#ifndef commentout_LOGMSG
#ifdef __xAFXSTR_H__
static CString LOGMSG( UINT msg )
{
	CString	s;
	switch( msg )
	{
//	case XTP_TTM_WINDOWFROMPOINT:	s.Format( "XTP_TTM_WINDOWFROMPOINT(0x%x)", (int)XTP_TTM_WINDOWFROMPOINT );	break;
#ifdef _WINUSER_
	case WM_NULL:					s = CString( "WM_NULL(0x0000)" );				break;
	case WM_CREATE:					s = CString( "WM_CREATE(0x0001)" );				break;
	case WM_DESTROY:				s = CString( "WM_DESTROY(0x0002)" );			break;
	case WM_MOVE:					s = CString( "WM_MOVE(0x0003)" );				break;
	case WM_SIZE:					s = CString( "WM_SIZE(0x0005)" );				break;
	case WM_ACTIVATE:				s = CString( "WM_ACTIVATE(0x0006)" );			break;
	case WM_SETFOCUS:				s = CString( "WM_SETFOCUS(0x0007)" );			break;
	case WM_KILLFOCUS:				s = CString( "WM_KILLFOCUS(0x0008)" );			break;
	case WM_ENABLE:					s = CString( "WM_ENABLE(0x000A)" );				break;
	case WM_SETREDRAW:				s = CString( "WM_SETREDRAW(0x000B)" );			break;
	case WM_SETTEXT:				s = CString( "WM_SETTEXT(0x000C)" );			break;
	case WM_GETTEXT:				s = CString( "WM_GETTEXT(0x000D)" );			break;
	case WM_GETTEXTLENGTH:			s = CString( "WM_GETTEXTLENGTH(0x000E)" );		break;
	case WM_PAINT:					s = CString( "WM_PAINT(0x000F)" );				break;
	case WM_CLOSE:					s = CString( "WM_CLOSE(0x0010)" );				break;
	case WM_QUERYENDSESSION:		s = CString( "WM_QUERYENDSESSION(0x0011)" );	break;
	case WM_QUERYOPEN:				s = CString( "WM_QUERYOPEN(0x0013)" );			break;
	case WM_ENDSESSION:				s = CString( "WM_ENDSESSION(0x0016)" );			break;
	case WM_QUIT:					s = CString( "WM_QUIT(0x0012)" );				break;
	case WM_ERASEBKGND:				s = CString( "WM_ERASEBKGND(0x0014)" );			break;
	case WM_SYSCOLORCHANGE:			s = CString( "WM_SYSCOLORCHANGE(0x0015)" );		break;
	case WM_SHOWWINDOW:				s = CString( "WM_SHOWWINDOW(0x0018)" );			break;
	case WM_WININICHANGE:			s = CString( "WM_WININICHANGE(0x001A)" );		break;
	case WM_DEVMODECHANGE:			s = CString( "WM_DEVMODECHANGE(0x001B)" );		break;
	case WM_ACTIVATEAPP:			s = CString( "WM_ACTIVATEAPP(0x001C)" );		break;
	case WM_FONTCHANGE:				s = CString( "WM_FONTCHANGE(0x001D)" );			break;
	case WM_TIMECHANGE:				s = CString( "WM_TIMECHANGE(0x001E)" );			break;
	case WM_CANCELMODE:				s = CString( "WM_CANCELMODE(0x001F)" );			break;
	case WM_SETCURSOR:				s = CString( "WM_SETCURSOR(0x0020)" );			break;
	case WM_MOUSEACTIVATE:			s = CString( "WM_MOUSEACTIVATE(0x0021)" );		break;
	case WM_CHILDACTIVATE:			s = CString( "WM_CHILDACTIVATE(0x0022)" );		break;
	case WM_QUEUESYNC:				s = CString( "WM_QUEUESYNC(0x0023)" );			break;
	case WM_GETMINMAXINFO:			s = CString( "WM_GETMINMAXINFO(0x0024)" );		break;
	case WM_PAINTICON:				s = CString( "WM_PAINTICON(0x0026)" );			break;
	case WM_ICONERASEBKGND:			s = CString( "WM_ICONERASEBKGND(0x0027)" );		break;
	case WM_NEXTDLGCTL:				s = CString( "WM_NEXTDLGCTL(0x0028)" );			break;
	case WM_SPOOLERSTATUS:			s = CString( "WM_SPOOLERSTATUS(0x002A)" );		break;
	case WM_DRAWITEM:				s = CString( "WM_DRAWITEM(0x002B)" );			break;
	case WM_MEASUREITEM:			s = CString( "WM_MEASUREITEM(0x002C)" );		break;
	case WM_DELETEITEM:				s = CString( "WM_DELETEITEM(0x002D)" );			break;
	case WM_VKEYTOITEM:				s = CString( "WM_VKEYTOITEM(0x002E)" );			break;
	case WM_CHARTOITEM:				s = CString( "WM_CHARTOITEM(0x002F)" );			break;
	case WM_SETFONT:				s = CString( "WM_SETFONT(0x0030)" );			break;
	case WM_GETFONT:				s = CString( "WM_GETFONT(0x0031)" );			break;
	case WM_SETHOTKEY:				s = CString( "WM_SETHOTKEY(0x0032)" );			break;
	case WM_GETHOTKEY:				s = CString( "WM_GETHOTKEY(0x0033)" );			break;
	case WM_QUERYDRAGICON:			s = CString( "WM_QUERYDRAGICON(0x0037)" );		break;
	case WM_COMPAREITEM:			s = CString( "WM_COMPAREITEM(0x0039)" );		break;
//	case WM_GETOBJECT:				s = CString( "WM_GETOBJECT(0x003D)" );			break;
	case 0x003D:					s = CString( "WM_GETOBJECT(0x003D)" );			break;
	case WM_COMPACTING:				s = CString( "WM_COMPACTING(0x0041)" );			break;
	case WM_COMMNOTIFY:				s = CString( "WM_COMMNOTIFY(0x0044)" );			break;
	case WM_WINDOWPOSCHANGING:		s = CString( "WM_WINDOWPOSCHANGING(0x0046)" );	break;
	case WM_WINDOWPOSCHANGED:		s = CString( "WM_WINDOWPOSCHANGED(0x0047)" );	break;
	case WM_POWER:					s = CString( "WM_POWER(0x0048)" );				break;
	case WM_COPYDATA:				s = CString( "WM_COPYDATA(0x004A)" );			break;
	case WM_CANCELJOURNAL:			s = CString( "WM_CANCELJOURNAL(0x004B)" );		break;
	case WM_NOTIFY:					s = CString( "WM_NOTIFY(0x004E)" );				break;
	case WM_INPUTLANGCHANGEREQUEST:	s = CString( "WM_INPUTLANGCHANGEREQUEST(0x0050)" );	break;
	case WM_INPUTLANGCHANGE:		s = CString( "WM_INPUTLANGCHANGE(0x0051)" );	break;
	case WM_TCARD:					s = CString( "WM_TCARD(0x0052)" );				break;
	case WM_HELP:					s = CString( "WM_HELP(0x0053)" );				break;
	case WM_USERCHANGED:			s = CString( "WM_USERCHANGED(0x0054)" );		break;
	case WM_NOTIFYFORMAT:			s = CString( "WM_NOTIFYFORMAT(0x0055)" );		break;
	case WM_CONTEXTMENU:			s = CString( "WM_CONTEXTMENU(0x007B)" );		break;
	case WM_STYLECHANGING:			s = CString( "WM_STYLECHANGING(0x007C)" );		break;
	case WM_STYLECHANGED:			s = CString( "WM_STYLECHANGED(0x007D)" );		break;
	case WM_DISPLAYCHANGE:			s = CString( "WM_DISPLAYCHANGE(0x007E)" );		break;
	case WM_GETICON:				s = CString( "WM_GETICON(0x007F)" );			break;
	case WM_SETICON:				s = CString( "WM_SETICON(0x0080)" );			break;
	case WM_NCCREATE:				s = CString( "WM_NCCREATE(0x0081)" );			break;
	case WM_NCDESTROY:				s = CString( "WM_NCDESTROY(0x0082)" );			break;
	case WM_NCCALCSIZE:				s = CString( "WM_NCCALCSIZE(0x0083)" );			break;
	case WM_NCHITTEST:				s = CString( "WM_NCHITTEST(0x0084)" );			break;
	case WM_NCPAINT:				s = CString( "WM_NCPAINT(0x0085)" );			break;
	case WM_NCACTIVATE:				s = CString( "WM_NCACTIVATE(0x0086)" );			break;
	case WM_GETDLGCODE:				s = CString( "WM_GETDLGCODE(0x0087)" );			break;
	case WM_SYNCPAINT:				s = CString( "WM_SYNCPAINT(0x0088)" );			break;
	case WM_NCMOUSEMOVE:			s = CString( "WM_NCMOUSEMOVE(0x00A0)" );		break;
	case WM_NCLBUTTONDOWN:			s = CString( "WM_NCLBUTTONDOWN(0x00A1)" );		break;
	case WM_NCLBUTTONUP:			s = CString( "WM_NCLBUTTONUP(0x00A2)" );		break;
	case WM_NCLBUTTONDBLCLK:		s = CString( "WM_NCLBUTTONDBLCLK(0x00A3)" );	break;
	case WM_NCRBUTTONDOWN:			s = CString( "WM_NCRBUTTONDOWN(0x00A4)" );		break;
	case WM_NCRBUTTONUP:			s = CString( "WM_NCRBUTTONUP(0x00A5)" );		break;
	case WM_NCRBUTTONDBLCLK:		s = CString( "WM_NCRBUTTONDBLCLK(0x00A6)" );	break;
	case WM_NCMBUTTONDOWN:			s = CString( "WM_NCMBUTTONDOWN(0x00A7)" );		break;
	case WM_NCMBUTTONUP:			s = CString( "WM_NCMBUTTONUP(0x00A8)" );		break;
	case WM_NCMBUTTONDBLCLK:		s = CString( "WM_NCMBUTTONDBLCLK(0x00A9)" );	break;
//	case WM_NCXBUTTONDOWN:			s = CString( "WM_NCXBUTTONDOWN(0x00AB)" );		break;
	case 0x00AB:					s = CString( "WM_NCXBUTTONDOWN(0x00AB)" );		break;
//	case WM_NCXBUTTONUP:			s = CString( "WM_NCXBUTTONUP(0x00AC)" );		break;
	case 0x00AC:					s = CString( "WM_NCXBUTTONUP(0x00AC)" );		break;
//	case WM_NCXBUTTONDBLCLK:		s = CString( "WM_NCXBUTTONDBLCLK(0x00AD)" );	break;
	case 0x00AD:					s = CString( "WM_NCXBUTTONDBLCLK(0x00AD)" );	break;
//	case WM_INPUT:					s = CString( "WM_INPUT(0x00FF)" );				break;
	case 0x00FF:					s = CString( "WM_INPUT(0x00FF)" );				break;
	case WM_KEYDOWN:				s = CString( "WM_KEYDOWN(0x0100)" );			break;
	case WM_KEYUP:					s = CString( "WM_KEYUP(0x0101)" );				break;
	case WM_CHAR:					s = CString( "WM_CHAR(0x0102)" );				break;
	case WM_DEADCHAR:				s = CString( "WM_DEADCHAR(0x0103)" );			break;
	case WM_SYSKEYDOWN:				s = CString( "WM_SYSKEYDOWN(0x0104)" );			break;
	case WM_SYSKEYUP:				s = CString( "WM_SYSKEYUP(0x0105)" );			break;
	case WM_SYSCHAR:				s = CString( "WM_SYSCHAR(0x0106)" );			break;
	case WM_SYSDEADCHAR:			s = CString( "WM_SYSDEADCHAR(0x0107)" );		break;
	case WM_UNICHAR:				s = CString( "WM_UNICHAR(0x0109)" );			break;
//	case WM_KEYLAST:				s = CString( "WM_KEYLAST(0x0108)" );			break;
	case 0x0108:					s = CString( "WM_KEYLAST(0x0108)" );			break;
	case WM_IME_STARTCOMPOSITION:	s = CString( "WM_IME_STARTCOMPOSITION(0x010D)" );	break;
	case WM_IME_ENDCOMPOSITION:		s = CString( "WM_IME_ENDCOMPOSITION(0x010E)" );	break;
	case WM_IME_KEYLAST:			s = CString( "WM_IME_KEYLAST(0x010F)" );		break;
	case WM_INITDIALOG:				s = CString( "WM_INITDIALOG(0x0110)" );			break;
	case WM_COMMAND:				s = CString( "WM_COMMAND(0x0111)" );			break;
	case WM_SYSCOMMAND:				s = CString( "WM_SYSCOMMAND(0x0112)" );			break;
	case WM_TIMER:					s = CString( "WM_TIMER(0x0113)" );				break;
	case WM_HSCROLL:				s = CString( "WM_HSCROLL(0x0114)" );			break;
	case WM_VSCROLL:				s = CString( "WM_VSCROLL(0x0115)" );			break;
	case WM_INITMENU:				s = CString( "WM_INITMENU(0x0116)" );			break;
	case WM_INITMENUPOPUP:			s = CString( "WM_INITMENUPOPUP(0x0117)" );		break;
	case WM_MENUSELECT:				s = CString( "WM_MENUSELECT(0x011F)" );			break;
	case WM_MENUCHAR:				s = CString( "WM_MENUCHAR(0x0120)" );			break;
	case WM_ENTERIDLE:				s = CString( "WM_ENTERIDLE(0x0121)" );			break;
//	case WM_MENURBUTTONUP:			s = CString( "WM_MENURBUTTONUP(0x0122)" );		break;
	case 0x0122:					s = CString( "WM_MENURBUTTONUP(0x0122)" );		break;
//	case WM_MENUDRAG:				s = CString( "WM_MENUDRAG(0x0123)" );			break;
	case 0x0123:					s = CString( "WM_MENUDRAG(0x0123)" );			break;
//	case WM_MENUGETOBJECT:			s = CString( "WM_MENUGETOBJECT(0x0124)" );		break;
	case 0x0124:					s = CString( "WM_MENUGETOBJECT(0x0124)" );		break;
//	case WM_UNINITMENUPOPUP:		s = CString( "WM_UNINITMENUPOPUP(0x0125)" );	break;
	case 0x0125:					s = CString( "WM_UNINITMENUPOPUP(0x0125)" );	break;
//	case WM_MENUCOMMAND:			s = CString( "WM_MENUCOMMAND(0x0126)" );		break;
	case 0x0126:					s = CString( "WM_MENUCOMMAND(0x0126)" );		break;
//	case WM_CHANGEUISTATE:			s = CString( "WM_CHANGEUISTATE(0x0127)" );		break;
	case 0x0127:					s = CString( "WM_CHANGEUISTATE(0x0127)" );		break;
//	case WM_UPDATEUISTATE:			s = CString( "WM_UPDATEUISTATE(0x0128)" );		break;
	case 0x0128:					s = CString( "WM_UPDATEUISTATE(0x0128)" );		break;
//	case WM_QUERYUISTATE:			s = CString( "WM_QUERYUISTATE(0x0129)" );		break;
	case 0x0129:					s = CString( "WM_QUERYUISTATE(0x0129)" );		break;
	case WM_CTLCOLORMSGBOX:			s = CString( "WM_CTLCOLORMSGBOX(0x0132)" );		break;
	case WM_CTLCOLOREDIT:			s = CString( "WM_CTLCOLOREDIT(0x0133)" );		break;
	case WM_CTLCOLORLISTBOX:		s = CString( "WM_CTLCOLORLISTBOX(0x0134)" );	break;
	case WM_CTLCOLORBTN:			s = CString( "WM_CTLCOLORBTN(0x0135)" );		break;
	case WM_CTLCOLORDLG:			s = CString( "WM_CTLCOLORDLG(0x0136)" );		break;
	case WM_CTLCOLORSCROLLBAR:		s = CString( "WM_CTLCOLORSCROLLBAR(0x0137)" );	break;
	case WM_CTLCOLORSTATIC:			s = CString( "WM_CTLCOLORSTATIC(0x0138)" );		break;
	case MN_GETHMENU:				s = CString( "MN_GETHMENU(0x01E1)" );			break;
	case WM_MOUSEMOVE:				s = CString( "WM_MOUSEMOVE(0x0200)" );			break;
	case WM_LBUTTONDOWN:			s = CString( "WM_LBUTTONDOWN(0x0201)" );		break;
	case WM_LBUTTONUP:				s = CString( "WM_LBUTTONUP(0x0202)" );			break;
	case WM_LBUTTONDBLCLK:			s = CString( "WM_LBUTTONDBLCLK(0x0203)" );		break;
	case WM_RBUTTONDOWN:			s = CString( "WM_RBUTTONDOWN(0x0204)" );		break;
	case WM_RBUTTONUP:				s = CString( "WM_RBUTTONUP(0x0205)" );			break;
	case WM_RBUTTONDBLCLK:			s = CString( "WM_RBUTTONDBLCLK(0x0206)" );		break;
	case WM_MBUTTONDOWN:			s = CString( "WM_MBUTTONDOWN(0x0207)" );		break;
	case WM_MBUTTONUP:				s = CString( "WM_MBUTTONUP(0x0208)" );			break;
//	case WM_MOUSELAST:				s = CString( "WM_MOUSELAST(0x0209)" );			break;
	case WM_MBUTTONDBLCLK:			s = CString( "WM_MBUTTONDBLCLK(0x0209)" );		break;
	case WM_MOUSEWHEEL:				s = CString( "WM_MOUSEWHEEL(0x020A)" );			break;
//	case WM_XBUTTONDOWN:			s = CString( "WM_XBUTTONDOWN(0x020B)" );		break;
	case 0x020B:					s = CString( "WM_XBUTTONDOWN(0x020B)" );		break;
//	case WM_XBUTTONUP:				s = CString( "WM_XBUTTONUP(0x020C)" );			break;
	case 0x020C:					s = CString( "WM_XBUTTONUP(0x020C)" );			break;
//	case WM_XBUTTONDBLCLK:			s = CString( "WM_XBUTTONDBLCLK(0x020D)" );		break;
	case 0x020D:					s = CString( "WM_XBUTTONDBLCLK(0x020D)" );		break;
	case WM_PARENTNOTIFY:			s = CString( "WM_PARENTNOTIFY(0x0210)" );		break;
	case WM_ENTERMENULOOP:			s = CString( "WM_ENTERMENULOOP(0x0211)" );		break;
	case WM_EXITMENULOOP:			s = CString( "WM_EXITMENULOOP(0x0212)" );		break;
	case WM_NEXTMENU:				s = CString( "WM_NEXTMENU(0x0213)" );			break;
	case WM_SIZING:					s = CString( "WM_SIZING(0x0214)" );				break;
	case WM_CAPTURECHANGED:			s = CString( "WM_CAPTURECHANGED(0x0215)" );		break;
	case WM_MOVING:					s = CString( "WM_MOVING(0x0216)" );				break;
	case WM_POWERBROADCAST:			s = CString( "WM_POWERBROADCAST(0x0218)" );		break;
	case WM_DEVICECHANGE:			s = CString( "WM_DEVICECHANGE(0x0219)" );		break;
	case WM_MDICREATE:				s = CString( "WM_MDICREATE(0x0220)" );			break;
	case WM_MDIDESTROY:				s = CString( "WM_MDIDESTROY(0x0221)" );			break;
	case WM_MDIACTIVATE:			s = CString( "WM_MDIACTIVATE(0x0222)" );		break;
	case WM_MDIRESTORE:				s = CString( "WM_MDIRESTORE(0x0223)" );			break;
	case WM_MDINEXT:				s = CString( "WM_MDINEXT(0x0224)" );			break;
	case WM_MDIMAXIMIZE:			s = CString( "WM_MDIMAXIMIZE(0x0225)" );		break;
	case WM_MDITILE:				s = CString( "WM_MDITILE(0x0226)" );			break;
	case WM_MDICASCADE:				s = CString( "WM_MDICASCADE(0x0227)" );			break;
	case WM_MDIICONARRANGE:			s = CString( "WM_MDIICONARRANGE(0x0228)" );		break;
	case WM_MDIGETACTIVE:			s = CString( "WM_MDIGETACTIVE(0x0229)" );		break;
	case WM_MDISETMENU:				s = CString( "WM_MDISETMENU(0x0230)" );			break;
	case WM_ENTERSIZEMOVE:			s = CString( "WM_ENTERSIZEMOVE(0x0231)" );		break;
	case WM_EXITSIZEMOVE:			s = CString( "WM_EXITSIZEMOVE(0x0232)" );		break;
	case WM_DROPFILES:				s = CString( "WM_DROPFILES(0x0233)" );			break;
	case WM_MDIREFRESHMENU:			s = CString( "WM_MDIREFRESHMENU(0x0234)" );		break;
	case WM_IME_SETCONTEXT:			s = CString( "WM_IME_SETCONTEXT(0x0281)" );		break;
	case WM_IME_NOTIFY:				s = CString( "WM_IME_NOTIFY(0x0282)" );			break;
	case WM_IME_CONTROL:			s = CString( "WM_IME_CONTROL(0x0283)" );		break;
	case WM_IME_COMPOSITIONFULL:	s = CString( "WM_IME_COMPOSITIONFULL(0x0284)" );	break;
	case WM_IME_SELECT:				s = CString( "WM_IME_SELECT(0x0285)" );			break;
	case WM_IME_CHAR:				s = CString( "WM_IME_CHAR(0x0286)" );			break;
//	case WM_IME_REQUEST:			s = CString( "WM_IME_REQUEST(0x0288)" );		break;
	case 0x0288:					s = CString( "WM_IME_REQUEST(0x0288)" );		break;
	case WM_IME_KEYDOWN:			s = CString( "WM_IME_KEYDOWN(0x0290)" );		break;
	case WM_IME_KEYUP:				s = CString( "WM_IME_KEYUP(0x0291)" );			break;
	case WM_MOUSEHOVER:				s = CString( "WM_MOUSEHOVER(0x02A1)" );			break;
	case WM_MOUSELEAVE:				s = CString( "WM_MOUSELEAVE(0x02A3)" );			break;
//	case WM_NCMOUSEHOVER:			s = CString( "WM_NCMOUSEHOVER(0x02A0)" );		break;
	case 0x02A0:					s = CString( "WM_NCMOUSEHOVER(0x02A0)" );		break;
//	case WM_NCMOUSELEAVE:			s = CString( "WM_NCMOUSELEAVE(0x02A2)" );		break;
	case 0x02A2:					s = CString( "WM_NCMOUSELEAVE(0x02A2)" );		break;
//	case WM_WTSSESSION_CHANGE:		s = CString( "WM_WTSSESSION_CHANGE(0x02B1)" );	break;
	case 0x02B1:					s = CString( "WM_WTSSESSION_CHANGE(0x02B1)" );	break;
//	case WM_TABLET_FIRST:			s = CString( "WM_TABLET_FIRST(0x02c0)" );		break;
	case 0x02c0:					s = CString( "WM_TABLET_FIRST(0x02c0)" );		break;
//	case WM_TABLET_LAST:			s = CString( "WM_TABLET_LAST(0x02df)" );		break;
	case 0x02df:					s = CString( "WM_TABLET_LAST(0x02df)" );		break;
	case WM_CUT:					s = CString( "WM_CUT(0x0300)" );				break;
	case WM_COPY:					s = CString( "WM_COPY(0x0301)" );				break;
	case WM_PASTE:					s = CString( "WM_PASTE(0x0302)" );				break;
	case WM_CLEAR:					s = CString( "WM_CLEAR(0x0303)" );				break;
	case WM_UNDO:					s = CString( "WM_UNDO(0x0304)" );				break;
	case WM_RENDERFORMAT:			s = CString( "WM_RENDERFORMAT(0x0305)" );		break;
	case WM_RENDERALLFORMATS:		s = CString( "WM_RENDERALLFORMATS(0x0306)" );	break;
	case WM_DESTROYCLIPBOARD:		s = CString( "WM_DESTROYCLIPBOARD(0x0307)" );	break;
	case WM_DRAWCLIPBOARD:			s = CString( "WM_DRAWCLIPBOARD(0x0308)" );		break;
	case WM_PAINTCLIPBOARD:			s = CString( "WM_PAINTCLIPBOARD(0x0309)" );		break;
	case WM_VSCROLLCLIPBOARD:		s = CString( "WM_VSCROLLCLIPBOARD(0x030A)" );	break;
	case WM_SIZECLIPBOARD:			s = CString( "WM_SIZECLIPBOARD(0x030B)" );		break;
	case WM_ASKCBFORMATNAME:		s = CString( "WM_ASKCBFORMATNAME(0x030C)" );	break;
	case WM_CHANGECBCHAIN:			s = CString( "WM_CHANGECBCHAIN(0x030D)" );		break;
	case WM_HSCROLLCLIPBOARD:		s = CString( "WM_HSCROLLCLIPBOARD(0x030E)" );	break;
	case WM_QUERYNEWPALETTE:		s = CString( "WM_QUERYNEWPALETTE(0x030F)" );	break;
	case WM_PALETTEISCHANGING:		s = CString( "WM_PALETTEISCHANGING(0x0310)" );	break;
	case WM_PALETTECHANGED:			s = CString( "WM_PALETTECHANGED(0x0311)" );		break;
	case WM_HOTKEY:					s = CString( "WM_HOTKEY(0x0312)" );				break;
	case WM_PRINT:					s = CString( "WM_PRINT(0x0317)" );				break;
	case WM_PRINTCLIENT:			s = CString( "WM_PRINTCLIENT(0x0318)" );		break;
//	case WM_APPCOMMAND:				s = CString( "WM_APPCOMMAND(0x0319)" );			break;
	case 0x0319:					s = CString( "WM_APPCOMMAND(0x0319)" );			break;
//	case WM_THEMECHANGED:			s = CString( "WM_THEMECHANGED(0x031A)" );		break;
	case 0x031A:					s = CString( "WM_THEMECHANGED(0x031A)" );		break;
	case WM_HANDHELDFIRST:			s = CString( "WM_HANDHELDFIRST(0x0358)" );		break;
	case WM_HANDHELDLAST:			s = CString( "WM_HANDHELDLAST(0x035F)" );		break;
	case WM_AFXFIRST:				s = CString( "WM_AFXFIRST(0x0360)" );			break;
	case WM_AFXLAST:				s = CString( "WM_AFXLAST(0x037F)" );			break;
	case WM_PENWINFIRST:			s = CString( "WM_PENWINFIRST(0x0380)" );		break;
	case WM_PENWINLAST:				s = CString( "WM_PENWINLAST(0x038F)" );			break;
#endif
	default:
#ifdef __UWINDOWS_H__
//	if( msg == WM_ACTIVATEPART_COMMANDCOMPLETED )	s.Format( _T("WM_ACTIVATEPART_COMMANDCOMPLETED(0x%x)"),	msg );	else
//	if( msg == WM_AUTO_BACKUPSAVE_CLEANUP )			s.Format( _T("WM_AUTO_BACKUPSAVE_CLEANUP(0x%x)"),		msg );	else
//	if( msg == WM_AUTOMATION_COMMAND )				s.Format( _T("WM_AUTOMATION_COMMAND(0x%x)"),			msg );	else
//	if( msg == WM_BEGIN_VIEWTRANSFORM )				s.Format( _T("WM_BEGIN_VIEWTRANSFORM(0x%x)"),			msg );	else
//	if( msg == WM_DMHOLE_UPDATE_FEATURE )			s.Format( _T("WM_DMHOLE_UPDATE_FEATURE(0x%x)"),			msg );	else
//	if( msg == WM_DTA_CHECK_COMMAND_ACTIVE )		s.Format( _T("WM_DTA_CHECK_COMMAND_ACTIVE(0x%x)"),		msg );	else
//	if( msg == WM_DTA_CHECK_NAME )					s.Format( _T("WM_DTA_CHECK_NAME(0x%x)"),				msg );	else
//	if( msg == WM_DTA_EMPTY_SELECT_LIST )			s.Format( _T("WM_DTA_EMPTY_SELECT_LIST(0x%x)"),			msg );	else
//	if( msg == WM_DTA_UPDATE_TABLE )				s.Format( _T("WM_DTA_UPDATE_TABLE(0x%x)"),				msg );	else
//	if( msg == WM_END_VIEWTRANSFORM )				s.Format( _T("WM_END_VIEWTRANSFORM(0x%x)"),				msg );	else
//	if( msg == WM_ENDPROFILE_COMMAND_COMPLETED )	s.Format( _T("WM_ENDPROFILE_COMMAND_COMPLETED(0x%x)"),	msg );	else
//	if( msg == WM_FOA_DISABLE_CONTROLS )			s.Format( _T("WM_FOA_DISABLE_CONTROLS(0x%x)"),			msg );	else
//	if( msg == WM_FOA_REENABLE_CONTROLS )			s.Format( _T("WM_FOA_REENABLE_CONTROLS(0x%x)"),			msg );	else
//	if( msg == WM_INIT_TOOLTIPCONTEXT )				s.Format( _T("WM_INIT_TOOLTIPCONTEXT(0x%x)"),			msg );	else
//	if( msg == WM_IS_SOLIDEDGE )					s.Format( _T("WM_IS_SOLIDEDGE(0x%x)"),					msg );	else
//	if( msg == WM_JACTIVEENVIRONMENTTRANSITION )	s.Format( _T("WM_JACTIVEENVIRONMENTTRANSITION(0x%x)"),	msg );	else
//	if( msg == WM_JACTIVESHEETTRANSITION )			s.Format( _T("WM_JACTIVESHEETTRANSITION(0x%x)"),		msg );	else
//	if( msg == WM_JCONTEXTHELPBYHELPID )			s.Format( _T("WM_JCONTEXTHELPBYHELPID(0x%x)"),			msg );	else
//	if( msg == WM_JCOPILOT )						s.Format( _T("WM_JCOPILOT(0x%x)"),						msg );	else
//	if( msg == WM_JDOCMARKEDEDITOPEN )				s.Format( _T("WM_JDOCMARKEDEDITOPEN(0x%x)"),			msg );	else
//	if( msg == WM_JDOCUMENTTRANSITION )				s.Format( _T("WM_JDOCUMENTTRANSITION(0x%x)"),			msg );	else
//	if( msg == WM_JENABLEAUTOMATION )				s.Format( _T("WM_JENABLEAUTOMATION(0x%x)"),				msg );	else
//	if( msg == WM_JENABLEHELPAUTOMATION )			s.Format( _T("WM_JENABLEHELPAUTOMATION(0x%x)"),			msg );	else
//	if( msg == WM_JENVIRONMENTTRANSITION )			s.Format( _T("WM_JENVIRONMENTTRANSITION(0x%x)"),		msg );	else
//	if( msg == WM_JIDLE )							s.Format( _T("WM_JIDLE(0x%x)"),							msg );	else
//	if( msg == WM_JPROCESSDDEREQUESTS )				s.Format( _T("WM_JPROCESSDDEREQUESTS(0x%x)"),			msg );	else
//	if( msg == WM_JQUERYAUTOMATION )				s.Format( _T("WM_JQUERYAUTOMATION(0x%x)"),				msg );	else
//	if( msg == WM_JQUERYHELPAUTOMATION )			s.Format( _T("WM_JQUERYHELPAUTOMATION(0x%x)"),			msg );	else
//	if( msg == WM_JSELECTSETCHANGE )				s.Format( _T("WM_JSELECTSETCHANGE(0x%x)"),				msg );	else
//	if( msg == WM_JSETFRAMEMENU )					s.Format( _T("WM_JSETFRAMEMENU(0x%x)"),					msg );	else
//	if( msg == WM_JSTARTSPECIFICHELPTUTORIAL )		s.Format( _T("WM_JSTARTSPECIFICHELPTUTORIAL(0x%x)"),	msg );	else
//	if( msg == WM_JVA )								s.Format( _T("WM_JVA(0x%x)"),							msg );	else
//	if( msg == WM_JVARIABLECHANGE )					s.Format( _T("WM_JVARIABLECHANGE(0x%x)"),				msg );	else
//	if( msg == WM_JVIEW_UPDATED_WITH_MOUSEBUTTON )	s.Format( _T("WM_JVIEW_UPDATED_WITH_MOUSEBUTTON(0x%x)"),msg );	else
//	if( msg == WM_JVIEWSETTRANSITION )				s.Format( _T("WM_JVIEWSETTRANSITION(0x%x)"),			msg );	else
//	if( msg == WM_JVK )								s.Format( _T("WM_JVK(0x%x)"),							msg );	else
//	if( msg == WM_MEASUREMENT_COMMANDCOMPLETED )	s.Format( _T("WM_MEASUREMENT_COMMANDCOMPLETED(0x%x)"),	msg );	else
//	if( msg == WM_MEASUREMENT_VARIABLE_CREATED )	s.Format( _T("WM_MEASUREMENT_VARIABLE_CREATED(0x%x)"),	msg );	else
//	if( msg == WM_MODIFYBALLOON_CMDCOMPLETED )		s.Format( _T("WM_MODIFYBALLOON_CMDCOMPLETED(0x%x)"),	msg );	else
//	if( msg == WM_MODIFYBALLOON_CMDSTARTED )		s.Format( _T("WM_MODIFYBALLOON_CMDSTARTED(0x%x)"),		msg );	else
//	if( msg == WM_MODIFYBALLOON_NOPOPUP )			s.Format( _T("WM_MODIFYBALLOON_NOPOPUP(0x%x)"),			msg );	else
//	if( msg == WM_OPTIONVALUE_MODIFIED )			s.Format( _T("WM_OPTIONVALUE_MODIFIED(0x%x)"),			msg );	else
//	if( msg == WM_REFRESH_SELECT_LIST )				s.Format( _T("WM_REFRESH_SELECT_LIST(0x%x)"),			msg );	else
//	if( msg == WM_RUNNING_LOW_ON_MEMORY )			s.Format( _T("WM_RUNNING_LOW_ON_MEMORY(0x%x)"),			msg );	else
	if( msg == WM_SCREENUPDATING )					s.Format( _T("WM_SCREENUPDATING(0x%x)"),				msg );	else
//	if( msg == WM_SE_LOW_MEMORY_WARNING )			s.Format( _T("WM_SE_LOW_MEMORY_WARNING(0x%x)"),			msg );	else
//	if( msg == WM_SECRETAGENTMAN )					s.Format( _T("WM_SECRETAGENTMAN(0x%x)"),				msg );	else
//	if( msg == WM_START_DIM_TEXT_EDIT )				s.Format( _T("WM_START_DIM_TEXT_EDIT(0x%x)"),			msg );	else
//	if( msg == WM_TANGENT_REF_PLANE_REFRESH )		s.Format( _T("WM_TANGENT_REF_PLANE_REFRESH(0x%x)"),		msg );	else
//	if( msg == WM_TERMINAL_BALLOON_MODIFIED )		s.Format( _T("WM_TERMINAL_BALLOON_MODIFIED(0x%x)"),		msg );	else
//	if( msg == WM_VIEW_TRANSFORM_CHANGED )			s.Format( _T("WM_VIEW_TRANSFORM_CHANGED(0x%x)"),		msg );	else
	if( msg == WM_VWBACTIVATE )						s.Format( _T("WM_VWBACTIVATE(0x%x)"),					msg );	else
	if( msg == WM_VWBDEACTIVATE )					s.Format( _T("WM_VWBDEACTIVATE(0x%x)"),					msg );	else
	if( msg == WM_VWBDRAWHANDLES )					s.Format( _T("WM_VWBDRAWHANDLES(0x%x)"),				msg );	else
	if( msg == WM_VWBMOUSEACTIVATE )				s.Format( _T("WM_VWBMOUSEACTIVATE(0x%x)"),				msg );	else
	if( msg == WM_VWBSCANSELECTSET )				s.Format( _T("WM_VWBSCANSELECTSET(0x%x)"),				msg );	else
	if( msg == WM_VWBSLEEP )						s.Format( _T("WM_VWBSLEEP(0x%x)"),						msg );	else
	if( msg == WM_VWBTRANSITION )					s.Format( _T("WM_VWBTRANSITION(0x%x)"),					msg );	else
	if( msg == WM_VWBWAKEUP )						s.Format( _T("WM_VWBWAKEUP(0x%x)"),						msg );	else
//	if( msg == WM_WIREPATH_COMMANDCOMPLETED )		s.Format( _T("WM_WIREPATH_COMMANDCOMPLETED(0x%x)"),		msg );	else
#endif
	if( msg == 0x0360 )	s.Format( _T("WM_QUERYAFXWNDPROC") );		else
	if( msg == 0x0361 )	s.Format( _T("WM_SIZEPARENT") );			else
	if( msg == 0x0362 )	s.Format( _T("WM_SETMESSAGESTRING") );		else
	if( msg == 0x0363 )	s.Format( _T("WM_IDLEUPDATECMDUI") );		else
	if( msg == 0x0364 )	s.Format( _T("WM_INITIALUPDATE") );			else
	if( msg == 0x0365 )	s.Format( _T("WM_COMMANDHELP") );			else
	if( msg == 0x0366 )	s.Format( _T("WM_HELPHITTEST") );			else
	if( msg == 0x0367 )	s.Format( _T("WM_EXITHELPMODE") );			else
	if( msg == 0x0368 )	s.Format( _T("WM_RECALCPARENT") );			else
	if( msg == 0x0369 )	s.Format( _T("WM_SIZECHILD") );				else
	if( msg == 0x036A )	s.Format( _T("WM_KICKIDLE") );				else
	if( msg == 0x036B )	s.Format( _T("WM_QUERYCENTERWND") );		else
	if( msg == 0x036C )	s.Format( _T("WM_DISABLEMODAL") );			else
	if( msg == 0x036D )	s.Format( _T("WM_FLOATSTATUS") );			else
	if( msg == 0x036E )	s.Format( _T("WM_ACTIVATETOPLEVEL") );		else
	if( msg == 0x036F )	s.Format( _T("WM_RESERVED_036F") );			else
	if( msg == 0x0370 )	s.Format( _T("WM_RESERVED_0370") );			else
	if( msg == 0x0371 )	s.Format( _T("WM_RESERVED_0371") );			else
	if( msg == 0x0372 )	s.Format( _T("WM_RESERVED_0372") );			else
	if( msg == 0x0373 )	s.Format( _T("WM_SOCKET_NOTIFY") );			else
	if( msg == 0x0374 )	s.Format( _T("WM_SOCKET_DEAD") );			else
	if( msg == 0x0375 )	s.Format( _T("WM_POPMESSAGESTRING") );		else
	if( msg == 0x0376 )	s.Format( _T("WM_HELPPROMPTADDR") );		else
	if( msg == 0x0376 )	s.Format( _T("WM_OCC_LOADFROMSTREAM") );	else
	if( msg == 0x0377 )	s.Format( _T("WM_OCC_LOADFROMSTORAGE") );	else
	if( msg == 0x0378 )	s.Format( _T("WM_OCC_INITNEW") );			else
	if( msg == 0x037A )	s.Format( _T("WM_OCC_LOADFROMSTREAM_EX") );	else
	if( msg == 0x037B )	s.Format( _T("WM_OCC_LOADFROMSTORAGE_EX") );	else
	if( msg == 0x0379 )	s.Format( _T("WM_QUEUE_SENTINEL") );		else
	if( msg == 0x037C )	s.Format( _T("WM_RESERVED_037C") );			else
	if( msg == 0x037D )	s.Format( _T("WM_RESERVED_037D") );			else
	if( msg == 0x037E )	s.Format( _T("WM_RESERVED_037E") );			else
	if( msg == 0x037F )	s.Format( _T("WM_FORWARDMSG") );			else
			s.Format( _T("Unknown(%x)"), msg );
		break;
	}

	return s;
}
#endif
#endif

#define	LOGFUNCTION			LOGfunction	xFunctionLogx( &tLogfile, __FUNCTION__, ++xcount[__LINE__], __LINE__, __FILE__, 0, 20000 );
#define	LOGFUNCTIONmx(n)	LOGfunction	xFunctionLogx( &tLogfile, __FUNCTION__, ++xcount[__LINE__], __LINE__, __FILE__, 0, (n) );
#define	LOGFUNCTIONst(n)	LOGfunction	xFunctionLogx( &tLogfile, __FUNCTION__, ++xcount[__LINE__], __LINE__, __FILE__, (n) );
#define	LOGBLOCK			LOGfunction	xFunctionLogx( &tLogfile, NULL, -1, __LINE__, NULL, 0, 100000 );
#define	LOGTHIS				tLogfile.fwriteln( "this: 0x%x", this );
//#define	LOGBLOCK			LOGfunction	xFunctionLogx__LINE__( &tLogfile, "CODEBLOCK", -1, __LINE__, NULL, 0, 1000 );
//#define	//LOGFUNCTION	LOGfunction	xFunctionLogx( &tLogfile, __FUNCTION__, ++xcount[__LINE__], __LINE__, NULL );

#define	GDIobjects	\
{	\
	DWORD n = GetGuiResources( GetCurrentProcess(), 0 );	\
	tLogfile.fwriteln( "%1d: GDI objects: %1d", __LINE__, n );	\
}

#define xSTRING(str)		AnsiString(str)
#define xWIDESTRING(str)	WideString(str)

#define	zMARKER	tLogfile.fwriteln( "MARKER================================================================MARKER" );

#ifdef __UMATH_H__
#define	LOGGPOSITION2D( cp, title )	\
	tLogfile.fwriteln( "%1d: GPosition2d: %s: %lf %lf", __LINE__, (char*)(title), (cp).x, (cp).y );
#endif

#ifdef __EXTRAS_H__
#ifdef commentout
static char* MyAnsiString1( const OLECHAR FAR* strIn )
{
	static char buf1[4096];
	return( ConvertStrWtoA( strIn, buf1, 4096 ) );	
}
static char* MyAnsiString2( const OLECHAR FAR* strIn )
{
	static char buf2[4096];
	return( ConvertStrWtoA( strIn, buf2, 4096 ) );	
}
static char* MyAnsiString3( const OLECHAR FAR* strIn )
{
	static char buf3[4096];
	return( ConvertStrWtoA( strIn, buf3, 4096 ) );	
}

#define STRING1(str)	MyAnsiString1(str)
#define STRING2(str)	MyAnsiString2(str)
#define STRING3(str)	MyAnsiString3(str)
#endif
#endif
#ifndef commentout
#define	LOGMSGIN(nMsg)	\
	bool blog = false;	\
	\
	if( (nMsg) == WM_NCPAINT )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_NCPAINT" );	\
	}	\
	else	\
	if( (nMsg) == WM_SIZE )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		WORD cx, cy;	\
		cx = lParam & 0xffff;	\
		cy = (lParam>>16) & 0xffff;	\
		switch( wParam )	\
		{	\
		case SIZE_RESTORED:		tLogfile.fwriteln( "WM_SIZE: SIZE_RESTORED: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MINIMIZED:	tLogfile.fwriteln( "WM_SIZE: SIZE_MINIMIZED: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MAXIMIZED:	tLogfile.fwriteln( "WM_SIZE: SIZE_MAXIMIZED: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MAXSHOW:		tLogfile.fwriteln( "WM_SIZE: SIZE_MAXSHOW: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MAXHIDE:		tLogfile.fwriteln( "WM_SIZE: SIZE_MAXHIDE: cx, cy: %1d %1d", cx, cy );	break;	\
		default:				tLogfile.fwriteln( "WM_SIZE: nType: cx, cy: %1d %1d %1d", wParam, cx, cy );	break;	\
		}	\
	}	\
	else	\
	if( (nMsg) == WM_VWBDEACTIVATE )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_VWBDEACTIVATE: %x %x", wParam, lParam );	\
	}	\
	else	\
	\
	if( (nMsg) == WM_VWBACTIVATE )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_VWBACTIVATE: %x %x", wParam, lParam );	\
	}
#endif

#ifdef commentout
#define	LOGMSGIN(nMsg)	\
	bool blog = false;	\
	\
	blog = true;	\
	tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
	tLogfile.addindent();	\
	LOGMSG(&tLogfile,nMsg);
#endif

#ifdef commentout
#define	LOGMSGIN(nMsg)	\
	bool blog = false;	\
	\
	if( (nMsg) == WM_SIZE )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		WORD cx, cy;	\
		cx = lParam & 0xffff;	\
		cy = (lParam>>16) & 0xffff;	\
		switch( wParam )	\
		{	\
		case SIZE_RESTORED:		tLogfile.fwriteln( "WM_SIZE: SIZE_RESTORED: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MINIMIZED:	tLogfile.fwriteln( "WM_SIZE: SIZE_MINIMIZED: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MAXIMIZED:	tLogfile.fwriteln( "WM_SIZE: SIZE_MAXIMIZED: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MAXSHOW:		tLogfile.fwriteln( "WM_SIZE: SIZE_MAXSHOW: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MAXHIDE:		tLogfile.fwriteln( "WM_SIZE: SIZE_MAXHIDE: cx, cy: %1d %1d", cx, cy );	break;	\
		default:				tLogfile.fwriteln( "WM_SIZE: nType: cx, cy: %1d %1d %1d", wParam, cx, cy );	break;	\
		}	\
	}
#endif

#ifdef commentout
#define	LOGMSGIN(nMsg)	\
	bool blog = false;	\
	\
	if( (nMsg) == WM_VIEW_RESIZE )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_VIEW_RESIZE: %x %x", wParam, lParam );	\
	}	\
	else	\
	if( (nMsg) == WM_VWBDEACTIVATE )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_VWBDEACTIVATE: %x %x", wParam, lParam );	\
	}	\
	else	\
	\
	if( (nMsg) == WM_VWBACTIVATE )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_VWBACTIVATE: %x %x", wParam, lParam );	\
	}	\
	else	\
	\
	if( (nMsg) == WM_KILLFOCUS )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_KILLFOCUS: %x %x", wParam, lParam );	\
	}	\
	else	\
	\
	if( (nMsg) == WM_NCCALCSIZE )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_NCCALCSIZE: %x %x", wParam, lParam );	\
	}	\
	else	\
	\
	if( (nMsg) == WM_NCPAINT )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_NCPAINT: %x %x", wParam, lParam );	\
	}	\
	else	\
	\
	if( (nMsg) == WM_SETFOCUS )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_SETFOCUS: %x %x", wParam, lParam );	\
	}	\
	else	\
	\
	if( (nMsg) == WM_MOVE )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_MOVE: %x %x", wParam, lParam );	\
	}	\
	else	\
	\
	if( (nMsg) == WM_SIZE )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		WORD cx, cy;	\
		cx = lParam & 0xffff;	\
		cy = (lParam>>16) & 0xffff;	\
		switch( wParam )	\
		{	\
		case SIZE_RESTORED:		tLogfile.fwriteln( "WM_SIZE: SIZE_RESTORED: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MINIMIZED:	tLogfile.fwriteln( "WM_SIZE: SIZE_MINIMIZED: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MAXIMIZED:	tLogfile.fwriteln( "WM_SIZE: SIZE_MAXIMIZED: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MAXSHOW:		tLogfile.fwriteln( "WM_SIZE: SIZE_MAXSHOW: cx, cy: %1d %1d", cx, cy );	break;	\
		case SIZE_MAXHIDE:		tLogfile.fwriteln( "WM_SIZE: SIZE_MAXHIDE: cx, cy: %1d %1d", cx, cy );	break;	\
		default:				tLogfile.fwriteln( "WM_SIZE: nType: cx, cy: %1d %1d %1d", wParam, cx, cy );	break;	\
		}	\
	}	\
	else	\
	\
	if( (nMsg) == WM_WINDOWPOSCHANGING )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_WINDOWPOSCHANGING" );	\
	}	\
	else	\
	\
	if( (nMsg) == WM_WINDOWPOSCHANGED )	\
	{	\
		blog = true;	\
		tLogfile.fwriteln( "%s(%1d)", __FUNCTION__, ++xcount[0] );	\
		tLogfile.addindent();	\
		tLogfile.fwriteln( "WM_WINDOWPOSCHANGED" );	\
	\
		if( lParam != 0 )	\
		{	\
			WINDOWPOS*	pwp = (WINDOWPOS*)lParam;	\
	\
			tLogfile.fwriteln( "WINDOWPOS" );	\
			tLogfile.addindent();	\
			tLogfile.fwriteln( "hwnd            : %x", pwp->hwnd );	\
			tLogfile.fwriteln( "hwndInsertAfter : %x", pwp->hwndInsertAfter );	\
		    tLogfile.fwriteln( "x, y            : %1d %1d", pwp->x, pwp->y );	\
			tLogfile.fwriteln( "cx, cy          : %1d %1d", pwp->cx, pwp->cy );	\
			tLogfile.fwriteln( "flags           : %x", pwp->flags );	\
			tLogfile.subindent();	\
		}	\
	}
#endif

#define	LOGMSGOUT	\
	if( blog )	\
	{	\
		tLogfile.subindent();	\
	}

static void LOG_LPMEASUREITEMSTRUCT( LOGfile* pLogfile, LPMEASUREITEMSTRUCT lpMeasureItemStruct, char* title )
{
	pLogfile->fwriteln( "LPMEASUREITEMSTRUCT: %s", title );
	pLogfile->addindent();
	pLogfile->fwriteln( "CtlType   : %x", lpMeasureItemStruct->CtlType );
	//if( (lpMeasureItemStruct)->CtlType == ODT_HEADER )			pLogfile->fwriteln( "CtlType   : ODT_HEADER" );
	//if( (lpMeasureItemStruct)->CtlType == ODT_TAB )			pLogfile->fwriteln( "CtlType   : ODT_TAB" );
	//if( (lpMeasureItemStruct)->CtlType == ODT_LISTVIEW )		pLogfile->fwriteln( "CtlType   : ODT_LISTVIEW" );
	if( (lpMeasureItemStruct)->CtlType == ODT_MENU )			pLogfile->fwriteln( "CtlType   : ODT_MENU" );
	if( (lpMeasureItemStruct)->CtlType == ODT_LISTBOX )		pLogfile->fwriteln( "CtlType   : ODT_LISTBOX" );
	if( (lpMeasureItemStruct)->CtlType == ODT_COMBOBOX )		pLogfile->fwriteln( "CtlType   : ODT_COMBOBOX" );
	if( (lpMeasureItemStruct)->CtlType == ODT_BUTTON )			pLogfile->fwriteln( "CtlType   : ODT_BUTTON" );
	if( (lpMeasureItemStruct)->CtlType == ODT_STATIC )			pLogfile->fwriteln( "CtlType   : ODT_STATIC" );
	pLogfile->fwriteln( "CtlID     : %x", lpMeasureItemStruct->CtlID );
	pLogfile->fwriteln( "itemID    : %x", lpMeasureItemStruct->itemID );
	pLogfile->fwriteln( "itemWidth : %1d", lpMeasureItemStruct->itemWidth );
	pLogfile->fwriteln( "itemHeight: %1d", lpMeasureItemStruct->itemHeight );
	pLogfile->fwriteln( "itemData  : %x", lpMeasureItemStruct->itemData );
	pLogfile->subindent();
}

#define LOG_LPDRAWITEMSTRUCT(lpDrawItemStruct, title )	\
{	\
	tLogfile.fwriteln( "LPDRAWITEMSTRUCT %s = %x", (char*)(title), (lpDrawItemStruct) );	\
	tLogfile.addindent();	\
	if( (lpDrawItemStruct) )	\
	{	\
		tLogfile.fwriteln( "CtlType   : %x", (lpDrawItemStruct)->CtlType );	\
		if( (lpDrawItemStruct)->CtlType == ODT_HEADER )			tLogfile.fwriteln( "CtlType   : ODT_HEADER" );	\
		if( (lpDrawItemStruct)->CtlType == ODT_TAB )			tLogfile.fwriteln( "CtlType   : ODT_TAB" );	\
		if( (lpDrawItemStruct)->CtlType == ODT_LISTVIEW )		tLogfile.fwriteln( "CtlType   : ODT_LISTVIEW" );	\
		if( (lpDrawItemStruct)->CtlType == ODT_MENU )			tLogfile.fwriteln( "CtlType   : ODT_MENU" );	\
		if( (lpDrawItemStruct)->CtlType == ODT_LISTBOX )		tLogfile.fwriteln( "CtlType   : ODT_LISTBOX" );	\
		if( (lpDrawItemStruct)->CtlType == ODT_COMBOBOX )		tLogfile.fwriteln( "CtlType   : ODT_COMBOBOX" );	\
		if( (lpDrawItemStruct)->CtlType == ODT_BUTTON )			tLogfile.fwriteln( "CtlType   : ODT_BUTTON" );	\
		if( (lpDrawItemStruct)->CtlType == ODT_STATIC )			tLogfile.fwriteln( "CtlType   : ODT_STATIC" );	\
	\
		tLogfile.fwriteln( "CtlID     : %x", (lpDrawItemStruct)->CtlID );	\
		tLogfile.fwriteln( "itemID    : %x", (lpDrawItemStruct)->itemID );	\
	\
		tLogfile.fwriteln( "itemAction: %x", (lpDrawItemStruct)->itemAction );	\
		if( (lpDrawItemStruct)->itemAction & ODA_DRAWENTIRE )	tLogfile.fwriteln( "itemAction: ODA_DRAWENTIRE" );	\
		if( (lpDrawItemStruct)->itemAction & ODA_SELECT )		tLogfile.fwriteln( "itemAction: ODA_SELECT" );	\
		if( (lpDrawItemStruct)->itemAction & ODA_FOCUS )		tLogfile.fwriteln( "itemAction: ODA_FOCUS" );	\
	\
		tLogfile.fwriteln( "itemState : %x", (lpDrawItemStruct)->itemState );	\
		if( (lpDrawItemStruct)->itemState & ODS_SELECTED )	tLogfile.fwriteln( "itemState : ODS_SELECTED" );	\
		if( (lpDrawItemStruct)->itemState & ODS_GRAYED )		tLogfile.fwriteln( "itemState : ODS_GRAYED" );	\
		if( (lpDrawItemStruct)->itemState & ODS_DISABLED )	tLogfile.fwriteln( "itemState : ODS_DISABLED" );	\
		if( (lpDrawItemStruct)->itemState & ODS_CHECKED )		tLogfile.fwriteln( "itemState : ODS_CHECKED" );	\
		if( (lpDrawItemStruct)->itemState & ODS_FOCUS )		tLogfile.fwriteln( "itemState : ODS_FOCUS" );	\
		if( (lpDrawItemStruct)->itemState & ODS_DEFAULT )		tLogfile.fwriteln( "itemState : ODS_DEFAULT" );	\
		if( (lpDrawItemStruct)->itemState & ODS_COMBOBOXEDIT )tLogfile.fwriteln( "itemState : ODS_COMBOBOXEDIT" );	\
		if( (lpDrawItemStruct)->itemState & ODS_HOTLIGHT )	tLogfile.fwriteln( "itemState : ODS_HOTLIGHT" );	\
		/*if( (lpDrawItemStruct)->itemState & ODS_INACTIVE )	tLogfile.fwriteln( "itemState : ODS_INACTIVE" );*/	\
		if( (lpDrawItemStruct)->itemState & ODS_NOACCEL )		tLogfile.fwriteln( "itemState : ODS_NOACCEL" );	\
		if( (lpDrawItemStruct)->itemState & ODS_NOFOCUSRECT )	tLogfile.fwriteln( "itemState : ODS_NOFOCUSRECT" );	\
	\
		tLogfile.fwriteln( "hwndItem  : %x", (lpDrawItemStruct)->hwndItem );	\
		tLogfile.fwriteln( "hDC       : %x", (lpDrawItemStruct)->hDC );	\
		tLogfile.fwriteln( "rcItem    : %1d %1d %1d %1d", 	\
			(lpDrawItemStruct)->rcItem.left,	\
			(lpDrawItemStruct)->rcItem.right,	\
			(lpDrawItemStruct)->rcItem.top,	\
			(lpDrawItemStruct)->rcItem.bottom );	\
		tLogfile.fwriteln( "itemData  : %x", (lpDrawItemStruct)->itemData );	\
	}	\
	tLogfile.subindent();	\
}

#define	LOG_LV_COLUMN(lvc, title)	\
{	\
tLogfile.fwriteln( "LV_COLUMN: %s", (char*)title );	\
tLogfile.addindent();	\
	\
	tLogfile.fwriteln( "mask      : %x", (lvc).mask );	\
	if( (lvc).mask & LVCF_FMT )		tLogfile.fwriteln( "mask      : LVCF_FMT" );	\
	if( (lvc).mask & LVCF_WIDTH )	tLogfile.fwriteln( "mask      : LVCF_WIDTH" );	\
	if( (lvc).mask & LVCF_TEXT )	tLogfile.fwriteln( "mask      : LVCF_TEXT" );	\
	if( (lvc).mask & LVCF_SUBITEM )	tLogfile.fwriteln( "mask      : LVCF_SUBITEM" );	\
	if( (lvc).mask & LVCF_IMAGE )	tLogfile.fwriteln( "mask      : LVCF_IMAGE" );	\
	if( (lvc).mask & LVCF_ORDER )	tLogfile.fwriteln( "mask      : LVCF_ORDER" );	\
	\
	tLogfile.fwriteln( "fmt       : %x", (lvc).fmt );	\
	if( (lvc).fmt & LVCFMT_LEFT )			tLogfile.fwriteln( "fmt      : LVCFMT_LEFT" );	\
	if( (lvc).fmt & LVCFMT_RIGHT )			tLogfile.fwriteln( "fmt      : LVCFMT_RIGHT" );	\
	if( (lvc).fmt & LVCFMT_CENTER )			tLogfile.fwriteln( "fmt      : LVCFMT_CENTER" );	\
	if( (lvc).fmt & LVCFMT_IMAGE )			tLogfile.fwriteln( "fmt      : LVCFMT_IMAGE" );	\
	if( (lvc).fmt & LVCFMT_BITMAP_ON_RIGHT )tLogfile.fwriteln( "fmt      : LVCFMT_BITMAP_ON_RIGHT" );	\
	if( (lvc).fmt & LVCFMT_COL_HAS_IMAGES )	tLogfile.fwriteln( "fmt      : LVCFMT_COL_HAS_IMAGES" );	\
	\
	tLogfile.fwriteln( "cx        : %x", (lvc).cx );	\
	tLogfile.fwriteln( "pszText   : %x", (lvc).pszText );	\
	tLogfile.fwriteln( "cchTextMax: %x", (lvc).cchTextMax );	\
	tLogfile.fwriteln( "iSubItem  : %x", (lvc).iSubItem );	\
	tLogfile.fwriteln( "iImage    : %x", (lvc).iImage );	\
	tLogfile.fwriteln( "iOrder    : %x", (lvc).iOrder );	\
tLogfile.subindent();	\
}

#define	LOG_EMBEDEDCONTROLTYPE( eControlType, title )	\
{	\
CString	s;	\
s.Format( "%1d: EMBEDEDCONTROLTYPE: %s: %x: ", __LINE__, (char*)title, (eControlType) );	\
if( (eControlType) == SELV_EDITBOX )						s += CString( "SELV_EDITBOX, " );	\
if( (eControlType) == SELV_COMBOBOX )						s += CString( "SELV_COMBOBOX, " );	\
if( (eControlType) == SELV_COMBOBOX_EDITABLE )				s += CString( "SELV_COMBOBOX_EDITABLE, " );	\
if( (eControlType) == SELV_COMBOBOX_MULTIVALUE )			s += CString( "SELV_COMBOBOX_MULTIVALUE, " );	\
if( (eControlType) == SELV_DATECONTROL )					s += CString( "SELV_DATECONTROL, " );	\
if( (eControlType) == SELV_SPINBUTTON )						s += CString( "SELV_SPINBUTTON, " );	\
if( (eControlType) == SELV_RADIOBUTTON )					s += CString( "SELV_RADIOBUTTON, " );	\
if( (eControlType) == SELV_PUSHBUTTON )						s += CString( "SELV_PUSHBUTTON, " );	\
if( (eControlType) == SELV_COMBOBOX_MULTIVALUE_EDITABLE )	s += CString( "SELV_COMBOBOX_MULTIVALUE_EDITABLE, " );	\
if( (eControlType) == SELV_CHECKBOX )						s += CString( "SELV_CHECKBOX, " );	\
if( (eControlType) == SELV_JUNIT_COMBOBOX )					s += CString( "SELV_JUNIT_COMBOBOX, " );	\
if( (eControlType) == SELV_JUNIT_EDITBOX )					s += CString( "SELV_JUNIT_EDITBOX, " );	\
if( (eControlType) == SELV_JCUSTOM_COMBOBOX )				s += CString( "SELV_JCUSTOM_COMBOBOX, " );	\
if( (eControlType) == SELV_COLOR_COMBOBOX )					s += CString( "SELV_COLOR_COMBOBOX, " );	\
if( (eControlType) == SELV_DYNAMIC_COMBOBOX )				s += CString( "SELV_DYNAMIC_COMBOBOX, " );	\
tLogfile.fwriteln( "%s", LPCTSTR(s) );	\
}

#define	LOG_SPFLAGS( nSPFlags, title )	\
{	\
CString	s;	\
s.Format( "%1d: %s: %x: ", __LINE__, (char*)title, (nSPFlags) );	\
if( (nSPFlags) & SELV_SPFLAG_COMPACT_PATH )				s += CString( "SELV_SPFLAG_COMPACT_PATH" );	\
if( (nSPFlags) & SELV_SPFLAG_USE_AUTOCOMPLETE )			s += CString( "SELV_SPFLAG_USE_AUTOCOMPLETE" );	\
if( (nSPFlags) & SELV_SPFLAG_DISPLAY_SORTED_LIST )		s += CString( "SELV_SPFLAG_DISPLAY_SORTED_LIST" );	\
if( (nSPFlags) & SELV_SPFLAG_ON_CLICK_SELECT_NOTHING )	s += CString( "SELV_SPFLAG_ON_CLICK_SELECT_NOTHING" );	\
if( (nSPFlags) & SELV_SPFLAG_ON_CLICK_SELECT_FULL_ROW )	s += CString( "SELV_SPFLAG_ON_CLICK_SELECT_FULL_ROW" );	\
if( (nSPFlags) & SELV_SPFLAG_CELL_WITH_CHECKBOX )		s += CString( "SELV_SPFLAG_CELL_WITH_CHECKBOX" );	\
if( (nSPFlags) & SELV_SPFLAG_ITEM_NO_SORTING )			s += CString( "SELV_SPFLAG_ITEM_NO_SORTING" );	\
tLogfile.fwriteln( "%s", LPCTSTR(s) );	\
}

#define	LOGSTR( cp, title )	\
	tLogfile.fwriteln( "%1d: STR: %s: %s", __LINE__, (char*)(title), xNarrow(LPCTSTR((cp))) );

#define	LOGCRECT( cr, title )	\
	tLogfile.fwriteln( "%1d: CRect: %s: %1d %1d %1d %1d: %1d %1d", __LINE__, (char*)(title),	\
		(cr).left, (cr).top, (cr).right, (cr).bottom, (cr).Width(), (cr).Height() );

#define	LOGRECT( cr, title )	\
	tLogfile.fwriteln( "%1d: RECT: %s: %1d %1d %1d %1d: %1d %1d", __LINE__, (char*)(title),	\
		(cr).left, (cr).top, (cr).right, (cr).bottom, ((cr).right-(cr).left), ((cr).bottom-(cr).top) );

#define	LOGCPOINT( cp, title )	\
	tLogfile.fwriteln( "%1d: CPoint: %s: %1d %1d", __LINE__, (char*)(title), (cp).x, (cp).y );

#define	LOGPOINT( cp, title )	\
	tLogfile.fwriteln( "%1d: POINT: %s: %1d %1d", __LINE__, (char*)(title), (cp).x, (cp).y );

#define	LOGCSIZE( cp, title )	\
	tLogfile.fwriteln( "%1d: CSize: %s: %1d %1d", __LINE__, (char*)(title), (cp).cx, (cp).cy );

#define	LOGSIZE( cp, title )	\
	tLogfile.fwriteln( "%1d: SIZE: %s: %1d %1d", __LINE__, (char*)(title), (cp).cx, (cp).cy );

#define	LOGGPOSITION( cp, title )	\
	tLogfile.fwriteln( "%1d: GPosition: %s: %lf %lf %lf", __LINE__, (char*)(title), (cp).x, (cp).y, (cp).z );

#define	LOGGVECTOR( cp, title )	\
	tLogfile.fwriteln( "%1d: GVector: %s: %lf %lf %lf: %lf", __LINE__, (char*)(title), (cp).x, (cp).y, (cp).z, (cp).GetLength() );

#define	LOGGUNITVECTOR( cp, title )	\
	tLogfile.fwriteln( "%1d: GVector: %s: %lf %lf %lf", __LINE__, (char*)(title), (cp).x, (cp).y, (cp).z );

#define	LOGGVECTOR2D( cp, title )	\
	tLogfile.fwriteln( "%1d: GVector2D: %s: %lf %lf: %lf", __LINE__, (char*)(title), (cp).x, (cp).y, (cp).GetLength() );

#define LOGCOLORREF( cp, title )	\
	tLogfile.fwriteln( "%d: COLORREF: %s: %x: (0x%x 0x%x 0x%x)", __LINE__, (char*)(title), (cp), GetRValue((cp)), GetGValue((cp)), GetBValue((cp)) );

#define	LOGD( cp, title )	\
	tLogfile.fwriteln( "%1d: double: %s: %.10lf: ---------- %s", __LINE__, (char*)(title), (cp), __FILE__ );

#define	LOGI( cp, title )	\
	tLogfile.fwriteln( "%1d: int: %s: %1d", __LINE__, (char*)(title), (cp) );

#define	LOGII( cp1, cp2, title )	\
	tLogfile.fwriteln( "%1d: int: %s: %1d %1d", __LINE__, (char*)(title), (cp1), (cp2) );

#define	LOGX( cp, title )	\
	tLogfile.fwriteln( "%1d: int: %s: 0x%x", __LINE__, (char*)(title), (cp) );

#define	LOGB( cp, title )	\
	if( (cp) )	\
		tLogfile.fwriteln( "%1d: BOOL  : %s: TRUE", __LINE__, (char*)(title) );	\
	else	\
		tLogfile.fwriteln( "%1d: BOOL  : %s: FALSE", __LINE__, (char*)(title) );

#define	LOGCLSID( cp, title )	\
	tLogfile.fwriteln( "%1d: CLSID: %s: 0x%08x 0x%04x 0x%04x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", __LINE__, (char*)(title),	\
		(cp).Data1,	(cp).Data2,	(cp).Data3,	\
		(cp).Data4[0], (cp).Data4[1], (cp).Data4[2], (cp).Data4[3], 	\
		(cp).Data4[4], (cp).Data4[5], (cp).Data4[6], (cp).Data4[7] );

#define LOGSTYLE( dwStyle )	\
	if( (dwStyle) & WS_OVERLAPPED )				tLogfile.fwriteln( "   WS_OVERLAPPED" );	\
	if( (dwStyle) & WS_POPUP )					tLogfile.fwriteln( "   WS_POPUP" );	\
	if( (dwStyle) & WS_CHILD )					tLogfile.fwriteln( "   WS_CHILD" );	\
	if( (dwStyle) & WS_MINIMIZE )				tLogfile.fwriteln( "   WS_MINIMIZE" );	\
	if( (dwStyle) & WS_VISIBLE )				tLogfile.fwriteln( "   WS_VISIBLE" );	\
	if( (dwStyle) & WS_DISABLED )				tLogfile.fwriteln( "   WS_DISABLED" );	\
	if( (dwStyle) & WS_CLIPSIBLINGS )			tLogfile.fwriteln( "   WS_CLIPSIBLINGS" );	\
	if( (dwStyle) & WS_CLIPCHILDREN )			tLogfile.fwriteln( "   WS_CLIPCHILDREN" );	\
	if( (dwStyle) & WS_MAXIMIZE )				tLogfile.fwriteln( "   WS_MAXIMIZE" );	\
	if( (dwStyle) & WS_CAPTION )				tLogfile.fwriteln( "   WS_CAPTION" );	\
	if( (dwStyle) & WS_BORDER )					tLogfile.fwriteln( "   WS_BORDER" );	\
	if( (dwStyle) & WS_DLGFRAME )				tLogfile.fwriteln( "   WS_DLGFRAME" );	\
	if( (dwStyle) & WS_VSCROLL )				tLogfile.fwriteln( "   WS_VSCROLL" );	\
	if( (dwStyle) & WS_HSCROLL )				tLogfile.fwriteln( "   WS_HSCROLL" );	\
	if( (dwStyle) & WS_SYSMENU )				tLogfile.fwriteln( "   WS_SYSMENU" );	\
	if( (dwStyle) & WS_THICKFRAME )				tLogfile.fwriteln( "   WS_THICKFRAME" );	\
	if( (dwStyle) & WS_GROUP )					tLogfile.fwriteln( "   WS_GROUP" );	\
	if( (dwStyle) & WS_TABSTOP )				tLogfile.fwriteln( "   WS_TABSTOP" );	\
	if( (dwStyle) & WS_MINIMIZEBOX )			tLogfile.fwriteln( "   WS_MINIMIZEBOX" );	\
	if( (dwStyle) & WS_MAXIMIZEBOX )			tLogfile.fwriteln( "   WS_MAXIMIZEBOX" );

#define LOGEXSTYLE( dwExStyle )	\
	if( (dwExStyle) & WS_EX_DLGMODALFRAME )		tLogfile.fwriteln( "   WS_EX_DLGMODALFRAME" );	\
	if( (dwExStyle) & WS_EX_NOPARENTNOTIFY )	tLogfile.fwriteln( "   WS_EX_NOPARENTNOTIFY" );	\
	if( (dwExStyle) & WS_EX_TOPMOST )			tLogfile.fwriteln( "   WS_EX_TOPMOST" );	\
	if( (dwExStyle) & WS_EX_ACCEPTFILES )		tLogfile.fwriteln( "   WS_EX_ACCEPTFILES" );	\
	if( (dwExStyle) & WS_EX_TRANSPARENT )		tLogfile.fwriteln( "   WS_EX_TRANSPARENT" );	\
	if( (dwExStyle) & WS_EX_MDICHILD )			tLogfile.fwriteln( "   WS_EX_MDICHILD" );	\
	if( (dwExStyle) & WS_EX_TOOLWINDOW )		tLogfile.fwriteln( "   WS_EX_TOOLWINDOW" );	\
	if( (dwExStyle) & WS_EX_WINDOWEDGE )		tLogfile.fwriteln( "   WS_EX_WINDOWEDGE" );	\
	if( (dwExStyle) & WS_EX_CLIENTEDGE )		tLogfile.fwriteln( "   WS_EX_CLIENTEDGE" );	\
	if( (dwExStyle) & WS_EX_CONTEXTHELP )		tLogfile.fwriteln( "   WS_EX_CONTEXTHELP" );	\
	if( (dwExStyle) & WS_EX_RIGHT )				tLogfile.fwriteln( "   WS_EX_RIGHT" );	\
	if( (dwExStyle) & WS_EX_LEFT )				tLogfile.fwriteln( "   WS_EX_LEFT" );	\
	if( (dwExStyle) & WS_EX_RTLREADING )		tLogfile.fwriteln( "   WS_EX_RTLREADING" );	\
	if( (dwExStyle) & WS_EX_LTRREADING )		tLogfile.fwriteln( "   WS_EX_LTRREADING" );	\
	if( (dwExStyle) & WS_EX_LEFTSCROLLBAR )		tLogfile.fwriteln( "   WS_EX_LEFTSCROLLBAR" );	\
	if( (dwExStyle) & WS_EX_RIGHTSCROLLBAR )	tLogfile.fwriteln( "   WS_EX_RIGHTSCROLLBAR" );	\
	if( (dwExStyle) & WS_EX_CONTROLPARENT )		tLogfile.fwriteln( "   WS_EX_CONTROLPARENT" );	\
	if( (dwExStyle) & WS_EX_STATICEDGE )		tLogfile.fwriteln( "   WS_EX_STATICEDGE" );	\
	if( (dwExStyle) & WS_EX_APPWINDOW )			tLogfile.fwriteln( "   WS_EX_APPWINDOW" );

#ifdef _WINUSER_
static void LOG_NMHDR( LOGfile* pLogfile, NMHDR nmHdr, char* title )
{
	pLogfile->fwriteln( "NMHDR: %s", (char*)(title) );
	pLogfile->addindent();
	pLogfile->fwriteln( "hwndFrom: %x", nmHdr.hwndFrom );
	pLogfile->fwriteln( "idFrom  : %x", nmHdr.idFrom );
	pLogfile->fwriteln( "code    : %x", nmHdr.code );
	pLogfile->subindent();
}
#endif

#ifdef _INC_COMMCTRL
static void LOG_NMLISTVIEW( LOGfile* pLogfile, NMLISTVIEW nmListView, char* title )
{
	pLogfile->fwriteln( "NMLISTVIEW: %s", (char*)(title) );
	pLogfile->addindent();
	LOG_NMHDR( pLogfile, nmListView.hdr, "nmListView.hdr" );
	pLogfile->fwriteln( "iItem    : %x", nmListView.iItem );
	pLogfile->fwriteln( "iSubItem : %x", nmListView.iSubItem );
	pLogfile->fwriteln( "uNewState: %x", nmListView.uNewState );
	if( nmListView.uNewState & LVIS_FOCUSED )		pLogfile->fwriteln(  "uNewState: LVIS_FOCUSED" );
	if( nmListView.uNewState & LVIS_SELECTED )		pLogfile->fwriteln(  "uNewState: LVIS_SELECTED" );
	if( nmListView.uNewState & LVIS_CUT )			pLogfile->fwriteln(  "uNewState: LVIS_CUT" );
	if( nmListView.uNewState & LVIS_DROPHILITED )	pLogfile->fwriteln(  "uNewState: LVIS_DROPHILITED" );
	if( nmListView.uNewState & LVIS_GLOW )			pLogfile->fwriteln(  "uNewState: LVIS_GLOW" );
	if( nmListView.uNewState & LVIS_ACTIVATING )	pLogfile->fwriteln(  "uNewState: LVIS_ACTIVATING" );
	pLogfile->fwriteln( "uOldState: %x", nmListView.uOldState );
	pLogfile->fwriteln( "uChanged : %x", nmListView.uChanged );
	pLogfile->fwriteln( "ptAction : %x", nmListView.ptAction );
	pLogfile->fwriteln( "lParam   : %x", nmListView.lParam );
	pLogfile->subindent();
}
#endif

#ifdef _INC_COMMCTRL
static void LOG_LVCOLUMN( LOGfile* pLogfile, LVCOLUMN lvColumn, char* title )
{
	pLogfile->fwriteln( "LVCOLUMN: %s", (char*)(title) );
	pLogfile->addindent();
	pLogfile->fwriteln( "mask      : %x", lvColumn.mask );
    pLogfile->fwriteln( "fmt       : %x", lvColumn.fmt );
    pLogfile->fwriteln( "cx        : %x", lvColumn.cx );
	if( lvColumn.pszText )
	    pLogfile->fwriteln( "pszText   : %s", lvColumn.pszText );
	else
	    pLogfile->fwriteln( "pszText   : NULL" );
    pLogfile->fwriteln( "cchTextMax: %x", lvColumn.cchTextMax );
    pLogfile->fwriteln( "iSubItem  : %x", lvColumn.iSubItem );
    pLogfile->fwriteln( "iImage    : %x", lvColumn.iImage );
    pLogfile->fwriteln( "iOrder    : %x", lvColumn.iOrder );
#	if _WIN32_WINNT >= 0x0600
    pLogfile->fwriteln( "cxMin     : %x", lvColumn.cxMin );
    pLogfile->fwriteln( "cxDefault : %x", lvColumn.cxDefault );
    pLogfile->fwriteln( "cxIdeal   : %x", lvColumn.cxIdeal );
#	endif
	pLogfile->subindent();
}
#endif

#ifdef commentout
static void LOG_LVITEM( LVITEM lvItem, char* title )
{
	tLogfile.fwriteln( "LVITEM: %s", (char*)(title) );
	tLogfile.addindent();
	tLogfile.fwriteln( "mask      : %x", lvItem.mask );
    tLogfile.fwriteln( "iItem     : %1d", lvItem.iItem );
    tLogfile.fwriteln( "iSubItem  : %1d", lvItem.iSubItem );
    tLogfile.fwriteln( "state     : %x", lvItem.state );
    tLogfile.fwriteln( "stateMask : %x", lvItem.stateMask );
    tLogfile.fwriteln( "pszText   : %s", lvItem.pszText );
    tLogfile.fwriteln( "cchTextMax: %1d", lvItem.cchTextMax );
    tLogfile.fwriteln( "iImage    : %x", lvItem.iImage );
    tLogfile.fwriteln( "lParam    : %x", lvItem.lParam );
    tLogfile.fwriteln( "iIndent   : %1d", lvItem.iIndent );
    tLogfile.fwriteln( "iGroupId  : %x", lvItem.iGroupId );
    tLogfile.fwriteln( "cColumns  : %x", lvItem.cColumns );
    tLogfile.fwriteln( "puColumns : %x", lvItem.puColumns );
	tLogfile.subindent();
}
#endif

#define	LOG_WINDOWPLACEMENT( pWnd )	\
{	\
	WINDOWPLACEMENT wndpl;	\
	(pWnd)->GetWindowPlacement( &wndpl );	\
	tLogfile.fwriteln( "WINDOWPLACEMENT" );	\
	tLogfile.addindent();	\
	LOGI( wndpl.length, "wndpl.length" );	\
    LOGX( wndpl.flags, "wndpl.flags" );	\
    LOGX( wndpl.showCmd, "wndpl.showCmd" );	\
    LOGCPOINT( CPoint(wndpl.ptMinPosition), "wndpl.ptMinPosition" );	\
    LOGCPOINT( CPoint(wndpl.ptMaxPosition), "wndpl.ptMaxPosition" );	\
    LOGCRECT( CRect(wndpl.rcNormalPosition), "wndpl.rcNormalPosition" );	\
	tLogfile.subindent();	\
}

#define	LOG_WINDOWINFO( pWnd )	\
{	\
	WINDOWINFO	wi;	\
	(pWnd)->GetWindowInfo( &wi );	\
	tLogfile.fwriteln( "WINDOWINFO" );	\
	tLogfile.addindent();	\
	LOGI( wi.cbSize, "wi.cbSize" );	\
	LOGCRECT( CRect( wi.rcWindow ), "wi.rcWindow" );	\
	LOGCRECT( CRect( wi.rcClient ), "wi.rcClient" );	\
	LOGX( wi.dwStyle, "wi.dwStyle" );	\
	LOGX( wi.dwExStyle, "wi.dwExStyle" );	\
	LOGX( wi.dwWindowStatus, "wi.dwWindowStatus" );	\
	LOGI( wi.cxWindowBorders, "wi.cxWindowBorders" );	\
	LOGI( wi.cyWindowBorders, "wi.cyWindowBorders" );	\
	LOGI( wi.atomWindowType, "wi.atomWindowType" );	\
	LOGI( wi.wCreatorVersion, "wi.wCreatorVersion" );	\
	tLogfile.subindent();	\
}

#define LOG_WINDOWPOS( pwp )	\
{	\
	tLogfile.fwriteln( "WINDOWPOS" );	\
	tLogfile.addindent();	\
	tLogfile.fwriteln( "hwnd            : %x", pwp->hwnd );	\
	tLogfile.fwriteln( "hwndInsertAfter : %x", pwp->hwndInsertAfter );	\
    tLogfile.fwriteln( "x, y            : %1d %1d", pwp->x, pwp->y );	\
	tLogfile.fwriteln( "cx, cy          : %1d %1d", pwp->cx, pwp->cy );	\
	tLogfile.fwriteln( "flags           : %x", pwp->flags );	\
	if( pwp->flags & SWP_NOSIZE )			tLogfile.fwriteln( "                : SWP_NOSIZE" );	\
	if( pwp->flags & SWP_NOMOVE )			tLogfile.fwriteln( "                : SWP_NOMOVE" );	\
	if( pwp->flags & SWP_NOZORDER )			tLogfile.fwriteln( "                : SWP_NOZORDER" );	\
	if( pwp->flags & SWP_NOREDRAW )			tLogfile.fwriteln( "                : SWP_NOREDRAW" );	\
	if( pwp->flags & SWP_NOACTIVATE )		tLogfile.fwriteln( "                : SWP_NOACTIVATE" );	\
	if( pwp->flags & SWP_FRAMECHANGED )		tLogfile.fwriteln( "                : SWP_FRAMECHANGED" );	\
	if( pwp->flags & SWP_SHOWWINDOW )		tLogfile.fwriteln( "                : SWP_SHOWWINDOW" );	\
	if( pwp->flags & SWP_HIDEWINDOW )		tLogfile.fwriteln( "                : SWP_HIDEWINDOW" );	\
	if( pwp->flags & SWP_NOCOPYBITS )		tLogfile.fwriteln( "                : SWP_NOCOPYBITS" );	\
	if( pwp->flags & SWP_NOOWNERZORDER )	tLogfile.fwriteln( "                : SWP_NOOWNERZORDER" );	\
	if( pwp->flags & SWP_NOSENDCHANGING )	tLogfile.fwriteln( "                : SWP_NOSENDCHANGING" );	\
	if( pwp->flags & SWP_DRAWFRAME )		tLogfile.fwriteln( "                : SWP_DRAWFRAME" );	\
	if( pwp->flags & SWP_NOREPOSITION )		tLogfile.fwriteln( "                : SWP_NOREPOSITION" );	\
	if( pwp->flags & SWP_DEFERERASE )		tLogfile.fwriteln( "                : SWP_DEFERERASE" );	\
	if( pwp->flags & SWP_ASYNCWINDOWPOS )	tLogfile.fwriteln( "                : SWP_ASYNCWINDOWPOS" );	\
	tLogfile.subindent();	\
}

#define	LOG_CWND_STYLE( pWnd )	\
{	\
	DWORD dwStyle = (pWnd)->GetStyle();	\
	tLogfile.fwriteln( "dwStyle: %x", dwStyle );	\
	if( dwStyle & DS_ABSALIGN )	tLogfile.fwriteln( "   DS_ABSALIGN(0x00000001)" );	\
	if( dwStyle & DS_SYSMODAL )	tLogfile.fwriteln( "   DS_SYSMODAL(0x00000002)" );	\
	if( dwStyle & DS_3DLOOK )	tLogfile.fwriteln( "   DS_3DLOOK(0x00000004)" );	\
	if( dwStyle & DS_FIXEDSYS )	tLogfile.fwriteln( "   DS_FIXEDSYS(0x00000008)" );	\
	if( dwStyle & DS_NOFAILCREATE )	tLogfile.fwriteln( "   DS_NOFAILCREATE(0x00000010)" );	\
	if( dwStyle & DS_LOCALEDIT )	tLogfile.fwriteln( "   DS_LOCALEDIT(0x00000020)" );	\
	if( dwStyle & DS_SETFONT )	tLogfile.fwriteln( "   DS_SETFONT(0x00000040)" );	\
	if( dwStyle & DS_MODALFRAME )	tLogfile.fwriteln( "   DS_MODALFRAME(0x00000080)" );	\
	if( dwStyle & DS_NOIDLEMSG )	tLogfile.fwriteln( "   DS_NOIDLEMSG(0x00000100)" );	\
	if( dwStyle & DS_SETFOREGROUND )	tLogfile.fwriteln( "   DS_SETFOREGROUND(0x00000200)" );	\
	if( dwStyle & DS_CONTROL )	tLogfile.fwriteln( "   DS_CONTROL(0x00000400)" );	\
	if( dwStyle & DS_CENTER )	tLogfile.fwriteln( "   DS_CENTER(0x00000800)" );	\
	if( dwStyle & DS_CENTERMOUSE )	tLogfile.fwriteln( "   DS_CENTERMOUSE(0x00001000)" );	\
	if( dwStyle & DS_CONTEXTHELP )	tLogfile.fwriteln( "   DS_CONTEXTHELP(0x00002000)" );	\
	if( dwStyle & WS_MAXIMIZEBOX )	tLogfile.fwriteln( "   WS_MAXIMIZEBOX(0x00010000)" );	\
	if( dwStyle & WS_MINIMIZEBOX )	tLogfile.fwriteln( "   WS_MINIMIZEBOX(0x00020000)" );	\
	if( dwStyle & WS_TABSTOP )	tLogfile.fwriteln( "   WS_TABSTOP(0x00010000)" );	\
	if( dwStyle & WS_GROUP )	tLogfile.fwriteln( "   WS_GROUP(0x00020000)" );	\
	if( dwStyle & WS_THICKFRAME )	tLogfile.fwriteln( "   WS_THICKFRAME(0x00040000)" );	\
	if( dwStyle & WS_SYSMENU )	tLogfile.fwriteln( "   WS_SYSMENU(0x00080000)" );	\
	if( dwStyle & WS_HSCROLL )	tLogfile.fwriteln( "   WS_HSCROLL(0x00100000)" );	\
	if( dwStyle & WS_VSCROLL )	tLogfile.fwriteln( "   WS_VSCROLL(0x00200000)" );	\
	if( dwStyle & WS_DLGFRAME )	tLogfile.fwriteln( "   WS_DLGFRAME(0x00400000)" );	\
	if( dwStyle & WS_BORDER )	tLogfile.fwriteln( "   WS_BORDER(0x00800000)" );	\
	if( dwStyle & WS_MAXIMIZE )	tLogfile.fwriteln( "   WS_MAXIMIZE(0x01000000)" );	\
	if( dwStyle & WS_CLIPCHILDREN )	tLogfile.fwriteln( "   WS_CLIPCHILDREN(0x02000000)" );	\
	if( dwStyle & WS_CLIPSIBLINGS )	tLogfile.fwriteln( "   WS_CLIPSIBLINGS(0x04000000)" );	\
	if( dwStyle & WS_DISABLED )	tLogfile.fwriteln( "   WS_DISABLED(0x08000000)" );	\
	if( dwStyle & WS_VISIBLE )	tLogfile.fwriteln( "   WS_VISIBLE(0x10000000)" );	\
	if( dwStyle & WS_MINIMIZE )	tLogfile.fwriteln( "   WS_MINIMIZE(0x20000000)" );	\
	if( dwStyle & WS_CHILD )	tLogfile.fwriteln( "   WS_CHILD(0x40000000)" );	\
	if( dwStyle & WS_POPUP )	tLogfile.fwriteln( "   WS_POPUP(0x80000000)" );	\
}

#define	LOG_STYLE( dwStyle )	\
{	\
	tLogfile.fwriteln( "dwStyle: %x", dwStyle );	\
	if( dwStyle & DS_ABSALIGN )	tLogfile.fwriteln( "   DS_ABSALIGN(0x00000001)" );	\
	if( dwStyle & DS_SYSMODAL )	tLogfile.fwriteln( "   DS_SYSMODAL(0x00000002)" );	\
	if( dwStyle & DS_3DLOOK )	tLogfile.fwriteln( "   DS_3DLOOK(0x00000004)" );	\
	if( dwStyle & DS_FIXEDSYS )	tLogfile.fwriteln( "   DS_FIXEDSYS(0x00000008)" );	\
	if( dwStyle & DS_NOFAILCREATE )	tLogfile.fwriteln( "   DS_NOFAILCREATE(0x00000010)" );	\
	if( dwStyle & DS_LOCALEDIT )	tLogfile.fwriteln( "   DS_LOCALEDIT(0x00000020)" );	\
	if( dwStyle & DS_SETFONT )	tLogfile.fwriteln( "   DS_SETFONT(0x00000040)" );	\
	if( dwStyle & DS_MODALFRAME )	tLogfile.fwriteln( "   DS_MODALFRAME(0x00000080)" );	\
	if( dwStyle & DS_NOIDLEMSG )	tLogfile.fwriteln( "   DS_NOIDLEMSG(0x00000100)" );	\
	if( dwStyle & DS_SETFOREGROUND )	tLogfile.fwriteln( "   DS_SETFOREGROUND(0x00000200)" );	\
	if( dwStyle & DS_CONTROL )	tLogfile.fwriteln( "   DS_CONTROL(0x00000400)" );	\
	if( dwStyle & DS_CENTER )	tLogfile.fwriteln( "   DS_CENTER(0x00000800)" );	\
	if( dwStyle & DS_CENTERMOUSE )	tLogfile.fwriteln( "   DS_CENTERMOUSE(0x00001000)" );	\
	if( dwStyle & DS_CONTEXTHELP )	tLogfile.fwriteln( "   DS_CONTEXTHELP(0x00002000)" );	\
	if( dwStyle & WS_MAXIMIZEBOX )	tLogfile.fwriteln( "   WS_MAXIMIZEBOX(0x00010000)" );	\
	if( dwStyle & WS_MINIMIZEBOX )	tLogfile.fwriteln( "   WS_MINIMIZEBOX(0x00020000)" );	\
	if( dwStyle & WS_TABSTOP )	tLogfile.fwriteln( "   WS_TABSTOP(0x00010000)" );	\
	if( dwStyle & WS_GROUP )	tLogfile.fwriteln( "   WS_GROUP(0x00020000)" );	\
	if( dwStyle & WS_THICKFRAME )	tLogfile.fwriteln( "   WS_THICKFRAME(0x00040000)" );	\
	if( dwStyle & WS_SYSMENU )	tLogfile.fwriteln( "   WS_SYSMENU(0x00080000)" );	\
	if( dwStyle & WS_HSCROLL )	tLogfile.fwriteln( "   WS_HSCROLL(0x00100000)" );	\
	if( dwStyle & WS_VSCROLL )	tLogfile.fwriteln( "   WS_VSCROLL(0x00200000)" );	\
	if( dwStyle & WS_DLGFRAME )	tLogfile.fwriteln( "   WS_DLGFRAME(0x00400000)" );	\
	if( dwStyle & WS_BORDER )	tLogfile.fwriteln( "   WS_BORDER(0x00800000)" );	\
	if( dwStyle & WS_MAXIMIZE )	tLogfile.fwriteln( "   WS_MAXIMIZE(0x01000000)" );	\
	if( dwStyle & WS_CLIPCHILDREN )	tLogfile.fwriteln( "   WS_CLIPCHILDREN(0x02000000)" );	\
	if( dwStyle & WS_CLIPSIBLINGS )	tLogfile.fwriteln( "   WS_CLIPSIBLINGS(0x04000000)" );	\
	if( dwStyle & WS_DISABLED )	tLogfile.fwriteln( "   WS_DISABLED(0x08000000)" );	\
	if( dwStyle & WS_VISIBLE )	tLogfile.fwriteln( "   WS_VISIBLE(0x10000000)" );	\
	if( dwStyle & WS_MINIMIZE )	tLogfile.fwriteln( "   WS_MINIMIZE(0x20000000)" );	\
	if( dwStyle & WS_CHILD )	tLogfile.fwriteln( "   WS_CHILD(0x40000000)" );	\
	if( dwStyle & WS_POPUP )	tLogfile.fwriteln( "   WS_POPUP(0x80000000)" );	\
}

#define	LOG_CWND_EXSTYLE( pWnd )	\
{	\
	DWORD dwExStyle = (pWnd)->GetExStyle();	\
	tLogfile.fwriteln( "dwExStyle: %x", dwExStyle );	\
	if( dwExStyle & WS_EX_DLGMODALFRAME )	tLogfile.fwriteln( "   WS_EX_DLGMODALFRAME(0x00000001)" );	\
	if( dwExStyle & WS_EX_NOPARENTNOTIFY )	tLogfile.fwriteln( "   WS_EX_NOPARENTNOTIFY(0x00000004)" );	\
	if( dwExStyle & WS_EX_TOPMOST )	tLogfile.fwriteln( "   WS_EX_TOPMOST(0x00000008)" );	\
	if( dwExStyle & WS_EX_ACCEPTFILES )	tLogfile.fwriteln( "   WS_EX_ACCEPTFILES(0x00000010)" );	\
	if( dwExStyle & WS_EX_TRANSPARENT )	tLogfile.fwriteln( "   WS_EX_TRANSPARENT(0x00000020)" );	\
	if( dwExStyle & WS_EX_MDICHILD )	tLogfile.fwriteln( "   WS_EX_MDICHILD(0x00000040)" );	\
	if( dwExStyle & WS_EX_TOOLWINDOW )	tLogfile.fwriteln( "   WS_EX_TOOLWINDOW(0x00000080)" );	\
	if( dwExStyle & WS_EX_WINDOWEDGE )	tLogfile.fwriteln( "   WS_EX_WINDOWEDGE(0x00000100)" );	\
	if( dwExStyle & WS_EX_CLIENTEDGE )	tLogfile.fwriteln( "   WS_EX_CLIENTEDGE(0x00000200)" );	\
	if( dwExStyle & WS_EX_CONTEXTHELP )	tLogfile.fwriteln( "   WS_EX_CONTEXTHELP(0x00000400)" );	\
	if( dwExStyle & WS_EX_RIGHT )	tLogfile.fwriteln( "   WS_EX_RIGHT(0x00001000)" );	\
	if( dwExStyle & WS_EX_LEFT )	tLogfile.fwriteln( "   WS_EX_LEFT(0x00000000)" );	\
	if( dwExStyle & WS_EX_RTLREADING )	tLogfile.fwriteln( "   WS_EX_RTLREADING(0x00002000)" );	\
	if( dwExStyle & WS_EX_LTRREADING )	tLogfile.fwriteln( "   WS_EX_LTRREADING(0x00000000)" );	\
	if( dwExStyle & WS_EX_LEFTSCROLLBAR )	tLogfile.fwriteln( "   WS_EX_LEFTSCROLLBAR(0x00004000)" );	\
	if( dwExStyle & WS_EX_RIGHTSCROLLBAR )	tLogfile.fwriteln( "   WS_EX_RIGHTSCROLLBAR(0x00000000)" );	\
	if( dwExStyle & WS_EX_CONTROLPARENT )	tLogfile.fwriteln( "   WS_EX_CONTROLPARENT(0x00010000)" );	\
	if( dwExStyle & WS_EX_STATICEDGE )	tLogfile.fwriteln( "   WS_EX_STATICEDGE(0x00020000)" );	\
	if( dwExStyle & WS_EX_APPWINDOW )	tLogfile.fwriteln( "   WS_EX_APPWINDOW(0x00040000)" );	\
	if( dwExStyle & WS_EX_LAYERED )	tLogfile.fwriteln( "   WS_EX_LAYERED(0x00080000)" );	\
	if( dwExStyle & WS_EX_NOINHERITLAYOUT )	tLogfile.fwriteln( "   WS_EX_NOINHERITLAYOUT(0x00100000)" );	\
	if( dwExStyle & WS_EX_LAYOUTRTL )	tLogfile.fwriteln( "   WS_EX_LAYOUTRTL(0x00400000)" );	\
	if( dwExStyle & WS_EX_COMPOSITED )	tLogfile.fwriteln( "   WS_EX_COMPOSITED(0x02000000)" );	\
	if( dwExStyle & WS_EX_NOACTIVATE )	tLogfile.fwriteln( "   WS_EX_NOACTIVATE(0x08000000)" );	\
}

#define	LOG_EXSTYLE( dwExStyle )	\
{	\
	tLogfile.fwriteln( "dwExStyle: %x", dwExStyle );	\
	if( dwExStyle & WS_EX_DLGMODALFRAME )	tLogfile.fwriteln( "   WS_EX_DLGMODALFRAME(0x00000001)" );	\
	if( dwExStyle & WS_EX_NOPARENTNOTIFY )	tLogfile.fwriteln( "   WS_EX_NOPARENTNOTIFY(0x00000004)" );	\
	if( dwExStyle & WS_EX_TOPMOST )	tLogfile.fwriteln( "   WS_EX_TOPMOST(0x00000008)" );	\
	if( dwExStyle & WS_EX_ACCEPTFILES )	tLogfile.fwriteln( "   WS_EX_ACCEPTFILES(0x00000010)" );	\
	if( dwExStyle & WS_EX_TRANSPARENT )	tLogfile.fwriteln( "   WS_EX_TRANSPARENT(0x00000020)" );	\
	if( dwExStyle & WS_EX_MDICHILD )	tLogfile.fwriteln( "   WS_EX_MDICHILD(0x00000040)" );	\
	if( dwExStyle & WS_EX_TOOLWINDOW )	tLogfile.fwriteln( "   WS_EX_TOOLWINDOW(0x00000080)" );	\
	if( dwExStyle & WS_EX_WINDOWEDGE )	tLogfile.fwriteln( "   WS_EX_WINDOWEDGE(0x00000100)" );	\
	if( dwExStyle & WS_EX_CLIENTEDGE )	tLogfile.fwriteln( "   WS_EX_CLIENTEDGE(0x00000200)" );	\
	if( dwExStyle & WS_EX_CONTEXTHELP )	tLogfile.fwriteln( "   WS_EX_CONTEXTHELP(0x00000400)" );	\
	if( dwExStyle & WS_EX_RIGHT )	tLogfile.fwriteln( "   WS_EX_RIGHT(0x00001000)" );	\
	if( dwExStyle & WS_EX_LEFT )	tLogfile.fwriteln( "   WS_EX_LEFT(0x00000000)" );	\
	if( dwExStyle & WS_EX_RTLREADING )	tLogfile.fwriteln( "   WS_EX_RTLREADING(0x00002000)" );	\
	if( dwExStyle & WS_EX_LTRREADING )	tLogfile.fwriteln( "   WS_EX_LTRREADING(0x00000000)" );	\
	if( dwExStyle & WS_EX_LEFTSCROLLBAR )	tLogfile.fwriteln( "   WS_EX_LEFTSCROLLBAR(0x00004000)" );	\
	if( dwExStyle & WS_EX_RIGHTSCROLLBAR )	tLogfile.fwriteln( "   WS_EX_RIGHTSCROLLBAR(0x00000000)" );	\
	if( dwExStyle & WS_EX_CONTROLPARENT )	tLogfile.fwriteln( "   WS_EX_CONTROLPARENT(0x00010000)" );	\
	if( dwExStyle & WS_EX_STATICEDGE )	tLogfile.fwriteln( "   WS_EX_STATICEDGE(0x00020000)" );	\
	if( dwExStyle & WS_EX_APPWINDOW )	tLogfile.fwriteln( "   WS_EX_APPWINDOW(0x00040000)" );	\
	if( dwExStyle & WS_EX_LAYERED )	tLogfile.fwriteln( "   WS_EX_LAYERED(0x00080000)" );	\
	if( dwExStyle & WS_EX_NOINHERITLAYOUT )	tLogfile.fwriteln( "   WS_EX_NOINHERITLAYOUT(0x00100000)" );	\
	if( dwExStyle & WS_EX_LAYOUTRTL )	tLogfile.fwriteln( "   WS_EX_LAYOUTRTL(0x00400000)" );	\
	if( dwExStyle & WS_EX_COMPOSITED )	tLogfile.fwriteln( "   WS_EX_COMPOSITED(0x02000000)" );	\
	if( dwExStyle & WS_EX_NOACTIVATE )	tLogfile.fwriteln( "   WS_EX_NOACTIVATE(0x08000000)" );	\
}

#define	LOGTOOLINFO( pTI )	\
	tLogfile.fwriteln( "TOOLINFO" );	\
	tLogfile.addindent();	\
	tLogfile.fwriteln( "cbSize  : %1d", (pTI)->cbSize );	\
	tLogfile.fwriteln( "uFlags  : %x", (pTI)->uFlags );	\
	tLogfile.fwriteln( "hwnd    : %x", (pTI)->hwnd );	\
	tLogfile.fwriteln( "uId     : %x", (pTI)->uId );	\
	tLogfile.fwriteln( "rect    : %1d %1d %1d %1d", (pTI)->rect.left, (pTI)->rect.top, (pTI)->rect.right, (pTI)->rect.bottom );	\
	tLogfile.fwriteln( "hinst   : %1d", (pTI)->hinst );	\
	if( (pTI)->lpszText )	\
	tLogfile.fwriteln( "lpszText: %s", xNarrow(LPCTSTR((pTI)->lpszText)) );	\
	tLogfile.fwriteln( "lParam  : %x", (pTI)->lParam );	\
	tLogfile.subindent();

static char* BSTRING(BOOL b)
{
	if( b )
		return "True";
	else
		return "False";
}

#ifdef __COMMENTOUT__
static CString	VK[] =
{
	L"0x00 (0x00)",					L"VK_LBUTTON (0x01)",				L"VK_RBUTTON (0x02)",			L"VK_CANCEL (0x03)",
	L"VK_MBUTTON (0x04)",			L"VK_XBUTTON1 (0x05)",				L"VK_XBUTTON2 (0x06)",			L"0x07 (0x07)",
	L"VK_BACK (0x08)",				L"VK_TAB (0x09)",					L"0x0A (0x0A)",					L"0x0B (0x0B)",
	L"VK_CLEAR (0x0C)",				L"VK_RETURN (0x0D)",				L"0x0E (0x0E)",					L"0x0F (0x0F)",
	L"VK_SHIFT (0x10)",				L"VK_CONTROL (0x11)",				L"VK_MENU (0x12)",				L"VK_PAUSE (0x13)",
	L"VK_CAPITAL (0x14)",			L"VK_KANA (0x15)",					L"0x16 (0x16)",					L"VK_JUNJA (0x17)",
	L"VK_FINAL (0x18)",				L"VK_HANJA (0x19)",					L"0x1A (0x1A)",					L"VK_ESCAPE (0x1B)",
	L"VK_CONVERT (0x1C)",			L"VK_NONCONVERT (0x1D)",			L"VK_ACCEPT (0x1E)",			L"VK_MODECHANGE (0x1F)",
	L"VK_SPACE (0x20)",				L"VK_PRIOR (0x21)",					L"VK_NEXT (0x22)",				L"VK_END (0x23)",
	L"VK_HOME (0x24)",				L"VK_LEFT (0x25)",					L"VK_UP (0x26)",				L"VK_RIGHT (0x27)",
	L"VK_DOWN (0x28)",				L"VK_SELECT (0x29)",				L"VK_PRINT (0x2A)",				L"VK_EXECUTE (0x2B)",
	L"VK_SNAPSHOT (0x2C)",			L"VK_INSERT (0x2D)",				L"VK_DELETE (0x2E)",			L"VK_HELP (0x2F)",
	L"VK_0 (0x30)",					L"VK_1 (0x31)",						L"VK_2 (0x32)",					L"VK_3 (0x33)",
	L"VK_4 (0x34)",					L"VK_5 (0x35)",						L"VK_6 (0x36)",					L"VK_7 (0x37)",
	L"VK_8 (0x38)",					L"VK_9 (0x39)",						L"0x3A (0x3A)",					L"0X3B (0X3B)",
	L"0X3C (0X3C)",					L"0X3D (0X3D)",						L"0X3E (0X3E)",					L"0X3F (0X3F)",
	L"0x40 (0x40)",					L"VK_A (0x41)",						L"VK_B (0x42)",					L"VK_C (0x43)",
	L"VK_D (0x44)",					L"VK_E (0x45)",						L"VK_F (0x46)",					L"VK_G (0x47)",
	L"VK_H (0x48)",					L"VK_I (0x49)",						L"VK_J (0x4A)",					L"VK_K (0x4B)",
	L"VK_L (0x4C)",					L"VK_M (0x4D)",						L"VK_N (0x4E)",					L"VK_O (0x4F)",
	L"VK_P (0x50)",					L"VK_Q (0x51)",						L"VK_R (0x52)",					L"VK_S (0x53)",
	L"VK_T (0x54)",					L"VK_U (0x55)",						L"VK_V (0x56)",					L"VK_W (0x57)",
	L"VK_X (0x58)",					L"VK_Y (0x59)",						L"VK_Z (0x5A)",					L"VK_LWIN (0x5B)",
	L"VK_RWIN (0x5C)",				L"VK_APPS (0x5D)",					L"0x5E (0x5E)",					L"VK_SLEEP (0x5F)",
	L"VK_NUMPAD0 (0x60)",			L"VK_NUMPAD1 (0x61)",				L"VK_NUMPAD2 (0x62)",			L"VK_NUMPAD3 (0x63)",
	L"VK_NUMPAD4 (0x64)",			L"VK_NUMPAD5 (0x65)",				L"VK_NUMPAD6 (0x66)",			L"VK_NUMPAD7 (0x67)",
	L"VK_NUMPAD8 (0x68)",			L"VK_NUMPAD9 (0x69)",				L"VK_MULTIPLY (0x6A)",			L"VK_ADD (0x6B)",
	L"VK_SEPARATOR (0x6C)",			L"VK_SUBTRACT (0x6D)",				L"VK_DECIMAL (0x6E)",			L"VK_DIVIDE (0x6F)",
	L"VK_F1 (0x70)",				L"VK_F2 (0x71)",					L"VK_F3 (0x72)",				L"VK_F4 (0x73)",
	L"VK_F5 (0x74)",				L"VK_F6 (0x75)",					L"VK_F7 (0x76)",				L"VK_F8 (0x77)",
	L"VK_F9 (0x78)",				L"VK_F10 (0x79)",					L"VK_F11 (0x7A)",				L"VK_F12 (0x7B)",
	L"VK_F13 (0x7C)",				L"VK_F14 (0x7D)",					L"VK_F15 (0x7E)",				L"VK_F16 (0x7F)",
	L"VK_F17 (0x80)",				L"VK_F18 (0x81)",					L"VK_F19 (0x82)",				L"VK_F20 (0x83)",
	L"VK_F21 (0x84)",				L"VK_F22 (0x85)",					L"VK_F23 (0x86)",				L"VK_F24 (0x87)",
	L"0x88 (0x88)",					L"0x89 (0x89)",						L"0x8A (0x8A)",					L"0x8B (0x8B)",
	L"0x8C (0x8C)",					L"0x8D (0x8D)",						L"0x8E (0x8E)",					L"0x8F (0x8F)",	
	L"VK_NUMLOCK (0x90)",			L"VK_SCROLL (0x91)",				L"VK_OEM_NEC_EQUAL (0x92)",		L"VK_OEM_FJ_MASSHOU (0x93)",
	L"VK_OEM_FJ_TOUROKU (0x94)",	L"VK_OEM_FJ_LOYA (0x95)",			L"VK_OEM_FJ_ROYA (0x96)",		L"0x97 (0x97)",
	L"0x98 (0x98)",					L"0x99 (0x99)",						L"0x9A (0x9A)",					L"0x9B (0x9B)",
	L"0x9C (0x9C)",					L"0x9D (0x9D)",						L"0x9E (0x9E)",					L"0x9F (0x9F)",
	L"VK_LSHIFT (0xA0)",			L"VK_RSHIFT (0xA1)",				L"VK_LCONTROL (0xA2)",			L"VK_RCONTROL (0xA3)",
	L"VK_LMENU (0xA4)",				L"VK_RMENU (0xA5)",					L"VK_BROWSER_BACK (0xA6)",		L"VK_BROWSER_FORWARD (0xA7)",
	L"VK_BROWSER_REFRESH (0xA8)",	L"VK_BROWSER_STOP (0xA9)",			L"VK_BROWSER_SEARCH (0xAA)",	L"VK_BROWSER_FAVORITES (0xAB)",
	L"VK_BROWSER_HOME (0xAC)",		L"VK_VOLUME_MUTE (0xAD)",			L"VK_VOLUME_DOWN (0xAE)",		L"VK_VOLUME_UP (0xAF)",
	L"VK_MEDIA_NEXT_TRACK (0xB0)",	L"VK_MEDIA_PREV_TRACK (0xB1)",		L"VK_MEDIA_STOP (0xB2)",		L"VK_MEDIA_PLAY_PAUSE (0xB3)",
	L"VK_LAUNCH_MAIL (0xB4)",		L"VK_LAUNCH_MEDIA_SELECT (0xB5)",	L"VK_LAUNCH_APP1 (0xB6)",		L"VK_LAUNCH_APP2 (0xB7)",
	L"0xB8 (0xB8)",					L"0xB9 (0xB9)",						L"VK_OEM_1 (0xBA)",				L"VK_OEM_PLUS (0xBB)",
	L"VK_OEM_COMMA (0xBC)",			L"VK_OEM_MINUS (0xBD)",				L"VK_OEM_PERIOD (0xBE)",		L"VK_OEM_2 (0xBF)",
	L"VK_OEM_3 (0xC0)",				L"0xC1 (0xC1)",						L"0xC2 (0xC2)",					L"0xC3 (0xC3)",
	L"0xC4 (0xC4)",					L"0xC5 (0xC5)",						L"0xC6 (0xC6)",					L"0xC7 (0xC7)",
	L"0xC8 (0xC8)",					L"0xC9 (0xC9)",						L"0xCA (0xCA)",					L"0xCB (0xCB)",
	L"0xCC (0xCC)",					L"0xCD (0xCD)",						L"0xCE (0xCE)",					L"0xCF (0xCF)",
	L"0xD0 (0xD0)",					L"0xD1 (0xD1)",						L"0xD2 (0xD2)",					L"0xD3 (0xD3)",
	L"0xD4 (0xD4)",					L"0xD5 (0xD5)",						L"0xD6 (0xD6)",					L"0xD7 (0xD7)",
	L"0xD8 (0xD8)",					L"0xD9 (0xD9)",						L"0xDA (0xDA)",					L"VK_OEM_4 (0xDB)",
	L"VK_OEM_5 (0xDC)",				L"VK_OEM_6 (0xDD)",					L"VK_OEM_7 (0xDE)",				L"VK_OEM_8 (0xDF)",
	L"0xE0 (0xE0)",					L"VK_OEM_AX (0xE1)",				L"VK_OEM_102 (0xE2)",			L"VK_ICO_HELP (0xE3)",
	L"VK_ICO_00 (0xE4)",			L"VK_PROCESSKEY (0xE5)",			L"VK_ICO_CLEAR (0xE6)",			L"VK_PACKET (0xE7)",
	L"0xE8 (0xE8)",					L"VK_OEM_RESET (0xE9)",				L"VK_OEM_JUMP (0xEA)",			L"VK_OEM_PA1 (0xEB)",
	L"VK_OEM_PA2 (0xEC)",			L"VK_OEM_PA3 (0xED)",				L"VK_OEM_WSCTRL (0xEE)",		L"VK_OEM_CUSEL (0xEF)",
	L"VK_OEM_ATTN (0xF0)",			L"VK_OEM_FINISH (0xF1)",			L"VK_OEM_COPY (0xF2)",			L"VK_OEM_AUTO (0xF3)",
	L"VK_OEM_ENLW (0xF4)",			L"VK_OEM_BACKTAB (0xF5)",			L"VK_ATTN (0xF6)",				L"VK_CRSEL (0xF7)",
	L"VK_EXSEL (0xF8)",				L"VK_EREOF (0xF9)",					L"VK_PLAY (0xFA)",				L"VK_ZOOM (0xFB)",
	L"VK_NONAME (0xFC)",			L"VK_PA1 (0xFD)",					L"VK_OEM_CLEAR (0xFE)",			L"0xFF (0xFF)",
};
#endif

#ifndef zPRGMSG
#define zPRGSTR1( x )	#x
#define zPRGSTR2( x )	zPRGSTR1( x )
#define zPRGMSG( msg )	"     Line(" zPRGSTR2(__LINE__) "): " msg
#endif

// Add these to StdAfx.cpp
//
//void LOG_enter(char* function)
//{
//	tLogfile.fwriteln("%s", function);
//	tLogfile.addindent();
//}
//
//void LOG_exit()
//{
//	tLogfile.subindent();
//}

#endif // LOGFILE_H
