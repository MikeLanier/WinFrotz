/*
 * win_smpl.c
 *
 * Visual C interface, sound support (windows)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <mmsystem.h>
#include <wingdi.h>
#include "../frotz/frotz.h"
#include "..\include\logfile_c.h"
#ifndef	_CONSOLE
#include "../include/crosscomp.h"
#include "../include/windefs.h"
#endif

/* ========================================================================== *\


			  S O U N D   R O U T I N E S


\* ========================================================================== */

#define MIN_HEADER_SIZE 10		/* Minimum size of a sound file (the header itself */

extern char stripped_story_name[];
extern char data_path[];

int user_min_volume = 0;
int user_max_volume = 10;

int	play_sounds = 1, has_played_sound = 0;

static struct {
    WORD prefix;
    BYTE repeats;
    BYTE base_note;
    WORD frequency;
    WORD unused;
    WORD length;
} sheader;

static int current_sample = 0;
HWAVEOUT hWaveOut = 0;
static WAVEHDR  WaveHDR;
static DWORD	dwStartVolume=0;
static BOOL		playing_sound = FALSE;

static unsigned char play_part = 0;

static char *sample_data = NULL;

/*
 * display_mmsystem_error
 *
 * Wrapper function for errors occuring during mmsystem calls; print out the error if there is one
 *
 */

void display_mmsystem_error( MMRESULT mmResult )
{
	//LOG_ENTER
	if( mmResult !=MMSYSERR_NOERROR )
	{
		switch( mmResult )
		{
			case MMSYSERR_ERROR:
				print_string("Sound error: Unspecified Windows error." );
				break;
			case MMSYSERR_ALLOCATED:
				print_string("Sound error: Specified resource is already allocated." );
				break;
			case MMSYSERR_BADDEVICEID:
				print_string("Sound error: Specified device identifier is out of range." );
				break;
			case MMSYSERR_NOTENABLED:
				print_string("Sound error: Specified driver failed enable." );
				break;
			case MMSYSERR_INVALHANDLE:	
				print_string("Sound error: Specified device handle is invalid." );
				break;
			case MMSYSERR_NODRIVER:
				print_string("Sound error: No device driver is present." );
				break;
			case MMSYSERR_NOMEM:
				print_string("Sound error: Unable to allocate or lock memory." );
				break;
			case MMSYSERR_NOTSUPPORTED:
				print_string("Sound error: function isn't supported." );
				break;
			case MMSYSERR_BADERRNUM:
				print_string("Sound error: error value out of range." );
				break;
			case WAVERR_UNPREPARED:
				print_string("Sound error: The data block pointed to by the pwh parameter hasn't been prepared." );
				break;
			case WAVERR_BADFORMAT:
				print_string("Sound error: Attempted to open with an unsupported waveform-audio format." );
				break;
			case WAVERR_SYNC:
				print_string("Sound error: The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag." );
				break;
			case MMSYSERR_INVALFLAG:
				print_string("Sound error: invalid flag passed." );
				break;
			case MMSYSERR_INVALPARAM:
				print_string("Sound error: invalid parameter passed." );
				break;
			case MMSYSERR_HANDLEBUSY:
				print_string("Sound error: handle being used simultaneously on another thread." );
				break;
			default:
			{
				char error[256];
				sprintf( error, "Unexpected mmsystem error %d when using a sound function.", mmResult );
				print_string( error );
				break;
			}
		}
		new_line();
	}
	//LOG_EXIT
}

/*
 * init_sound
 *
 * Initialise the sound board and various sound related variables.
 *
 */

int init_sound (void)
{
	//LOG_ENTER
	if( !play_sounds )
	{
	//LOG_EXIT
		return 0;
	}

	if( user_max_volume > 10 || user_max_volume < 0 )
		user_max_volume = 10;

	/* Save the volume setting so we can put it back later */
	if( hWaveOut )
		display_mmsystem_error( waveOutGetVolume( hWaveOut, &dwStartVolume ) );
	//LOG_EXIT
    return 1;
}/* init_sound */

/*
 * reset_sound
 *
 * Free resources allocated for playing samples.
 *
 */

void reset_sound(void)
{
	//LOG_ENTER
	os_stop_sample();

    if (sample_data != NULL)
		free(sample_data);

    sample_data = NULL;

	current_sample = 0;

	//LOG_EXIT
}/* reset_sound */

/*
 * os_beep
 *
 * Play a beep sound. Ideally, the sound should be high- (number == 1)
 * or low-pitched (number == 2).
 *
 */

void os_beep (int number)
{
	//LOG_ENTER
	if( number == 2 )
		MessageBeep( MB_OK );
	else
		MessageBeep( MB_ICONASTERISK );
	//LOG_EXIT
}/* os_beep */

/*
 * os_prepare_sample
 *
 * Load the given sample from the disk.
 *
 */

