#pragma once
#include <string>
#include <vector>
#include "PixelMap.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/pixdesc.h>
#include <libswresample/swresample.h> // Audio resampling
}
#include <AL/al.h>      // Core OpenAL API
#include <AL/alc.h>     // Context and device management
#include <AL/alext.h> 
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/avconfig.h>
namespace Q3D {
    namespace Engine {
        namespace Texture {
            class Texture2D;
        }
    }
}

struct YUVFrameTextures {
    Q3D::Engine::Texture::Texture2D* Y = nullptr;
    Q3D::Engine::Texture::Texture2D* U = nullptr;
    Q3D::Engine::Texture::Texture2D* V = nullptr;
};

struct Frame {

    YUVFrameTextures Image;
    double TimeStamp;


};

class GameVideo
{
public:

    GameVideo(std::string path);
    ~GameVideo();
    void Play();
    void Update();
    Q3D::Engine::Texture::Texture2D* GetFrame();
    double GetCurrentFrameTimestamp() const;
    YUVFrameTextures* GetYUVFrame();
    void Pause();
    void Resume();
    double GetCurrentTime() const;
    void Seek(float timeInSeconds);
    Q3D::Engine::Texture::Texture2D* GetSingleFrameTex(float timeInSeconds);
    PixelMap* GetSingleFramePM(float timeInSeconds);
    double GetDuration() const;
    std::vector<PixelMap*> GenerateThumbnails();
    
private:
    AVFrame* decodeFrameAtTime(float timeInSeconds);
    std::string m_Path;
    AVFormatContext* formatCtx = nullptr;
    AVCodecContext* videoCodecCtx = nullptr;
    AVCodecContext* audioCodecCtx = nullptr;
    YUVFrameTextures* m_CurrentFrame = nullptr;
    AVFrame* videoFrame = nullptr;
    AVFrame* audioFrame = nullptr;
    AVPacket packet;
    int videoStreamIndex = -1;
    int audioStreamIndex = -1;
    ALCdevice* device;
    ALCcontext* context;
    ALuint source;

    //YUVFrameTextures* m_CurrentFrame = nullptr;
    std::vector<Frame*> m_Frames;
    double m_CurrentFrameTimestamp;

    bool isPlaying = false;
    double frameDelay = 1.0 / 30.0;  // For 30 FPS video, adjust as needed
    double lastFrameTime = 0.0;

    double StartTime = 0.0f;
    int StartClock = 0;
    bool m_justSought = false;
    double m_playbackStartTime = 0.0;
    SwsContext* swsContext = nullptr;
};