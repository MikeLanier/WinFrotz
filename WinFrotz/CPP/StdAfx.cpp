// stdafx.cpp : source file that includes just the standard includes
//	WinFrotz.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "../include/stdafx.h"

extern "C" void LOG_enter(char* function, char* file, int line, int count)
{
	tLogfile.fwriteln("%s(%1d) [%1d]: %s =====>", function, line, count, file);
	tLogfile.addindent();
}

extern "C" void LOG_exit()
{
	tLogfile.subindent();
}

extern "C" void LOG_write( const char *format, ... )
{
	char	buff[819200];
	va_list	argptr;

	va_start( argptr, format );
	vsprintf( buff, format, argptr );

	tLogfile.fwriteln("%s", buff);
}
