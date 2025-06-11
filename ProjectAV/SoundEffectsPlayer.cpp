#include "SoundEffectsPlayer.h"
#include "SoundEffectsLibrary.h"
#include <iostream>

SoundEffectsPlayer::SoundEffectsPlayer(Node* owner, int sourceCount) : Component(owner)
{
	if (sourceCount <= 0) return;

	m_sources.resize(sourceCount);
	alGenSources(sourceCount, m_sources.data());

	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
	{
		m_sources.clear();
		OutputDebugStringA("OpenAL Error: Failed to generate sources in SoundEffectsPlayer.\n");
		return;
	}

	for (ALuint source : m_sources)
	{
		alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);
		alSourcef(source, AL_REFERENCE_DISTANCE, 15.0f);
		alSourcef(source, AL_MAX_DISTANCE, 600.0f);
		alSourcef(source, AL_GAIN, 1.0f);
	}
}

void SoundEffectsPlayer::Update(float dt)
{
	DirectX::XMFLOAT3 worldPos = pOwner->GetWorldPosition();
	SetPosition(worldPos.x, worldPos.y, worldPos.z);
}

void SoundEffectsPlayer::AddSound(const std::string& path) {
	ALuint bufferID = SoundEffectsLibrary::Get().Load(path);
	if (bufferID != 0)
	{
		m_soundBufferIDs.push_back(bufferID);
	}
	else
	{
		OutputDebugStringA(("Warning: Failed to load or find sound: " + path + "\n").c_str());
	}
}

SoundEffectsPlayer::~SoundEffectsPlayer()
{
	if (!m_sources.empty())
	{
		alDeleteSources(m_sources.size(), m_sources.data());
	}
}

void SoundEffectsPlayer::Play(int index)
{
	if (m_sources.empty() || index >= m_soundBufferIDs.size())
	{
		return;
	}

	ALuint bufferToPlay = m_soundBufferIDs[index];
	if (bufferToPlay == 0) return;

	ALuint sourceToPlay = GetAvailableSource();
	if (sourceToPlay != 0)
	{
		alSourceStop(sourceToPlay);
		alSourcei(sourceToPlay, AL_BUFFER, bufferToPlay);
		alSourcePlay(sourceToPlay);
	}
}

void SoundEffectsPlayer::StopAll()
{
	if (m_sources.empty()) return;
	for (ALuint source : m_sources)
	{
		alSourceStop(source);
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

ALuint SoundEffectsPlayer::GetAvailableSource()
{
	if (m_sources.empty()) return 0;

	for (ALuint source : m_sources)
	{
		ALint playState;
		alGetSourcei(source, AL_SOURCE_STATE, &playState);
		if (playState != AL_PLAYING)
		{
			return source;
		}
	}
	return m_sources[0];
}