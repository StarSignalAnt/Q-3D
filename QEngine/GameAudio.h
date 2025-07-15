#pragma once
#define WITH_WASAPI
#include "soloud.h"
#include "soloud_wav.h"
#include <string>
#include <vector>
struct GSound {

	SoLoud::Wav* sound;
	float Length() {

		return sound->getLength();

	}
	std::vector<float> waveformData;

};

class GameAudio
{
public:

	GameAudio();

	static GameAudio* m_Instance;
	void PlaySong(std::string path);
	void StopSong();
	GSound* LoadSound(std::string path);
	int PlaySound(GSound* sound);
	void StopSound(int sound);
	void SetPitch(int sound, float pitch);
	void SetVolume(int sound, float volume);
	void SeekSound(int handle, float timeSeconds);
	SoLoud::Soloud gSoloud;
private:


	SoLoud::handle songHandle;

};

