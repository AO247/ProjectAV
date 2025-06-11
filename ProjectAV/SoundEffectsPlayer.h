#pragma once
#include <AL\al.h>
#include "Node.h"

class SoundEffectsPlayer : public Component
{
public:
	SoundEffectsPlayer(Node* owner, int sourceCount = 8);
	~SoundEffectsPlayer();

	void Play(int t = 0);
	void Stop();
	void StopAll();
;	void Pause();
	void Resume();

	void SetBufferToPlay(const ALuint& buffer_to_play);
	void SetLooping(int soundIndex, const bool& loop);
	void SetPosition(const float& x, const float& y, const float& z);
	virtual void Update(float dt) override;

	bool isPlaying();
	void AddSound(std::string path);
	std::vector<ALuint> soundBuffers;
private:
	ALuint GetAvailableSource();
	std::vector<ALuint> m_sources;
	ALuint p_Source;
	ALuint p_Buffer = 0;
};

