/*
 * $Id: alias.c 1.6 1996/09/26 04:34:16 Madsen Exp $
 *
 * Alias expansion and management
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "frotz.h"
#include "../INCLUDE/logfile_c.h"

#define ALIAS_SIZE 1024

void delete_alias(const char*);

int use_aliases = 0;            /* True if aliases should be expanded */

/*
 * The alias table is a sequence of null terminated strings:
 *   ALIAS EXPANSION ALIAS EXPANSION ...
 * The aliases are listed in increasing ASCII order.
 * alias_end points to the first unused byte in the table.
 *
 */

char aliases[ALIAS_SIZE] = "";
char* alias_end = aliases;

/*
 * display_int
 *
 * Display a non-negative integer on the screen.
 *
 */

void display_int(int val)
{
  char  buf[sizeof(int)*3 + 1];
  char  *s = buf + sizeof(buf) - 1;
  //LOG_ENTER
  *s = '\0';

  do {
    *(--s) = '0' + val % 10;
    val /= 10;
  } while (val);

  os_display_string(s);
  //LOG_EXIT
} /* end display_int */

/*
 * display_error
 *
 * Displays an error message and increments the error count.
 *
 * If FILENAME is null, only MSG is printed.  If LINE_NUMBER is 0, only
 * the FILENAME and MSG are printed.
 *
 */

static int error_count = 0;

void display_error(const char* msg, const char* filename, int line_number)
{
  //LOG_ENTER
	/* screen_mssg_on and screen_mssg_off were removed by Stefan in 2.32 sources */
  /*screen_mssg_on();*/
  if (filename) {
    os_display_string(filename);
    if (line_number) {
      os_display_string(", line ");
      display_int(line_number);
    }
    os_display_string(": ");
  }
  os_display_string(msg);
  /*screen_mssg_off();*/
  ++error_count;
  //LOG_EXIT
} /* end display_error */

/*
 * find_alias
 *
 * Returns a pointer to ALIAS in the alias table.
 * Returns NULL if ALIAS is not listed.
 *
 */

static char* find_alias(const char* alias)
{
  char* s = aliases;
  //LOG_ENTER

  while (s < alias_end) {
    int  cmp = strcmp(alias, s);
    if (cmp == 0)
	{
	  //LOG_EXIT
      return s;
	}
    else if (cmp < 0)
	{
	  //LOG_EXIT
      return NULL;
	}
    s = strchr(strchr(s,0)+1, 0) + 1;
  }

  //LOG_EXIT
  return NULL;                  /* Not found */
} /* end find_alias */

/*
 * add_alias
 *
 * Add an ALIAS and its EXPANSION to the alias table.
 * If ALIAS already exists, it is replaced.
 *
 */

void add_alias(const char* alias, const char* expansion)
{
  char* s;
  int   size = strlen(alias) + strlen(expansion) + 2;

  //LOG_ENTER
  delete_alias(alias);

  if ((alias_end - aliases) + size > ALIAS_SIZE) {
    display_error("Alias table full",NULL,0);
	  //LOG_EXIT
    return;
  }

  s = aliases;

  while (s < alias_end) {
    if (strcmp(alias, s) < 0)
      break;
    s = strchr(strchr(s,0)+1, 0) + 1;
  }

  if (s < alias_end)
    memmove(s+size, s, alias_end - s);
  strcpy(s,alias);
  strcpy(s+strlen(alias)+1, expansion);
  alias_end += size;

  use_aliases = 1;
  //LOG_EXIT
} /* end add_alias */

/*
 * clear_aliases
 *
 * Clear all aliases from the alias table.
 *
 */

void clear_aliases(void)
{
  //LOG_ENTER
  alias_end = aliases;
  use_aliases = 0;
  //LOG_EXIT
} /* end clear_aliases */

/*
 * delete_alias
 *
 * Remove ALIAS from the alias table.
 * If ALIAS does not exist, nothing happens.
 *
 */

void delete_alias(const char* alias)
{
  char* start;
  char* next;

  //LOG_ENTER
  start = find_alias(alias);

  if (!start) 
  {
	  //LOG_EXIT
	  return;           /* Not found */
  }

  next = strchr(strchr(start,0)+1, 0)+1;

  if (next == alias_end)
    alias_end = start;
  else {
    memmove(start, next, alias_end - next);
    alias_end -= next - start;
  }

  if (aliases == alias_end)
    use_aliases = 0;            /* No more aliases */
  //LOG_EXIT
} /* end delete_alias */

/*
 * expand_aliases
 *
 * BUFFER contains the text to be checked for aliases.
 * MAX_SIZE is the maximum size of the buffer (not counting trailing nul).
 *
 * The expanded text is returned in BUFFER.
 *
 */

