#include "GameAudio.h"

GameAudio* GameAudio::m_Instance = nullptr;

GameAudio::GameAudio() {

	m_Instance = this;
	gSoloud.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::WASAPI);

   

  

}

void GameAudio::PlaySong(std::string path) {

    SoLoud::Wav* sound = new SoLoud::Wav;
    sound->load(path.c_str());
    songHandle = gSoloud.play(*sound);


}

void* GameAudio::LoadSound(std::string path) {

    SoLoud::Wav* sound = new SoLoud::Wav;
    sound->load(path.c_str());
    return (void*)sound;

}

void GameAudio::StopSong() {

    gSoloud.stop(songHandle);


}

int GameAudio::PlaySound(SoLoud::Wav* sound) {

    auto r = gSoloud.play(*sound);
    return (int)r;
}

void GameAudio::StopSound(int sound) {

    auto handle = (SoLoud::handle*) sound;


    gSoloud.stop(sound);

}