#include "GameVideo.h"
#include <iostream>
#include "Texture2D.h"

GameVideo::GameVideo(std::string path)
{
    // Load video from path
    m_Path = path;
    avformat_network_init();
    m_CurrentFrameTimestamp = -1.0;  // Use -1.0 to indicate "not set yet"

    // Allocate format context
    formatCtx = avformat_alloc_context();
    if (!formatCtx) {
        std::cerr << "Failed to allocate AVFormatContext!" << std::endl;
        return;
    }

    // Open the video file
    if (avformat_open_input(&formatCtx, path.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Failed to open video file: " << path << std::endl;
        avformat_free_context(formatCtx);
        formatCtx = nullptr;
        return;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
        std::cerr << "Failed to retrieve stream info!" << std::endl;
        avformat_close_input(&formatCtx);
        return;
    }

    // Print video file info
    std::cout << "Opened video: " << path << std::endl;
    av_dump_format(formatCtx, 0, path.c_str(), 0);
    videoStreamIndex = -1;
    audioStreamIndex = -1;

    for (unsigned int i = 0; i < formatCtx->nb_streams; ++i) {
        AVCodecParameters* codecParams = formatCtx->streams[i]->codecpar;
        if (codecParams->codec_type == AVMEDIA_TYPE_VIDEO && videoStreamIndex == -1) {
            videoStreamIndex = i;
            const AVCodec* videoCodec = avcodec_find_decoder(codecParams->codec_id);
            videoCodecCtx = avcodec_alloc_context3(videoCodec);
            if (avcodec_parameters_to_context(videoCodecCtx, codecParams) < 0) {
                std::cerr << "Failed to copy codec parameters for video!" << std::endl;
            }
            avcodec_open2(videoCodecCtx, videoCodec, nullptr);
        }
        else if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO && audioStreamIndex == -1) {
            audioStreamIndex = i;
            const AVCodec* audioCodec = avcodec_find_decoder(codecParams->codec_id);
            audioCodecCtx = avcodec_alloc_context3(audioCodec);
            if (avcodec_parameters_to_context(audioCodecCtx, codecParams) < 0) {
                std::cerr << "Failed to copy codec parameters for audio!" << std::endl;
            }
            avcodec_open2(audioCodecCtx, audioCodec, nullptr);
        }
    }

    // Allocate frames
    videoFrame = av_frame_alloc();
    audioFrame = av_frame_alloc();

    device = alcOpenDevice(nullptr);  // Use default device
    if (!device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return;
    }

    context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

}

float getSourceTime(ALuint source) {
    float time = 0.0f;

    // Get the current playback time in seconds
    alGetSourcef(source, AL_SEC_OFFSET, &time);

    // Check for OpenAL errors
    ALenum alError = alGetError();
    if (alError != AL_NO_ERROR) {
        std::cerr << "OpenAL error getting source time: " << alError << std::endl;
        return -1.0f; // Return an invalid time if there's an error
    }

    return time;
}

