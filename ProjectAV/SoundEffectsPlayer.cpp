#include "SoundEffectsPlayer.h"
#include "StaticSoundPlayer.h" 

SoundEffectsPlayer::SoundEffectsPlayer(Node* owner) : Component(owner)
{
    
}

SoundEffectsPlayer::~SoundEffectsPlayer()
{
	StopAll();
	m_soundPlaylist.clear();
	m_activeSounds.clear();
}

void SoundEffectsPlayer::AddSound(const std::string& filename)
{
	m_soundPlaylist.push_back(filename);
}

ALuint SoundEffectsPlayer::Play(int soundIndex, float gain, bool isPositional, bool loop)
{
    if (soundIndex >= m_soundPlaylist.size())
    {
        return 0;
    }

    if (loop && m_activeSounds.count(soundIndex))
    {
        return m_activeSounds[soundIndex];
    }

    const std::string& filename = m_soundPlaylist[soundIndex];

    float volume = volumeGlobal;
    if (!isPositional) {
        volume *= volumePlayer;
    }
    else {
        volume *= volumePos;
    }

    ALuint sourceID = StaticSoundPlayer::Get().Play(
        filename,
        pOwner->GetWorldPosition(),
        gain * volume,
        loop,
		StaticSoundPlayer::Get().rolloff, // rolloff
		StaticSoundPlayer::Get().refDistance, // refDistance
		StaticSoundPlayer::Get().m_maxAudibleDistance, // maxDistance
		StaticSoundPlayer::Get().cullingMode // cullingMode
    );

    if (sourceID != 0)
    {
        m_activeSounds[soundIndex] = sourceID;
    }

    return sourceID;
}

ALuint SoundEffectsPlayer::PlayAdvanced(int soundIndex, float gain, bool loop,
    float rolloff, float refDistance, float maxDistance, bool cullingMode)
{
    if (soundIndex < 0 || soundIndex >= m_soundPlaylist.size()) return 0;
    const std::string& filename = m_soundPlaylist[soundIndex];
    StaticSoundPlayer::Get().Play(filename, pOwner->GetWorldPosition(), gain, loop,
        rolloff, refDistance, maxDistance, cullingMode);
}

void SoundEffectsPlayer::Stop(int soundIndex)
{
    if (m_activeSounds.count(soundIndex))
    {
        ALuint sourceID = m_activeSounds[soundIndex];

        StaticSoundPlayer::Get().Stop(sourceID);

        m_activeSounds.erase(soundIndex);
    }
}

void SoundEffectsPlayer::StopAll()
{
    for (auto const& [soundIndex, sourceID] : m_activeSounds)
    {
        StaticSoundPlayer::Get().Stop(sourceID);
    }
    m_activeSounds.clear();
}