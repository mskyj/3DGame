#include "Sound.h"

Sound::Sound()
{
}

Sound::~Sound()
{
}

bool Sound::LoadBuffer( ALuint buff, ALfloat dec, ALboolean loop )
{
	buffer = buff;

	// Bind the buffer with the source.
	alGenSources(1, &source);

	if(alGetError() != AL_NO_ERROR) return AL_FALSE;

	ALfloat pos[] = {0.0f, 0.0f, 0.0f};
	ALfloat vel[] = {0.0f, 0.0f, 0.0f};

	//�\�[�X�̏����ݒ�
	alSourcei (source, AL_BUFFER,   buffer );//����
	alSourcef (source, AL_PITCH,    1.0f   );//�s�b�`
	alSourcef (source, AL_GAIN,     1.0f   );//�Q�C��
	alSourcefv(source, AL_POSITION, pos    );//���W
	alSourcefv(source, AL_VELOCITY, vel    );//�ړ�
	alSourcei (source, AL_LOOPING,  loop   );//�J��Ԃ�
	alSourcef( source, AL_REFERENCE_DISTANCE, 5.0f*dec );//���W�n�ɂ�錸��

	// Do an error check and return.
	if(alGetError() != AL_NO_ERROR) return AL_FALSE;

	return AL_TRUE;
}

bool Sound::LoadWave( const char* path, ALfloat dec, ALboolean loop )
{
	// Load wav data into a buffer.
	buffer = alutCreateBufferFromFile(path);

	if(alGetError() != AL_NO_ERROR) return AL_FALSE;

	return LoadBuffer(buffer, dec, loop);
}

void Sound::Release()
{
	if( alIsBuffer(buffer) )
		alDeleteBuffers(1, &buffer);
	if( alIsSource(source) )
		alDeleteSources(1, &source);
}

void Sound::SetPosition( float x, float y, float z )
{
	ALfloat pos[] = {x,y,z};
	alSourcefv(source, AL_POSITION, pos);
}

void Sound::SetPosition( Vector3f pos )
{
	SetPosition(pos.x, pos.y, pos.z);
}

void Sound::SetVelocity( float x, float y, float z )
{
	ALfloat vel[] = {x,y,z};
	alSourcefv(source, AL_VELOCITY, vel);
}

void Sound::SetVelocity( Vector3f vel )
{
	SetVelocity(vel.x, vel.y, vel.z);
}

void Sound::Play()
{
	alSourcePlay(source);
}

void Sound::Pause()
{
	alSourcePause(source);
}

void Sound::Stop()
{
	alSourceStop(source);
}