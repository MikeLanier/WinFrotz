/*
 * getopt.c
 *
 * Replacement for a Unix style getopt function
 *
 */

#include <stdio.h>
#include <string.h>
#include "..\include\logfile_c.h"

#ifndef __MSDOS__
#define cdecl
#endif

int optind = 1;
int optopt = 0;

const char *optarg = NULL;

int cdecl getopt (int argc, char *argv[], const char *options)
{
    static int pos = 1;

    const char *p;

	//LOG_ENTER
    if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == 0)
	{
		//LOG_EXIT
	return EOF;
	}

    optopt = argv[optind][pos++];
    optarg = NULL;

    if (argv[optind][pos] == 0)
	{ pos = 1; optind++; }

    p = strchr (options, optopt);

    if (optopt == ':' || p == NULL) {

	fputs ("illegal option -- ", stderr);
	goto error;

    } else if (p[1] == ':')

	if (optind >= argc) {

	    fputs ("option requires an argument -- ", stderr);
	    goto error;

	} else {

	    optarg = argv[optind];

	    if (pos != 1)
		optarg += pos;

	    pos = 1; optind++;

	}

		//LOG_EXIT
    return optopt;

error:

    fputc (optopt, stderr);
    fputc ('\n', stderr);
		//LOG_EXIT
    return '?';

}/* getopt */