void GameVideo::Play() {

    isPlaying = true;  // Start playback
    StartClock = clock();
    alGenSources(1, &source);
    alSourcePlay(source);
}
void GameVideo::Update() {
    if (!isPlaying) return;

    if (av_read_frame(formatCtx, &packet) < 0) {
        return; // End of file or error
    }

    if (packet.stream_index == videoStreamIndex) {
        int ret = avcodec_send_packet(videoCodecCtx, &packet);
        if (ret >= 0) {
            ret = avcodec_receive_frame(videoCodecCtx, videoFrame);
            if (ret >= 0) {

                // CORRECTION: Lazily initialize the swsContext for the intermediate conversion.
                if (!swsContext) {
                    // STEP 1 SETUP: Convert to a standard 8-bit RGBA format first.
                    swsContext = sws_getContext(
                        videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt, // Source
                        videoCodecCtx->width, videoCodecCtx->height, AV_PIX_FMT_RGBA,        // Destination
                        SWS_BILINEAR, nullptr, nullptr, nullptr);
                }

                if (!swsContext) {
                    std::cerr << "Could not initialize the conversion context!" << std::endl;
                    av_packet_unref(&packet);
                    return;
                }

                AVStream* videoStream = formatCtx->streams[videoStreamIndex];
                int64_t timestamp = videoFrame->pts != AV_NOPTS_VALUE ? videoFrame->pts : videoFrame->pkt_dts;
                if (timestamp != AV_NOPTS_VALUE) {
                    m_CurrentFrameTimestamp = timestamp * av_q2d(videoStream->time_base);
                }
                else {
                    double frameRate = av_q2d(videoStream->r_frame_rate);
                    double frameDuration = frameRate > 0 ? 1.0 / frameRate : 0.04;
                    if (m_CurrentFrameTimestamp >= 0) {
                        m_CurrentFrameTimestamp += frameDuration;
                    }
                    else {
                        m_CurrentFrameTimestamp = 0.0;
                    }
                }

                // --- START: TWO-STEP CONVERSION ---

                // STEP 1: Convert from YUV to 8-bit RGBA
                uint8_t* intermediateRgbaBuffer = new uint8_t[videoCodecCtx->width * videoCodecCtx->height * 4];
                int intermediateStride[1] = { videoCodecCtx->width * 4 };
                uint8_t* intermediateData[1] = { intermediateRgbaBuffer };

                sws_scale(swsContext, videoFrame->data, videoFrame->linesize,
                    0, videoCodecCtx->height,
                    intermediateData, intermediateStride);

                // STEP 2: Manually convert from 8-bit RGBA to 32-bit float RGBA
                const int numPixels = videoCodecCtx->width * videoCodecCtx->height;
                float* finalFloatBuffer = new float[numPixels * 4];

                for (int i = 0; i < numPixels; ++i) {
                    // Read 4 uint8_t values (0-255) and convert them to float (0.0-1.0)
                    finalFloatBuffer[i * 4 + 0] = intermediateRgbaBuffer[i * 4 + 0] / 255.0f; // R
                    finalFloatBuffer[i * 4 + 1] = intermediateRgbaBuffer[i * 4 + 1] / 255.0f; // G
                    finalFloatBuffer[i * 4 + 2] = intermediateRgbaBuffer[i * 4 + 2] / 255.0f; // B
                    finalFloatBuffer[i * 4 + 3] = intermediateRgbaBuffer[i * 4 + 3] / 255.0f; // A
                }

                // --- END: TWO-STEP CONVERSION ---

                // Now, create the texture using the final float buffer
                auto img = new Q3D::Engine::Texture::Texture2D(videoCodecCtx->width, videoCodecCtx->height, finalFloatBuffer, 4);

                // IMPORTANT: Clean up both buffers
                delete[] intermediateRgbaBuffer;
                delete[] finalFloatBuffer;

                Frame* frame = new Frame;
                frame->Image = img;
                frame->TimeStamp = GetCurrentFrameTimestamp();

                m_Frames.push_back(frame);
            }
        }
    }
    else if (packet.stream_index == audioStreamIndex) {
        // ... (audio processing code remains the same)
        int ret = avcodec_send_packet(audioCodecCtx, &packet);
        if (ret >= 0) {
            ret = avcodec_receive_frame(audioCodecCtx, audioFrame);
            if (ret >= 0) {
                ALenum format;
                int channels = audioFrame->ch_layout.nb_channels;
                if (channels == 2) {
                    format = AL_FORMAT_STEREO_FLOAT32;
                    int numSamples = audioFrame->nb_samples;
                    float* interleavedData = new float[numSamples * channels];
                    float* leftChannel = (float*)audioFrame->data[0];
                    float* rightChannel = (float*)audioFrame->data[1];
                    for (int i = 0; i < numSamples; ++i) {
                        interleavedData[2 * i] = leftChannel[i];
                        interleavedData[2 * i + 1] = rightChannel[i];
                    }

                    int dataSize = numSamples * channels * sizeof(float);
                    ALuint buffer;
                    alGenBuffers(1, &buffer);
                    alBufferData(buffer, format, interleavedData, dataSize, audioFrame->sample_rate);

                    if (alGetError() != AL_NO_ERROR) {
                        std::cerr << "OpenAL error loading buffer data" << std::endl;
                        delete[] interleavedData;
                        return;
                    }

                    alSourceQueueBuffers(source, 1, &buffer);

                    ALint sourceState;
                    alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
                    if (sourceState != AL_PLAYING) {
                        alSourcePlay(source);
                    }
                    delete[] interleavedData;
                }
                else {
                    std::cerr << "Unsupported channel count: " << channels << std::endl;
                    return;
                }
            }
        }
    }

    av_packet_unref(&packet);
}

Q3D::Engine::Texture::Texture2D* GameVideo::GetFrame() {
    float ftime = getSourceTime(source);

    if (m_Frames.size() > 0) {
        for (auto it = m_Frames.begin(); it != m_Frames.end(); ) {
            if ((*it)->TimeStamp > ftime) {
                return (*it)->Image;
            }
            else {
                delete (*it)->Image;
                delete* it;
                it = m_Frames.erase(it);
            }
        }
    }
    return nullptr;
}

double GameVideo::GetCurrentFrameTimestamp() const {
    return m_CurrentFrameTimestamp;
}


GameVideo::~GameVideo()
{
    if (swsContext) {
        sws_freeContext(swsContext);
    }
    if (formatCtx) {
        avformat_close_input(&formatCtx);
    }
    avformat_network_deinit();
}

void GameVideo::Pause() {
    isPlaying = false;
    alSourcePause(source);
}

void GameVideo::Resume() {
    isPlaying = true;
    alSourcePlay(source);
}