void os_prepare_sample (int number)
{
    char sample_name[MAX_FILE_NAME + 1];
	char error[256], tmp_path[512];
    FILE *fp;
    int i;
	unsigned int filelen;
	unsigned char lobyte, hibyte;

	//LOG_ENTER
    if (current_sample == number || !play_sounds )
	{
	//LOG_EXIT
		return;
	}

	has_played_sound = 1;

    reset_sound();

    /* Build sample file name */
    strcpy (sample_name, "sound\\");

    for (i = 0; stripped_story_name[i] != '.' && stripped_story_name[i] != 0 && i < 6; i++)
	sample_name[6 + i] = stripped_story_name[i];

    sample_name[6 + i] = '0' + number / 10;
    sample_name[7 + i] = '0' + number % 10;

    strcpy (sample_name + 8 + i, ".snd");

    /* Open sample file */

    if ((fp = fopen (sample_name, "rb")) == NULL)
	{
		/* Windows can do weird things about directories when you drag and drop. We just checked
		   out current dir, let's check the path where the .dat file came from for the sound too*/
		strcpy( tmp_path, data_path );
		strcat( tmp_path, sample_name );
		if( (fp = fopen( tmp_path, "rb"))==NULL )
		{
			sprintf( tmp_path, "Could not find sound file in either %s or %s", sample_name, tmp_path );
			print_string( error );
			new_line();
	//LOG_EXIT
			return;
		}
	}

	/* Find out how long the file is for error checking later */
	fseek( fp, 0, SEEK_END );
	filelen = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	/* If it's shorter than this, it has to be corrupt (minimum size of sound header) */
	if( filelen < MIN_HEADER_SIZE )
	{
		fclose( fp );
		sprintf( error, "Sound file %s is too short to contain a sound!", sample_name );
		os_fatal( error );
	}
    /* Load header */

	/* MSB is first, LSB is second - Might as well do this device independent in case it
	   might work on an Alpha or something */

	hibyte = fgetc( fp );
	lobyte = fgetc( fp );
	sheader.prefix = (hibyte << 8) + lobyte;

	if( sheader.prefix > filelen )
	{
		fclose( fp );
		sprintf( error, "Sound file %s appears to be short", sample_name );
		os_fatal( error );
	}

	sheader.repeats = (unsigned char)fgetc( fp );
	sheader.base_note = (unsigned char)fgetc( fp );

	hibyte = fgetc( fp );
	lobyte = fgetc( fp );
	sheader.frequency= (hibyte << 8) + lobyte;

	/* Skip the unused two bytes */
	hibyte = fgetc( fp );
	hibyte = fgetc( fp );


	hibyte = fgetc( fp );
	lobyte = fgetc( fp );
	sheader.length = ( hibyte << 8 ) + lobyte;

	if( feof( fp ) )
	{
		fclose( fp );
		sprintf( error, "EOF reached reading header of sound file %s", sample_name );
		os_fatal( error );
	}

    /* Allocate memory for sample data */

    if ((sample_data = (char *)calloc (1, sheader.length)) != NULL) 
	{
		/* Read sample data */
		
	PROBE
		if( (fread(sample_data, 1, sheader.length, fp))!=sheader.length )
		{
			fclose( fp );
			sprintf( error, "Error reading sound data from file %s", sample_name );
			os_fatal( error );
		}

		/* Store sample number */
		current_sample = number;
    } 
	else 
		print_string ("\n[Not enough memory for sound]\n");

    /* Close sample file */

    fclose (fp);
	//LOG_EXIT
}/* os_prepare_sample */

/*
 * os_start_sample
 *
 * Play the given sample at the given volume (ranging from 1 to 8 and
 * 255 meaning a default volume). The sound is played once or several
 * times in the background (255 meaning forever). In Z-code 3, the
 * repeats value is always 0 and the number of repeats is taken from
 * the sound file itself. The end_of_sound function is called as soon
 * as the sound finishes.
 *
 */

