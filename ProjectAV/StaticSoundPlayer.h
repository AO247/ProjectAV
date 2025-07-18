#pragma once
#include <AL/al.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <unordered_map>
#include "Node.h"

class StaticSoundPlayer
{
public:
    static StaticSoundPlayer& Get();

    void Init(int sourceCount = 64);
    void SetPlayerNode(Node* player);
    void Shutdown();

    void Play(const std::string& filename, DirectX::XMFLOAT3 position, float gain = 1.0f, bool loop = false);
    ALuint Play(const std::string& filename, DirectX::XMFLOAT3 position, float gain, bool loop,
        float rolloff, float refDistance, float maxDistance, float cullingDistanceModifier, bool cullingMode);
    void Stop(ALuint sourceID);

    float m_maxAudibleDistance = 400.0f;
    float rolloff = 1.0f;
    float refDistance = 15.0f;
	float cullingDistanceModifier = 1.0f;
    bool cullingMode = true;

private:
    StaticSoundPlayer() = default;
    ~StaticSoundPlayer();
    StaticSoundPlayer(const StaticSoundPlayer&) = delete;
    StaticSoundPlayer& operator=(const StaticSoundPlayer&) = delete;

    ALuint LoadAndCacheSound(const std::string& filename);
    ALuint GetAvailableSource();

    bool m_initialized = false;
    std::vector<ALuint> m_sources;
    Node* m_playerNode = nullptr;
    
    int m_nextSourceIndex = 0;

    std::unordered_map<std::string, ALuint> m_soundCache;
};