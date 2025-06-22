#include "SoundEffectsPlayer.h"
#include "StaticSoundPlayer.h" 

SoundEffectsPlayer::SoundEffectsPlayer(Node* owner) : Component(owner)
{
    
}

void SoundEffectsPlayer::AddSound(const std::string& filename)
{
	m_soundPlaylist.push_back(filename);
}

ALuint SoundEffectsPlayer::Play(int soundIndex, float gain, bool loop)
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

    ALuint sourceID = StaticSoundPlayer::Get().Play(
        filename,
        pOwner->GetWorldPosition(),
        gain,
        loop
    );

    if (sourceID != 0 && loop)
    {
        m_activeSounds[soundIndex] = sourceID;
    }

    return sourceID;
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