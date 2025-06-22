#pragma once
#include "Component.h"
#include <vector>
#include <string>

class SoundEffectsPlayer : public Component
{
public:
	SoundEffectsPlayer(Node* owner);
	~SoundEffectsPlayer() = default;

	void AddSound(const std::string& filename);

	void Play(int soundIndex, float gain = 1.0f);

	void Stop(int soundIndex);

	void StopAll();

private:
	std::vector<std::string> m_soundPlaylist;
};