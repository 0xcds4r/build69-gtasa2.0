#include <pthread.h>

#include "main.h"
#include "game/game.h"
#include "audiostream.h"

char g_szAudioStreamUrl[256];
float g_fAudioStreamX;
float g_fAudioStreamY;
float g_fAudioStreamZ;
float g_fAudioStreamRadius;
bool g_audioStreamUsePos;
bool g_bAudioStreamStop;
pthread_t g_bAudioStreamThreadWorked;
uintptr_t bassStream;
	
void *audioStreamThread(void *p)
{
	g_bAudioStreamThreadWorked = 1;

	if ( bassStream )
	{
		BASS_ChannelStop(bassStream);
		bassStream = 0;
	}

	bassStream = BASS_StreamCreateURL(g_szAudioStreamUrl, 0, 9699328, 0);
	BASS_ChannelPlay(bassStream);

	while ( !g_bAudioStreamStop ) {
		usleep(2000);
	}

	BASS_ChannelStop(bassStream);

	bassStream = 0;
	g_bAudioStreamThreadWorked = 0;

	pthread_exit(0);
}

void CAudioStream::Initialize()
{
	bassStream = 0;
	BASS_Free();

	if ( BASS_Init(-1, 44100, 0) )
	{
		BASS_SetConfigPtr(16, "SA-MP/0.3");
		BASS_SetConfig(21, 1);
		BASS_SetConfig(11, 10000);
	}
}

void CAudioStream::Process()
{
	// todo
}

void CAudioStream::Play(const char* szURL, float X, float Y, float Z, float Radius, bool bUsePos)
{
	if ( g_bAudioStreamThreadWorked )
	{
		g_bAudioStreamStop = 1;

		do {
			usleep(2000);
		}
		while ( g_bAudioStreamThreadWorked );

		BASS_StreamFree(bassStream);
		bassStream = 0;
	}

	if ( bassStream )
	{
		BASS_StreamFree(bassStream);
		bassStream = 0;
	}

	memset(g_szAudioStreamUrl, 0, sizeof(g_szAudioStreamUrl));
	strncpy(g_szAudioStreamUrl, szURL, sizeof(g_szAudioStreamUrl));

	g_fAudioStreamX = X;
	g_fAudioStreamY = Y;
	g_fAudioStreamZ = Z;
	g_fAudioStreamRadius = Radius;
	g_audioStreamUsePos = bUsePos;
	g_bAudioStreamStop = 0;

	pthread_create(&g_bAudioStreamThreadWorked, 0, audioStreamThread, 0);
}

void CAudioStream::Stop(bool bStop)
{
	if ( g_bAudioStreamThreadWorked )
	{
		g_bAudioStreamStop = 1;
		if ( bStop )
		{
			do {
				usleep(200);
			}
			while ( g_bAudioStreamThreadWorked );
		}

		BASS_StreamFree(bassStream);
		bassStream = 0;
	}
}

bool CAudioStream::IsStopped()
{
	return g_bAudioStreamStop;
}

int CAudioStream::GetStatus()
{
	return BASS_ChannelIsActive(bassStream);
}