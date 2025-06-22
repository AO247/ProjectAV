#include "SoundEffectsPlayer.h"
#include "StaticSoundPlayer.h" 

SoundEffectsPlayer::SoundEffectsPlayer(Node* owner) : Component(owner)
{
    
}

void SoundEffectsPlayer::AddSound(const std::string& filename)
{
	m_soundPlaylist.push_back(filename);
}

void SoundEffectsPlayer::Play(int soundIndex, float gain)
{
    if (soundIndex < m_soundPlaylist.size())
    {
        const std::string& filename = m_soundPlaylist[soundIndex];
        StaticSoundPlayer::Get().Play(filename, pOwner->GetWorldPosition(), gain);
    }
}