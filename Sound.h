#ifndef _MY_SOUND_H_
#define _MY_SOUND_H_

#include <stdlib.h>
#include <MyAlutLib.h>
#include "Vector.h"

class Sound
{
private:
	ALuint buffer;
	ALuint source;
public:
	Sound();
	~Sound();
	bool LoadBuffer( ALuint buff, ALfloat dec, ALboolean loop = AL_FALSE );
	bool LoadWave( const char* path, ALfloat dec = 1.0f, ALboolean loop = AL_FALSE );
	void Release();
	void SetPosition( float x, float y, float z );
	void SetPosition( Vector3f pos );
	void SetVelocity( float x, float y, float z );
	void SetVelocity( Vector3f vel );
	void Play();
	void Pause();
	void Stop();
	ALuint GetBuffer(){ return buffer; };
};

#endif