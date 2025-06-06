#pragma once
#define WITH_WASAPI
#include "soloud.h"
#include "soloud_wav.h"
#include <string>

class GameAudio
{
public:

	GameAudio();

	static GameAudio* m_Instance;
	void PlaySong(std::string path);
	void StopSong();
	void* LoadSound(std::string path);
	int PlaySound(SoLoud::Wav* sound);
	void StopSound(int sound);

private:

	SoLoud::Soloud gSoloud;
	SoLoud::handle songHandle;

};

