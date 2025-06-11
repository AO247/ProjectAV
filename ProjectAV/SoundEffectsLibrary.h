#pragma once
#include <AL/al.h>
#include <vector>
#include <string>
#include <unordered_map>

class SoundEffectsLibrary
{
public:
	// Pobranie instancji singletona
	static SoundEffectsLibrary& Get();

	// Wczytuje d�wi�k, je�li nie ma go w cache, i zwraca jego ID.
	ALuint Load(const std::string& filename);

	// Zwalnia wszystkie zasoby
	void UnloadAll();

private:
	SoundEffectsLibrary() = default;
	~SoundEffectsLibrary();
	SoundEffectsLibrary(const SoundEffectsLibrary&) = delete;
	SoundEffectsLibrary& operator=(const SoundEffectsLibrary&) = delete;

	ALuint LoadFromFile(const char* filename);

	std::unordered_map<std::string, ALuint> m_buffers;
};