void os_start_sample (int number, int volume, int repeats)
{
	WAVEFORMATEX	WaveFormat;
	MMRESULT		mmResult;
	DWORD			dwVolume, dwTempVolume;
	WAVEOUTCAPS		woc;

	//LOG_ENTER
	memset( &WaveFormat, 0, sizeof( WAVEFORMATEX) );
	memset( &woc, 0, sizeof( WAVEOUTCAPS ) );

    os_prepare_sample(number);
    if (!current_sample)
	{
	//LOG_EXIT
		return;
	}

	os_stop_sample();

	memset( &WaveHDR, 0, sizeof( WAVEHDR) );

	/* Set this up for PCM, 1 channel, 8 bits unsigned samples */
	WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	WaveFormat.nChannels = 1;
	WaveFormat.nSamplesPerSec = sheader.frequency;
	WaveFormat.nAvgBytesPerSec = sheader.frequency;
	WaveFormat.nBlockAlign = 1;
	WaveFormat.wBitsPerSample = 8;
	WaveFormat.cbSize = 0;

	if( !hWaveOut )
	{
		mmResult = waveOutOpen( &hWaveOut, WAVE_MAPPER, &WaveFormat, 0, 0, CALLBACK_NULL );
		if( mmResult !=MMSYSERR_NOERROR )
		{
			reset_sound();
			display_mmsystem_error( mmResult );
		}
		if( !hWaveOut )
		{
			char szDriverWarn[512];

			if( !woc.szPname[0] )
				sprintf( szDriverWarn, "WinFrotz cannot find a Windows sound driver on your system.\nNo sounds will be played.", "WinFrotz", MB_ICONEXCLAMATION );
			else
				sprintf( szDriverWarn, "WinFrotz cannot open the sound driver\n\"%s\".\n\nNo sounds will be played.", "WinFrotz", MB_ICONEXCLAMATION );

			MessageBox( NULL, szDriverWarn, "WinFrotz", MB_ICONEXCLAMATION );
	//LOG_EXIT
			return;
		}
	}

	display_mmsystem_error( waveOutGetDevCaps( (unsigned int)hWaveOut, &woc, sizeof( WAVEOUTCAPS ) ));

	if( volume < user_min_volume )
		volume = user_min_volume;
	if( volume > user_max_volume )
		volume = user_max_volume;

	if( woc.dwSupport & WAVECAPS_VOLUME )
	{
		if( !dwStartVolume )
			display_mmsystem_error( waveOutGetVolume( hWaveOut, &dwStartVolume ) );

		/* Hiword is the right channel, low word is the left channel */
		dwVolume = HIWORD( dwStartVolume ) / 10 * volume * 65536;
		dwVolume += (LOWORD(dwStartVolume) / 10) * volume;

		display_mmsystem_error( waveOutSetVolume( hWaveOut, dwVolume ) );

		/* It's possible this wave device doesn't support 16 bits of volume control, so we'll check
		   the result of the set with waveOutGetVolume, if it's less than what we set, we know the
		   new max and we'll scale to that */

		display_mmsystem_error( waveOutGetVolume( hWaveOut, &dwTempVolume ) );
		if( dwTempVolume < dwVolume )
		{
			float percentage = ((float)dwTempVolume/dwVolume);
			dwVolume = (unsigned long)(dwVolume * percentage);
			display_mmsystem_error( waveOutSetVolume( hWaveOut, dwVolume ) );
		}
	}

	WaveHDR.lpData = (LPSTR)sample_data;
	WaveHDR.dwBufferLength = sheader.length;
	/* Stefan says this is all done by Frotz code now as of 2.32 */
//	WaveHDR.dwLoops = sheader.repeats;
//	if( WaveHDR.dwLoops == 0 )
//		WaveHDR.dwLoops = 255;
//    if (h_version >= V5)
		WaveHDR.dwLoops = repeats;

	WaveHDR.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
	mmResult = waveOutPrepareHeader( hWaveOut, &WaveHDR, sizeof( WAVEHDR));
	if( mmResult !=MMSYSERR_NOERROR )
	{
		reset_sound();
		display_mmsystem_error( mmResult );
	}

	/* os_wait_sample would be the right thing to do, but see that function for a description
	   of why it isn't called here. 
	
	if( playing_sound )		
		os_wait_sample(); */

	mmResult = waveOutWrite(hWaveOut,&WaveHDR,sizeof( WAVEHDR));
	if( mmResult !=MMSYSERR_NOERROR )
	{
		reset_sound();
		display_mmsystem_error( mmResult );
	}
	else
		playing_sound = TRUE;
	//LOG_EXIT
}/* os_start_sample */


/*
 * os_stop_sample
 *
 * Turn off the current sample.
 *
 */

void os_stop_sample (void)
{
	//LOG_ENTER
	if( hWaveOut )
	{
		if( dwStartVolume )
			display_mmsystem_error( waveOutSetVolume( hWaveOut, dwStartVolume ) );
		dwStartVolume = 0;

		display_mmsystem_error( waveOutReset( hWaveOut ) );
		if( WaveHDR.lpData )
		{
			waveOutUnprepareHeader( hWaveOut, &WaveHDR,	sizeof(WAVEHDR) );
			memset( &WaveHDR, 0, sizeof( WAVEHDR ) );
		}
		display_mmsystem_error( waveOutClose( hWaveOut ) );
		hWaveOut = 0;
		playing_sound = FALSE;
	}
	//LOG_EXIT
}/* os_stop_sample */

/*
 * os_finish_with_sample
 *
 * Clear the wave header and reset the wave device, clearing the handle as well
 *
 */

void os_finish_with_sample (void)
{
	//LOG_ENTER
	reset_sound();
	//LOG_EXIT
}/* os_finish_with_sample */

/*
 * os_wait_sample
 *
 * Stop repeating the current sample and wait until it finishes.
 *
 */

void os_wait_sample (void)
{
	//LOG_ENTER
	/*MMTIME	mmTime;*/

	/* I'd like for this work, but it doesn't - the Soundblaster drivers for Windows crash with an 
	   exception. */

/*	mmTime.u.cb = 0;
	mmTime.wType = TIME_BYTES;
	while( mmTime.u.cb < sheader.length && hWaveOut!=0 )
		display_mmsystem_error( waveOutGetPosition(hWaveOut, &mmTime, sizeof(MMTIME)));	*/
	os_stop_sample();
	//LOG_EXIT
}/* os_wait_sample */