void expand_aliases(char* buffer, int max_size)
{
  char  buf2[INPUT_BUFFER_SIZE];
  char  old;
  char* scan = buffer;
  char* out = buf2;
  char* alias;
  char* word;
  char* d;
  int   size_left;

  //LOG_ENTER
  if (aliases == alias_end)
  {
  //LOG_EXIT
    return;                     /* No aliases */
  }

  if (max_size >= INPUT_BUFFER_SIZE)
    max_size = INPUT_BUFFER_SIZE-1;

  size_left = max_size - strlen(buffer);

  while (*scan) {
    if (isalnum(*scan)) {
      word = scan++;
      while (isalnum(*scan))
        ++scan;
      old = *scan;
      *scan = '\0';
      alias = find_alias(word);
      if (alias) {
        *scan = old;
        alias = strchr(alias,0) + 1; /* Find expansion */
        size_left += scan - word;
        for (d = out; *alias && size_left--; ++alias) {
          if (*alias == '%') {
            ++size_left;
            switch (*(++alias)) {
             case 'r':          /* Insert rest of input */
              while (isspace(*scan)) ++scan;
              while (*scan && size_left--)
                *(d++) = *(scan++);
              break;
             case 's':          /* Insert next sentence */
              while (isspace(*scan)) ++scan;
              while (*scan && *scan != '.' && size_left--)
                *(d++) = *(scan++);
              break;
             case 'w':          /* Insert next word */
              while (isspace(*scan)) ++scan;
              while (*scan && *scan != '.' && *scan != ',' && !isspace(*scan)
                     && size_left--)
                *(d++) = *(scan++);
              break;
             case '%':
              ++alias;          /* Counteract decrement in default case */
              /* Fall thru to copy % to expansion */
             default:
              --alias;          /* Following character is normal */
              --size_left;
              *(d++) = '%';
            }
          } else /* not % */
            *(d++) = *alias;
        } /* end for */
        strcpy(d,scan);
        scan = word;
        strcpy(scan, out);
        old = *scan;
      } else {
        strcpy(out, word);
        out += strlen(word);
      }
      *scan = old;
    } else /* not alphanumeric */
      *(out++) = *(scan++);
  } /* end while more chars */

  *out = '\0';
  strcpy(buffer,buf2);
  /* display_string(buf2); display_new_line(); */
  //LOG_EXIT
} /* end expand_aliases */

/*
 * list_aliases
 *
 * Display the contents of the alias table.
 *
 */

void list_aliases(void)
{
  const char* s = aliases;

  //LOG_ENTER
  while (s < alias_end) {
    print_string(s);
    print_string(" -> ");
    s = strchr(s,0)+1;
    print_string(s);
    print_string("\n");
    s = strchr(s,0)+1;
  }
  //LOG_EXIT
} /* end list_aliases */

/*
 * load_aliases
 *
 * Load aliases from an init file named FILENAME.  Returns the number
 * of errors.
 *
 * load_aliases may use the following functions for error messages:
 *   os_display_char     os_message_start   os_beep
 *   os_display_string   os_message_end
 *
 * If your implementation of any of these functions depends on
 * display_new_line, then your os_init_screen MUST call restart_screen
 * BEFORE calling load_aliases.  Remember that display_string("\n")
 * calls display_new_line.
 *
 */

int load_aliases(const char* filename)
{
	FILE* file = fopen(filename, "r");
	int   line_number = 0;
	char  buffer[82];
	int   c;
	char* d;
	char* s;
	
  //LOG_ENTER
	error_count = 0;
	
	if (!file) {
		display_error("No such file",filename,0);
		os_beep(2);
  //LOG_EXIT
		return 1;
	}
	
	while (1) {
		++line_number;
		
		if (!fgets(buffer, sizeof(buffer), file)) {
			if (ferror(file))
				display_error("Error reading line", filename, line_number);
			break;                    /* Exit loop */
		} /* end if couldn't read line */
		
		s = strchr(buffer,'\n');
		if (s) {
			while ((s > buffer) && isspace(s[-1]))
				--s;
			*s = '\0';
		} else {
			display_error("Line too long (80 characters max)",
				filename, line_number);
	PROBE
			do { c = getc(file); } while (c != '\n' && c != EOF);
			continue;
		} /* end else line was too long */
		
		/* Strip leading whitespace and collapse the rest */
		for (d = s = buffer; *s;) {
			*d = *(s++);
			if (isspace(*d)) {
				*d = ' ';
				if (!isspace(*s) && (d > buffer)) ++d;
			} else
				++d;
		} /* end for collapsing spaces */
		
		if (strncmp(buffer,"alias ", 6) == 0) {
			s = strchr(buffer+6,' ');
			if (!s)
				display_error("Missing alias", filename, line_number);
			else {
				*s = '\0';
				add_alias(buffer+6,s+1);
			}
		} else if (strncmp(buffer,"unalias ", 8) == 0)
			delete_alias(buffer + 8);
		else if (strcmp(buffer,"clear") == 0)
			clear_aliases();
		else if (buffer[0] != '\0' && buffer[0] != '%')
			display_error("Invalid command", filename, line_number);
	} /* end while more lines */
	
	fclose(file);
	
	use_aliases = (alias_end > aliases); /* Turn on expansion (if aliases) */
	if (error_count)
		os_beep(2);
  //LOG_EXIT
	return error_count;
} /* end load_aliases */

/*
 * save_aliases
 *
 * Save the current aliases to an init file named FILENAME
 *
 */

void save_aliases(const char* filename)
{
  FILE* file = fopen(filename,"w");
  const char* s = aliases;

  //LOG_ENTER
  if (!file)
  {
  //LOG_EXIT
    return;
  }

  while (s < alias_end) {
    fputs("alias ",file);
    fputs(s,file);
    putc(' ', file);
    s = strchr(s,0)+1;
    fputs(s, file);
    putc('\n', file);
    s = strchr(s,0)+1;
  }

  fclose(file);
  //LOG_EXIT
} /* end save_aliases */
