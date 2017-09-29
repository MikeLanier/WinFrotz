/*
 * hotkey.c
 *
 * Hot key functions
 *
 */

#include "frotz.h"
#include "..\include\logfile_c.h"

extern int restore_undo (void);

extern int read_number (void);

extern bool read_yes_or_no (const char *);

extern void replay_open (void);
extern void replay_close (void);
extern void record_open (void);
extern void record_close (void);

extern void seed_random (int);

char init_name[MAX_FILE_NAME + 1] = DEFAULT_INIT_NAME;
extern int use_aliases;
extern void add_alias(const char* alias, const char* expansion);
extern void clear_aliases(void);
extern void delete_alias(const char*);
extern int load_aliases(const char* filename);
extern void list_aliases(void);
extern void list_aliases(void);
extern void save_aliases(const char* filename);
/*
 * hot_key_alias
 *
 */

static int hot_key_alias (void)
{

	int c;
	char alias[11] = "";
	char expansion[(MAX_FILE_NAME > 79) ? MAX_FILE_NAME+1 : 80] = "";
	char* s;

	//LOG_ENTER
	print_string("Alias A)dd C)lear D)elete L)oad O)");
	print_string(use_aliases ? "ff" : "n");
	print_string(" S)ave V)iew: ");
	c = os_read_key(0, TRUE);
	if (isalnum(c)) print_char(c);
	z_new_line();
	
	switch (tolower(c)) 
	{
	case 'a':
		print_string("Add alias: ");
		while (os_read_line(60, alias, 0, h_screen_width, 0) != 13);
		z_new_line();
		for (s = alias; *s; ++s)
			if (!isalnum(*s))
			{
	//LOG_EXIT
				return 1;
			}
			if (!alias[0])
			{
	//LOG_EXIT
				return 1;
			}
			
			print_string("Expansion: ");
			while (os_read_line(60, expansion, 0, h_screen_width, 0) != 13);
			z_new_line();
			if (!expansion[0])
			{
	//LOG_EXIT
				return 1;
			}
			add_alias(alias, expansion);
			break;
			
	case 'c':
		print_string("Clear all aliases (yes/no): ");
		while (os_read_line(60, alias, 0, h_screen_width, 0) != 13);
		z_new_line();
		if (alias[0] == 'y') {
			clear_aliases();
			print_string("Aliases cleared\n");
		}
		break;
		
	case 'd':
		print_string("Delete alias: ");
		while (os_read_line(60, alias, 0, h_screen_width, 0) != 13);
		z_new_line();
		delete_alias(alias);
		break;
		
	case 'l':
		if (os_read_file_name(expansion, init_name, FILE_LOAD_INIT)) {
			strcpy(init_name, expansion);
			load_aliases(init_name);
		}
		break;
		
	case 'o':
		use_aliases = !use_aliases;
		print_string("Alias expansion o");
		print_string(use_aliases ? "n" : "ff");
		z_new_line();
		break;
		
	case 's':
		if (os_read_file_name(expansion, init_name, FILE_SAVE_INIT)) {
			strcpy(init_name, expansion);
			save_aliases(init_name);
		}
		break;
		
	case 'v':
		list_aliases();
		break;
	}
	//LOG_EXIT
	return 0;
} /* end hot_key_alias */

/*
 * hot_key_debugging
 *
 * ...allows user to toggle cheating options on/off.
 *
 */

static bool hot_key_debugging (void)
{
	//LOG_ENTER

    print_string ("Debugging options\n");

    option_attribute_assignment = read_yes_or_no ("Watch attribute assignment");
    option_attribute_testing = read_yes_or_no ("Watch attribute testing");
    option_object_movement = read_yes_or_no ("Watch object movement");
    option_object_locating = read_yes_or_no ("Watch object locating");

	//LOG_EXIT
    return FALSE;

}/* hot_key_debugging */

/*
 * hot_key_help
 *
 * ...displays a list of all hot keys.
 *
 */

static bool hot_key_help (void) {
	//LOG_ENTER

    print_string ("Help\n");

    print_string (
	"\n"
	"Alt-D  debugging options\n"
	"Alt-H  help\n"
	"Alt-N  new game\n"
	"Alt-P  playback on\n"
	"Alt-R  recording on/off\n"
	"Alt-S  seed random numbers\n"
	"Alt-U  undo one turn\n"
	"Alt-X  exit game\n");

	//LOG_EXIT
    return FALSE;

}/* hot_key_help */

