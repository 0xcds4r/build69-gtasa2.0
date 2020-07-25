#pragma once

#define BASS_ACTIVE_STOPPED 		0
#define BASS_ACTIVE_PLAYING 		1
#define BASS_ACTIVE_PAUSED 			2
#define BASS_ACTIVE_PAUSED_DEVICE 	3
#define BASS_ACTIVE_STALLED			4

class CAudioStream
{	
public:
	CAudioStream() {};
	~CAudioStream() {};
	
	void Initialize();
	void Play(const char*, float, float, float, float, bool);
	void Stop(bool bStop);
	void Process();

	bool IsStopped();
	int GetStatus();
};