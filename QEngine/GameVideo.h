#pragma once
#include <string>
#include <vector>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h> // Audio resampling
}
#include <AL/al.h>      // Core OpenAL API
#include <AL/alc.h>     // Context and device management
#include <AL/alext.h> 
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/avconfig.h>
class Texture2D;

struct Frame {

    Texture2D* Image;
    double TimeStamp;


};

class GameVideo
{
public:

    GameVideo(std::string path);
    ~GameVideo();
    void Play();
    void Update();
    Texture2D* GetFrame();
    double GetCurrentFrameTimestamp() const;
    void Pause();
    void Resume();

private:

    std::string m_Path;
    AVFormatContext* formatCtx = nullptr;
    AVCodecContext* videoCodecCtx = nullptr;
    AVCodecContext* audioCodecCtx = nullptr;
    AVFrame* videoFrame = nullptr;
    AVFrame* audioFrame = nullptr;
    AVPacket packet;
    int videoStreamIndex = -1;
    int audioStreamIndex = -1;
    ALCdevice* device;
    ALCcontext* context;
    ALuint source;
    Texture2D* m_CurrentFrame = nullptr;

    std::vector<Frame*> m_Frames;
    double m_CurrentFrameTimestamp;

    bool isPlaying = false;
    double frameDelay = 1.0 / 30.0;  // For 30 FPS video, adjust as needed
    double lastFrameTime = 0.0;

    double StartTime = 0.0f;
    int StartClock = 0;

    SwsContext* swsContext = nullptr;
};