#include "SoundEffectsPlayer.h"
#include "SoundEffectsLibrary.h"
#include <iostream>

SoundEffectsPlayer::SoundEffectsPlayer(Node* owner) : Component(owner)
{
	alGenSources(1, &p_Source);
	alSourcei(p_Source, AL_BUFFER, p_Buffer);
	//sound1 = SE_LOAD("..\\ProjectAV\\Models\\turn.ogg");
	

}

void SoundEffectsPlayer::Update(float dt)
{
	SetPosition(
		pOwner->GetLocalPosition().x,
		pOwner->GetLocalPosition().y,
		pOwner->GetLocalPosition().z
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
