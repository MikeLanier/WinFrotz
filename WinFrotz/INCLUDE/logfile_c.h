#pragma once

extern void LOG_enter(char* function, char* file, int line, int count);
extern void LOG_exit();
extern void LOG_write( const char *format, ... );

#define NXCOUNT	20000
static int xcount[NXCOUNT] = {0};

#define LOG_ENTER LOG_enter(__FUNCTION__, __FILE__, __LINE__, ++xcount[__LINE__]);
#define LOG_EXIT LOG_exit();

#define	PROBE		LOG_write( "PROBE: %s[%1d]: %s", __FUNCTION__, __LINE__, __FILE__ );
