#pragma once
#include <AL\al.h>
#include "Node.h"

class SoundEffectsPlayer : public Component
{
public:
	SoundEffectsPlayer(Node* owner);
	~SoundEffectsPlayer();

	void Play(int t = 0);
	void Stop();
	void Pause();
	void Resume();

	void SetBufferToPlay(const ALuint& buffer_to_play);
	void SetLooping(const bool& loop);

	bool isPlaying();
	void AddSound(std::string path);
	std::vector<uint32_t> sound;
private:
	ALuint p_Source;
	ALuint p_Buffer = 0;
};

