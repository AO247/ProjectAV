#include "SoundEffectsPlayer.h"
#include "SoundEffectsLibrary.h"
#include <iostream>

SoundEffectsPlayer::SoundEffectsPlayer(Node* owner, int sourceCount) : Component(owner)
{
	//alGenSources(1, &p_Source);
	
	m_sources.resize(sourceCount);
	alGenSources(sourceCount, m_sources.data());
	alSourcei(p_Source, AL_BUFFER, p_Buffer);

	for (ALuint source : m_sources)
	{
		// Check for errors
		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			// Handle error, maybe log it
			OutputDebugStringA("OpenAL Error during source generation in SoundEffectsPlayer.\n");
			break;
		}

		alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);
		alSourcef(source, AL_REFERENCE_DISTANCE, 8.0f);
		alSourcef(source, AL_MAX_DISTANCE, 500.0f);
		alSourcef(source, AL_GAIN, 1.0f);
	}
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
	soundBuffers.push_back(SE_LOAD(path.c_str()));
}

SoundEffectsPlayer::~SoundEffectsPlayer()
{
	
	alDeleteSources(m_sources.size(), m_sources.data());
}

void SoundEffectsPlayer::Play(int t)
{
	//ALuint& buffer_to_play = sound[t];
	//if (buffer_to_play != p_Buffer)
	//{
	//	p_Buffer = buffer_to_play;
	//	alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
	//}

	//alSourcePlay(p_Source);
	if (t >= soundBuffers.size() || soundBuffers[t] == 0)
	{
		// Invalid sound index or buffer not loaded
		return;
	}

	// Find a free source from our pool
	ALuint sourceToPlay = GetAvailableSource();

	if (sourceToPlay != 0)
	{
		// Stop whatever the source was doing (if anything)
		alSourceStop(sourceToPlay);
		// Assign the new sound buffer to this source
		alSourcei(sourceToPlay, AL_BUFFER, soundBuffers[t]);
		// Play the sound
		alSourcePlay(sourceToPlay);
	}
}

void SoundEffectsPlayer::Stop()
{
	alSourceStop(p_Source);
}

void SoundEffectsPlayer::StopAll()
{
	for (ALuint source : m_sources)
	{
		alSourceStop(source);
	}
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

void SoundEffectsPlayer::SetLooping(int soundIndex, const bool& loop)
{
	if (!m_sources.empty())
	{
		ALuint source = m_sources[0];
		alSourcei(source, AL_BUFFER, soundBuffers[soundIndex]);
		alSourcei(source, AL_LOOPING, loop);
	}
}

void SoundEffectsPlayer::SetPosition(const float& x, const float& y, const float& z)
{
	for (ALuint source : m_sources)
	{
		alSource3f(source, AL_POSITION, x, y, z);
	}
}


bool SoundEffectsPlayer::isPlaying()
{
	ALint playState;
	alGetSourcei(p_Source, AL_SOURCE_STATE, &playState);
	return (playState == AL_PLAYING);
}

ALuint SoundEffectsPlayer::GetAvailableSource()
{
	for (ALuint source : m_sources)
	{
		ALint playState;
		alGetSourcei(source, AL_SOURCE_STATE, &playState);
		if (playState != AL_PLAYING)
		{
			// This source is not playing, so it's available
			return source;
		}
	}
	// If all sources are busy, just return the first one.
	// This will cut off the oldest sound, which is usually acceptable ("sound stealing").
	return m_sources[0];
}