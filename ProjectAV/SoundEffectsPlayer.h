#pragma once
#include "Component.h"
#include <vector>
#include <string>
#include <AL/al.h>

class SoundEffectsPlayer : public Component
{
public:
	SoundEffectsPlayer(Node* owner);
	~SoundEffectsPlayer() = default;

	void AddSound(const std::string& filename);

	ALuint Play(int soundIndex, float gain = 1.0f, bool isPositional = true, bool loop = false);

	void Stop(int soundIndex);

	void StopAll();

	float volumeGlobal = 0.7f;
	float volumePos = 0.7f;
	float volumePlayer = 0.4f;

private:
	std::vector<std::string> m_soundPlaylist;

	std::unordered_map<int, ALuint> m_activeSounds;
};