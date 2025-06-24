#include "StaticSoundPlayer.h"
#include <sndfile.h>      
#include <inttypes.h>     
#include <AL/alext.h>     
#include <Windows.h>      
#include <cstdio>
#include <cstdlib>

StaticSoundPlayer& StaticSoundPlayer::Get()
{
    static StaticSoundPlayer instance;
    return instance;
}

void StaticSoundPlayer::Init(int sourceCount)
{
    if (m_initialized) return;
    if (sourceCount <= 0) return;

    m_sources.resize(sourceCount);
    alGenSources(sourceCount, m_sources.data());

    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        m_sources.clear();
        OutputDebugStringA("FATAL OpenAL Error: Failed to generate sources for StaticSoundPlayer.\n");
        return;
    }
    m_initialized = true;
}

StaticSoundPlayer::~StaticSoundPlayer()
{
    Shutdown();
}

void StaticSoundPlayer::Shutdown()
{
    if (!m_initialized) return;

    if (!m_sources.empty())
    {
        for (ALuint source : m_sources)
        {
            alSourceStop(source);
        }
        alDeleteSources(m_sources.size(), m_sources.data());
        m_sources.clear();
    }

    std::vector<ALuint> buffer_ids;
    for (auto const& [key, val] : m_soundCache)
    {
        buffer_ids.push_back(val);
    }
    if (!buffer_ids.empty())
    {
        alDeleteBuffers(buffer_ids.size(), buffer_ids.data());
    }
    m_soundCache.clear();

    m_initialized = false;
}

void StaticSoundPlayer::SetPlayerNode(Node* player)
{
    m_playerNode = player;
}

ALuint StaticSoundPlayer::GetAvailableSource()
{
    if (!m_initialized) return 0;

    for (ALuint source : m_sources)
    {
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING)
        {
            return source;
        }
    }

    ALuint stolenSource = m_sources[m_nextSourceIndex];
    m_nextSourceIndex = (m_nextSourceIndex + 1) % m_sources.size();
    return stolenSource;
}

void StaticSoundPlayer::Play(const std::string& filename, DirectX::XMFLOAT3 position, float gain, bool loop)
{
    Play(filename, position, gain, loop, rolloff, refDistance, m_maxAudibleDistance, cullingDistanceModifier, cullingMode);
    return;
}


ALuint StaticSoundPlayer::Play(const std::string& filename, DirectX::XMFLOAT3 position, float gain, bool loop,
    float rolloff, float refDistance, float maxDistance, float cullingDistanceModifier, bool cullingMode)
{
    if (!m_initialized) return 0;

    // Culling na podstawie odleg³oœci
    if (m_playerNode && cullingMode)
    {
        DirectX::SimpleMath::Vector3 playerPos(m_playerNode->GetWorldPosition());
        DirectX::SimpleMath::Vector3 sourcePos(position);
        float distanceSq = DirectX::SimpleMath::Vector3::DistanceSquared(playerPos, sourcePos);
        if (distanceSq > (maxDistance * maxDistance * cullingDistanceModifier))
        {
            return 0;
        }
    }

    ALuint bufferID = LoadAndCacheSound(filename);
    if (bufferID == 0) return 0;

    ALuint source = GetAvailableSource();
    if (source == 0) return 0;

    alSourceStop(source);
    alSourcei(source, AL_BUFFER, bufferID);
    alSourcef(source, AL_GAIN, gain);
    alSource3f(source, AL_POSITION, position.x, position.y, position.z);
    alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
    alSourcef(source, AL_ROLLOFF_FACTOR, rolloff);
    alSourcef(source, AL_REFERENCE_DISTANCE, refDistance);
    alSourcef(source, AL_MAX_DISTANCE, maxDistance);
    alSourcePlay(source);

    return source;
}

ALuint StaticSoundPlayer::LoadAndCacheSound(const std::string& filename)
{
    if (m_soundCache.count(filename))
    {
        return m_soundCache[filename];
    }

    ALenum err, format;
    ALuint buffer;
    SNDFILE* sndfile;
    SF_INFO sfinfo;
    short* membuf;
    sf_count_t num_frames;
    ALsizei num_bytes;

    sndfile = sf_open(filename.c_str(), SFM_READ, &sfinfo);
    if (!sndfile)
    {
        fprintf(stderr, "Could not open audio in %s: %s\n", filename.c_str(), sf_strerror(sndfile));
        return 0;
    }
    if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
    {
        sf_close(sndfile); return 0;
    }

    format = AL_NONE;
    if (sfinfo.channels == 1) format = AL_FORMAT_MONO16;
    else if (sfinfo.channels == 2) format = AL_FORMAT_STEREO16;
    if (!format)
    {
        sf_close(sndfile); return 0;
    }

    membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));
    num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
    if (num_frames < 1)
    {
        free(membuf); sf_close(sndfile); return 0;
    }
    num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

    buffer = 0;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);
    free(membuf);
    sf_close(sndfile);

    err = alGetError();
    if (err != AL_NO_ERROR)
    {
        if (buffer && alIsBuffer(buffer)) alDeleteBuffers(1, &buffer);
        return 0;
    }

    m_soundCache[filename] = buffer;
    return buffer;
}

void StaticSoundPlayer::Stop(ALuint sourceID)
{
    if (!m_initialized || sourceID == 0) return;

    bool isValidSource = false;
    for (ALuint s : m_sources)
    {
        if (s == sourceID)
        {
            isValidSource = true;
            break;
        }
    }

    if (isValidSource)
    {
        alSourceStop(sourceID);
        alSourcei(sourceID, AL_BUFFER, 0);
    }
}