/*
 * hot_key_playback
 *
 * ...allows user to turn playback on.
 *
 */

static bool hot_key_playback (void)
{
	//LOG_ENTER

    print_string ("Playback on\n");

    if (!istream_replay)
	replay_open ();

	//LOG_EXIT
    return FALSE;

}/* hot_key_playback */

/*
 * hot_key_recording
 *
 * ...allows user to turn recording on/off.
 *
 */

static bool hot_key_recording (void)
{
	//LOG_ENTER

    if (istream_replay) {
	print_string ("Playback off\n");
	replay_close ();
    } else if (ostream_record) {
	print_string ("Recording off\n");
	record_close ();
    } else {
	print_string ("Recording on\n");
	record_open ();
    }

	//LOG_EXIT
    return FALSE;

}/* hot_key_recording */

/*
 * hot_key_seed
 *
 * ...allows user to seed the random number seed.
 *
 */

static bool hot_key_seed (void)
{
	//LOG_ENTER

    print_string ("Seed random numbers\n");

    print_string ("Enter seed value (or return to randomize): ");
    seed_random (read_number ());

	//LOG_EXIT
    return FALSE;

}/* hot_key_seed */

/*
 * hot_key_undo
 *
 * ...allows user to undo the previous turn.
 *
 */

static bool hot_key_undo (void)
{
	//LOG_ENTER

    print_string ("Undo one turn\n");

    if (restore_undo ()) {

	if (h_version >= V5) {		/* for V5+ games we must */
	    store (2);			/* store 2 (for success) */
	//LOG_EXIT
	    return TRUE;		/* and abort the input   */
	}

	if (h_version <= V3) {		/* for V3- games we must */
	    z_show_status ();		/* draw the status line  */
	//LOG_EXIT
	    return FALSE;		/* and continue input    */
	}

    } else print_string ("No more undo information available.\n");

	//LOG_EXIT
    return FALSE;

}/* hot_key_undo */

/*
 * hot_key_restart
 *
 * ...allows user to start a new game.
 *
 */

static bool hot_key_restart (void)
{
	//LOG_ENTER

    print_string ("New game\n");

    if (read_yes_or_no ("Do you wish to restart")) {

	z_restart ();
	{
	//LOG_EXIT
	return TRUE;
	}

    } else 
	{
	//LOG_EXIT
		return FALSE;
	}

}/* hot_key_restart */

/*
 * hot_key_quit
 *
 * ...allows user to exit the game.
 *
 */

static bool hot_key_quit (void)
{
	//LOG_ENTER

    print_string ("Exit game\n");

    if (read_yes_or_no ("Do you wish to quit")) {

	z_quit ();
	//LOG_EXIT
	return TRUE;

    } else
	{
	//LOG_EXIT
		return FALSE;
	}

}/* hot_key_quit */

/*
 * handle_hot_key
 *
 * Perform the action associated with a so-called hot key. Return
 * true to abort the current input action.
 *
 */

bool handle_hot_key (zchar key)
{

	//LOG_ENTER
    if (cwin == 0) {

	bool aborting;

	print_string ("\nHot key -- ");

	switch (key) {
	    case ZC_HKEY_RECORD: aborting = hot_key_recording (); break;
	    case ZC_HKEY_PLAYBACK: aborting = hot_key_playback (); break;
	    case ZC_HKEY_SEED: aborting = hot_key_seed (); break;
	    case ZC_HKEY_UNDO: aborting = hot_key_undo (); break;
	    case ZC_HKEY_RESTART: aborting = hot_key_restart (); break;
	    case ZC_HKEY_QUIT: aborting = hot_key_quit (); break;
	    case ZC_HKEY_DEBUG: aborting = hot_key_debugging (); break;
	    case ZC_HKEY_HELP: aborting = hot_key_help (); break;
	}

	if (aborting)
	    return TRUE;

	print_string ("\nContinue input...\n");

    }

	//LOG_EXIT
    return FALSE;

}/* handle_hot_key */
