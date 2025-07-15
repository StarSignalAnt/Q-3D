#include "GameAudio.h"


GameAudio* GameAudio::m_Instance = nullptr;
void generateWaveformData(GSound* gs) {
	if (!gs || !gs->sound || gs->sound->mSampleCount == 0 || gs->sound->getLength() <= 0) {
		gs->waveformData.clear();
		return;
	}

	SoLoud::Wav* wav = gs->sound;
	gs->waveformData.clear();

	// Calculate the total pixel width the sound will occupy on the timeline.
	float totalPixelWidth = wav->getLength() * 100.0f;

	// We want one data point (one vertical line) for every 2 horizontal pixels.
	// This creates a dense waveform without being excessively detailed.
	unsigned int totalPoints = static_cast<unsigned int>(totalPixelWidth / 2.0f);

	// Ensure we have at least a few points to draw for very short sounds.
	if (totalPoints < 20) totalPoints = 20;

	// Calculate how many audio samples fall into each "bucket" or data point.
	unsigned int samplesPerPoint = wav->mSampleCount / totalPoints;
	if (samplesPerPoint == 0) samplesPerPoint = 1;

	gs->waveformData.reserve(totalPoints);

	for (unsigned int i = 0; i < totalPoints; ++i) {
		float maxSample = 0.0f;

		// Find the peak amplitude within this bucket of samples.
		unsigned int startSample = i * samplesPerPoint;
		unsigned int endSample = startSample + samplesPerPoint;

		for (unsigned int j = startSample; j < endSample; j += wav->mChannels) {
			if (j < wav->mSampleCount) {
				// We only need to check one channel for the amplitude.
				float currentSample = std::abs(wav->mData[j]);
				if (currentSample > maxSample) {
					maxSample = currentSample;
				}
			}
		}
		// Add the normalized peak value to our waveform data.
		gs->waveformData.push_back(maxSample);
	}
}

GameAudio::GameAudio() {

	m_Instance = this;
	gSoloud.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::WASAPI);

   

  

}

void GameAudio::PlaySong(std::string path) {

    SoLoud::Wav* sound = new SoLoud::Wav;
    sound->load(path.c_str());
    songHandle = gSoloud.play(*sound);


}

GSound* GameAudio::LoadSound(std::string path) {

	SoLoud::Wav* sound = new SoLoud::Wav;
	if (sound->load(path.c_str()) != SoLoud::SO_NO_ERROR) {
		delete sound;
		return nullptr;
	}

	GSound* gs = new GSound;
	gs->sound = sound;

	// Process the raw audio data into a drawable waveform.
	generateWaveformData(gs);

	return gs;

}

void GameAudio::StopSong() {

    gSoloud.stop(songHandle);


}

int GameAudio::PlaySound(GSound* s) {

    auto r = gSoloud.play(*s->sound);
    return (int)r;
}

void GameAudio::StopSound(int sound) {

    auto handle = (SoLoud::handle*) sound;


    gSoloud.stop(sound);

}

void GameAudio::SetPitch(int id, float pitch) {

    gSoloud.setRelativePlaySpeed(id, pitch);

}

void GameAudio::SetVolume(int id, float volume) {

    gSoloud.setVolume(id, volume);

}

void GameAudio::SeekSound(int handle, float timeSeconds)
{
	gSoloud.seek(handle, timeSeconds);
}