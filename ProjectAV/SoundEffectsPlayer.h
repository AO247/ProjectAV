#pragma once
#include <AL\al.h>
#include "Node.h"

class SoundEffectsPlayer : public Component
{
public:
	SoundEffectsPlayer(Node* owner, int sourceCount = 4);
	~SoundEffectsPlayer();

	void Play(int index);
	void Stop(int index);
	void StopAll();
;	void Pause(int index);
	void Resume(int index);

	void SetBufferToPlay(const ALuint& buffer_to_play);
	void SetLooping(int soundIndex, const bool& loop);
	void SetPosition(const float& x, const float& y, const float& z);

	virtual void Update(float dt) override;
	void AddSound(const std::string& path);
	std::vector<ALuint> soundBuffers;
	Node* player;
private:
	float m_maxAudibleDistance = 300.0f;
	ALuint GetAvailableSource();
	std::vector<ALuint> m_sources;
	std::vector<ALuint> m_soundBufferIDs;
};

