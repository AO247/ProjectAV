#pragma once
#include <AL\alc.h>

#define SD_INIT SoundDevice::Init();
#define LISTENER SoundDevice::Get()

class SoundDevice
{
public:
	static SoundDevice* Get();
	static void Init();

	void GetLocation(float& x, float& y, float& z);
	void GetOrientation(float& ori);
	float GetGain();

	void SetAttenuation(int key);
	void SetLocation(const float& x, const float& y, const float& z);
	void SetOrientation(
		const float& atx, const float& aty, const float& atz,
		const float& upx, const float& upy, const float& upz);
	void SetGain(const float& val);

private:
	SoundDevice();
	~SoundDevice();

	ALCdevice* p_ALCDevice;
	ALCcontext* p_ALCContext;

};

