#include "SoundEffectsPlayer.h"
#include "SoundEffectsLibrary.h"
#include <iostream>

SoundEffectsPlayer::SoundEffectsPlayer(Node* owner) : Component(owner)
{
	alGenSources(1, &p_Source);
	alSourcei(p_Source, AL_BUFFER, p_Buffer);

		// 1. Make sure the source's position is absolute in the world.
	alSourcei(p_Source, AL_SOURCE_RELATIVE, AL_FALSE);

	// 2. Set the rolloff factor. This controls how quickly the sound fades with distance. 1.0 is a normal rate.
	alSourcef(p_Source, AL_ROLLOFF_FACTOR, 0.7f);

	// 3. Set a reference distance. This is the distance at which the sound will be at 100% volume.
	// Sounds closer than this will not get any louder.
	alSourcef(p_Source, AL_REFERENCE_DISTANCE, 200.0f);

	// 4. Set a max distance. Beyond this distance, the sound will be completely inaudible.
	// This is important for performance, as OpenAL can stop processing sources that are too far away.
	// Set it to a large value to ensure you can hear distant stones.
	alSourcef(p_Source, AL_MAX_DISTANCE, 1000.0f);

	// 5. Ensure gain is not zero.
	alSourcef(p_Source, AL_GAIN, 1.0f);
}

void SoundEffectsPlayer::Update(float dt)
{
	SetPosition(
		pOwner->GetWorldPosition().x,
		pOwner->GetWorldPosition().y,
		pOwner->GetWorldPosition().z
	);
}

void SoundEffectsPlayer::AddSound(std::string path) {
	sound.push_back(SE_LOAD(path.c_str()));
}

SoundEffectsPlayer::~SoundEffectsPlayer()
{
	
	alDeleteSources(1, &p_Source);
}

void SoundEffectsPlayer::Play(int t)
{
	ALuint& buffer_to_play = sound[t];
	if (buffer_to_play != p_Buffer)
	{
		p_Buffer = buffer_to_play;
		alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
	}

	alSourcePlay(p_Source);
}

void SoundEffectsPlayer::Stop()
{
	alSourceStop(p_Source);
}

void SoundEffectsPlayer::Pause()
{
	alSourcePause(p_Source);
}

void SoundEffectsPlayer::Resume()
{
	alSourcePlay(p_Source);
}

void SoundEffectsPlayer::SetBufferToPlay(const ALuint& buffer_to_play)
{
	if (buffer_to_play != p_Buffer)
	{
		p_Buffer = buffer_to_play;
		alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
	}
}

void SoundEffectsPlayer::SetLooping(const bool& loop)
{
	alSourcei(p_Source, AL_LOOPING, (ALint)loop);
}

void SoundEffectsPlayer::SetPosition(const float& x, const float& y, const float& z)
{
	alSource3f(p_Source, AL_POSITION, x, y, z);
}


bool SoundEffectsPlayer::isPlaying()
{
	ALint playState;
	alGetSourcei(p_Source, AL_SOURCE_STATE, &playState);
	return (playState == AL_PLAYING);
}
