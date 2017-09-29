// CSoundOpt.cpp : implementation file
//

//*--------------------------------------------------------------------------
//| CSoundOpt allows the control of the (very simple) sound capabilities of
//| the Z-Machine. Actual sound play code is in the C module win_smpl.c
//*--------------------------------------------------------------------------

#include "../include/stdafx.h"
#include "../include/winfrotz.h"
#include "../include/CSoundOpt.h"
#include "../include/mmsystem.h"
#include "../include/windefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void WriteRegDWORD( HKEY hkInput, char *item, DWORD value);

extern "C"	int	play_sounds;
extern "C"	int user_max_volume;
extern "C"	int has_played_sound;
extern "C"	HWAVEOUT	hWaveOut;
extern "C"  void display_mmsystem_error( MMRESULT mmResult );

/////////////////////////////////////////////////////////////////////////////
// CSoundOpt dialog


CSoundOpt::CSoundOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundOpt::IDD, pParent)
{
	//LOGFUNCTION
	//{{AFX_DATA_INIT(CSoundOpt)
	m_SoundEnabled = FALSE;
	//}}AFX_DATA_INIT
}


void CSoundOpt::DoDataExchange(CDataExchange* pDX)
{
	//LOGFUNCTION
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoundOpt)
	DDX_Control(pDX, IDC_VOLUME, m_VolSlider);
	DDX_Check(pDX, IDC_SOUND_ENABLED, m_SoundEnabled);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSoundOpt, CDialog)
	//{{AFX_MSG_MAP(CSoundOpt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundOpt message handlers

BOOL CSoundOpt::OnInitDialog() 
{
	//LOGFUNCTION
	WAVEFORMATEX	WaveFormat;
	WAVEOUTCAPS		woc;
	MMRESULT		mmResult;
	
	memset( &WaveFormat, 0, sizeof( WAVEFORMATEX) );
	memset( &woc, 0, sizeof( WAVEOUTCAPS ) );

	/* Typical settings for a Z-Machine game */
	WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	WaveFormat.nChannels = 1;
	WaveFormat.nSamplesPerSec = 8000;
	WaveFormat.nAvgBytesPerSec = 8000;
	WaveFormat.nBlockAlign = 1;
	WaveFormat.wBitsPerSample = 8;
	WaveFormat.cbSize = 0;

	if( !hWaveOut )
	{
		mmResult = waveOutOpen( &hWaveOut, WAVE_MAPPER, &WaveFormat, 0, 0, CALLBACK_NULL );

		//In case of failure here the default dialog appearance will indicate that WinFrotz
		//couldn't figure out the wave device (it would have to be pretty broken to fail this)
		
		if( mmResult !=MMSYSERR_NOERROR )
			display_mmsystem_error( mmResult );
		
		if( hWaveOut )
		{
			waveOutGetDevCaps( (unsigned int)hWaveOut, &woc, sizeof( WAVEOUTCAPS ));

			if( !woc.szPname[0] )
				SetDlgItemText( IDC_SOUND_DEVICE, "Opened but unknown name." );
			else
				SetDlgItemText( IDC_SOUND_DEVICE, woc.szPname );

			if( woc.dwSupport & WAVECAPS_VOLUME )
				SetDlgItemText( IDC_SUPPORT_VOLUME, "Yes" );
			else
				SetDlgItemText( IDC_SUPPORT_VOLUME, "No " );

			waveOutClose( hWaveOut );
			hWaveOut = NULL;
		}
	}
	else
	{
		//If the wave out device is already open, we wouldn't be able to re-open it anyway,
		//so we'll go ahead and grab values but not close it (let win_smpl.c handle that).
		waveOutGetDevCaps( (unsigned int)hWaveOut, &woc, sizeof( WAVEOUTCAPS ));

		if( !woc.szPname[0] )
			SetDlgItemText( IDC_SOUND_DEVICE, "Opened but unknown name." );
		else
			SetDlgItemText( IDC_SOUND_DEVICE, woc.szPname );

		if( woc.dwSupport & WAVECAPS_VOLUME )
			SetDlgItemText( IDC_SUPPORT_VOLUME, "Yes" );
		else
			SetDlgItemText( IDC_SUPPORT_VOLUME, "No " );
	}

	//Has this particular story file attempted to play a sound or not?
	if( has_played_sound )
		SetDlgItemText( IDC_HAS_PLAYED_SOUND, "Yes" );
	else
		SetDlgItemText( IDC_HAS_PLAYED_SOUND, "No " );

	//Are sounds themselves enabled for the Z-Machine?
	if( play_sounds )
		m_SoundEnabled = TRUE;
	else
		m_SoundEnabled = FALSE;

	CDialog::OnInitDialog();

	m_VolSlider.SetRange( 0, 10, TRUE );
	m_VolSlider.SetPos( user_max_volume );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSoundOpt::OnOK() 
{
	//LOGFUNCTION
	if( GetCheckedRadioButton( IDC_SOUND_ENABLED, IDC_SOUND_ENABLED ) )
		play_sounds = 1;
	else
		play_sounds = 0;

	user_max_volume = m_VolSlider.GetPos( );
 
	CDialog::OnOK();

	WriteRegDWORD( NULL, ITEM_SOUND_VOLUME, (DWORD)user_max_volume );
	WriteRegDWORD( NULL, ITEM_SOUND_PLAYED, (DWORD)play_sounds );
}
