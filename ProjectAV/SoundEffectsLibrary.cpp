#include "SoundEffectsLibrary.h"
#include <sndfile.h>
#include <inttypes.h>
#include <AL/alext.h>
#include <cstdio> // dla fprintf, stderr
#include <cstdlib> // dla malloc

SoundEffectsLibrary& SoundEffectsLibrary::Get()
{
	static SoundEffectsLibrary instance;
	return instance;
}

ALuint SoundEffectsLibrary::Load(const std::string& filename)
{
	// Jeœli dŸwiêk jest ju¿ wczytany, zwróæ jego ID
	if (m_buffers.count(filename))
	{
		return m_buffers[filename];
	}

	// W przeciwnym razie wczytaj z pliku
	ALuint bufferID = LoadFromFile(filename.c_str());
	if (bufferID != 0)
	{
		// Zapisz w cache i zwróæ
		m_buffers[filename] = bufferID;
	}
	return bufferID;
}

SoundEffectsLibrary::~SoundEffectsLibrary()
{
	UnloadAll();
}

void SoundEffectsLibrary::UnloadAll()
{
	// Stwórz wektor z samymi ID do usuniêcia
	std::vector<ALuint> buffer_ids;
	for (auto const& [key, val] : m_buffers)
	{
		buffer_ids.push_back(val);
	}

	if (!buffer_ids.empty())
	{
		alDeleteBuffers(buffer_ids.size(), buffer_ids.data());
	}

	m_buffers.clear();
}


// Prywatna metoda wczytuj¹ca, kod z poprzednich implementacji
ALuint SoundEffectsLibrary::LoadFromFile(const char* filename)
{
	ALenum err, format;
	ALuint buffer;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	sndfile = sf_open(filename, SFM_READ, &sfinfo);
	if (!sndfile)
	{
		fprintf(stderr, "Could not open audio in %s: %s\n", filename, sf_strerror(sndfile));
		return 0;
	}
	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
	{
		fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", filename, sfinfo.frames);
		sf_close(sndfile);
		return 0;
	}

	format = AL_NONE;
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3)
	{
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT2D_16;
	}
	else if (sfinfo.channels == 4)
	{
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!format)
	{
		fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
		sf_close(sndfile);
		return 0;
	}

	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1)
	{
		free(membuf);
		sf_close(sndfile);
		fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", filename, num_frames);
		return 0;
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
		fprintf(stderr, "OpenAL Error during buffer data: %s\n", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}

	return buffer;
}