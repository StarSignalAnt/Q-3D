#include "GameVideo.h"
#include <iostream>
#include "Texture2D.h"
#include <algorithm> 
GameVideo::GameVideo(std::string path)
{
    // Load video from path
    m_Path = path;
    avformat_network_init();
    m_CurrentFrameTimestamp = -1.0;  // Use -1.0 to indicate "not set yet"
    m_justSought = false;
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
    alGetSourcef(source, AL_SEC_OFFSET, &time);
    ALenum alError = alGetError();
    if (alError != AL_NO_ERROR) {
        std::cerr << "OpenAL error getting source time: " << alError << std::endl;
        return 0.0f; // Return a valid time to prevent issues.
    }
    return time;
}

void GameVideo::Play() {
    // When starting from the beginning, the playback start time is 0.
   // When starting from the beginning, the playback start time is 0.
    m_playbackStartTime = 0.0;
    isPlaying = true;

    // Re-initialize the OpenAL source for a clean start
    if (source != 0) alDeleteSources(1, &source);
    alGenSources(1, &source);
    alSourcePlay(source);
}
void GameVideo::Update() {
    if (!isPlaying) return;
    const size_t MAX_VIDEO_FRAMES_BUFFERED = 60;
  //  if (m_Frames.size() >= MAX_VIDEO_FRAMES_BUFFERED) {
 //       return; // Don't decode more video until some frames are consumed.
   // }

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
    if (!isPlaying && m_CurrentFrame != nullptr) {
        return m_CurrentFrame;
    }

    float audioClockTime = getSourceTime(source);
    double displayTime = m_playbackStartTime + audioClockTime;

    Frame* bestFrame = nullptr;

    // Find the most recent frame that should be on screen.
    for (Frame* frame : m_Frames) {
        if (frame->TimeStamp <= displayTime) {
            bestFrame = frame;
        }
        else {
            break; // Stop as soon as we see a frame from the future.
        }
    }

    if (bestFrame) {
        // Clean up all frames that are now older than the one we're showing.
        while (!m_Frames.empty() && m_Frames.front() != bestFrame) {
            Frame* oldFrame = m_Frames.front();
            delete oldFrame->Image;
            delete oldFrame;
            m_Frames.erase(m_Frames.begin());
        }
        // Cache the correct frame.
        m_CurrentFrame = bestFrame->Image;
    }

    // Return the last known good frame to prevent flickering.
    return m_CurrentFrame;
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

void GameVideo::Seek(float timeInSeconds)
{
    if (!formatCtx || videoStreamIndex < 0) return;

    // Pause playback while we perform the seek operations
    Pause();

    // Clamp the seek time to the video's duration
    double duration = GetDuration();
    timeInSeconds = std::max(0.0f, std::min((float)duration, timeInSeconds));

    // 1. Flush all old data from decoders and our internal buffers
    if (videoCodecCtx) avcodec_flush_buffers(videoCodecCtx);
    if (audioCodecCtx) avcodec_flush_buffers(audioCodecCtx);
    for (Frame* frame : m_Frames) {
        delete frame->Image;
        delete frame;
    }
    m_Frames.clear();
    m_CurrentFrame = nullptr;

    // Clear and restart the OpenAL audio source
    alSourceStop(source);
    ALint buffersProcessed = 0;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffersProcessed);
    if (buffersProcessed > 0) {
        std::vector<ALuint> buffers(buffersProcessed);
        alSourceUnqueueBuffers(source, buffersProcessed, buffers.data());
        alDeleteBuffers(buffersProcessed, buffers.data());
    }

    // 2. Perform a fast seek to the keyframe BEFORE our target time
    AVStream* stream = formatCtx->streams[videoStreamIndex];
    int64_t targetTimestamp = static_cast<int64_t>(timeInSeconds / av_q2d(stream->time_base));
    if (av_seek_frame(formatCtx, videoStreamIndex, targetTimestamp, AVSEEK_FLAG_BACKWARD) < 0) {
        std::cerr << "Error seeking video stream!" << std::endl;
        Resume();
        return;
    }

    // 3. Decode forward from the keyframe until we find the precise frame
    bool frameFound = false;
    while (!frameFound && av_read_frame(formatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(videoCodecCtx, &packet) >= 0) {
                int ret = avcodec_receive_frame(videoCodecCtx, videoFrame);
                if (ret == 0) {
                    double frameTimestamp = videoFrame->pts * av_q2d(stream->time_base);
                    // If this frame is at or after our target, it's the one we want
                    if (frameTimestamp >= timeInSeconds) {
                        // Set the precise start time based on this frame's timestamp
                        m_playbackStartTime = frameTimestamp;
                        m_CurrentFrameTimestamp = frameTimestamp;

                        // Process and buffer this single frame
                        // (This is your video conversion logic from the Update function)
                        if (!swsContext) {
                            swsContext = sws_getContext(videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt, videoCodecCtx->width, videoCodecCtx->height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);
                        }
                        uint8_t* iRgbaBuffer = new uint8_t[videoCodecCtx->width * videoCodecCtx->height * 4];
                        int iStride[1] = { videoCodecCtx->width * 4 };
                        sws_scale(swsContext, videoFrame->data, videoFrame->linesize, 0, videoCodecCtx->height, &iRgbaBuffer, iStride);
                        const int numPixels = videoCodecCtx->width * videoCodecCtx->height;
                        float* fFloatBuffer = new float[numPixels * 4];
                        for (int p = 0; p < numPixels; ++p) {
                            fFloatBuffer[p * 4 + 0] = iRgbaBuffer[p * 4 + 0] / 255.0f;
                            fFloatBuffer[p * 4 + 1] = iRgbaBuffer[p * 4 + 1] / 255.0f;
                            fFloatBuffer[p * 4 + 2] = iRgbaBuffer[p * 4 + 2] / 255.0f;
                            fFloatBuffer[p * 4 + 3] = iRgbaBuffer[p * 4 + 3] / 255.0f;
                        }
                        delete[] iRgbaBuffer;
                        auto img = new Q3D::Engine::Texture::Texture2D(videoCodecCtx->width, videoCodecCtx->height, fFloatBuffer, 4);
                        delete[] fFloatBuffer;

                        Frame* frame = new Frame;
                        frame->Image = img;
                        frame->TimeStamp = m_CurrentFrameTimestamp;
                        m_Frames.push_back(frame);

                        frameFound = true;
                    }
                }
            }
        }
        // We still need to process audio packets found during this search to prime the audio buffer
        else if (packet.stream_index == audioStreamIndex) {
            // (Your existing audio decoding logic from Update goes here)
        }
        av_packet_unref(&packet);
    }

    // 4. Start playback from the new, precisely synced position
    alSourcePlay(source);
    Resume();
}
double GameVideo::GetCurrentTime() const {
    float audioClockTime = getSourceTime(const_cast<GameVideo*>(this)->source);
    if (audioClockTime < 0) audioClockTime = 0;
    return m_playbackStartTime + audioClockTime;
}

AVFrame* GameVideo::decodeFrameAtTime(float timeInSeconds)
{
    if (!formatCtx || videoStreamIndex < 0) return nullptr;

    // 1. Seek to the keyframe at or just before the requested time.
    AVStream* stream = formatCtx->streams[videoStreamIndex];
    int64_t targetTimestamp = static_cast<int64_t>(timeInSeconds / av_q2d(stream->time_base));
    if (av_seek_frame(formatCtx, videoStreamIndex, targetTimestamp, AVSEEK_FLAG_BACKWARD) < 0) {
        return nullptr;
    }

    // 2. Flush the video decoder to clear any old data.
    if (videoCodecCtx) avcodec_flush_buffers(videoCodecCtx);

    // 3. Decode frames in a loop until we find one at or after our target time.
    AVFrame* decodedFrame = av_frame_alloc();
    while (av_read_frame(formatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(videoCodecCtx, &packet) >= 0) {
                int ret = avcodec_receive_frame(videoCodecCtx, decodedFrame);
                if (ret == 0) {
                    // We successfully decoded a frame. Check if its timestamp is correct.
                    double frameTimestamp = decodedFrame->pts * av_q2d(stream->time_base);
                    if (frameTimestamp >= timeInSeconds) {
                        // This is the frame we want. Stop and return it.
                        av_packet_unref(&packet);
                        return decodedFrame;
                    }
                }
            }
        }
        // This packet wasn't the one we wanted, so unref it and continue decoding.
        av_packet_unref(&packet);
    }

    // If we get here, we failed to find a suitable frame.
    av_frame_free(&decodedFrame);
    return nullptr;
}

Q3D::Engine::Texture::Texture2D* GameVideo::GetSingleFrameTex(float timeInSeconds)
{
    AVFrame* frame = decodeFrameAtTime(timeInSeconds);
    if (!frame) return nullptr;

    // This conversion logic is the same as in your Update() function.
    if (!swsContext) {
        swsContext = sws_getContext(videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt,
            videoCodecCtx->width, videoCodecCtx->height, AV_PIX_FMT_RGBA,
            SWS_BILINEAR, nullptr, nullptr, nullptr);
    }
    if (!swsContext) {
        av_frame_free(&frame);
        return nullptr;
    }

    // Perform the two-step conversion from the video's native format to 32-bit float RGBA
    uint8_t* intermediateRgbaBuffer = new uint8_t[videoCodecCtx->width * videoCodecCtx->height * 4];
    int intermediateStride[1] = { videoCodecCtx->width * 4 };
    sws_scale(swsContext, frame->data, frame->linesize, 0, videoCodecCtx->height, &intermediateRgbaBuffer, intermediateStride);

    const int numPixels = videoCodecCtx->width * videoCodecCtx->height;
    float* finalFloatBuffer = new float[numPixels * 4];
    for (int i = 0; i < numPixels; ++i) {
        finalFloatBuffer[i * 4 + 0] = intermediateRgbaBuffer[i * 4 + 0] / 255.0f;
        finalFloatBuffer[i * 4 + 1] = intermediateRgbaBuffer[i * 4 + 1] / 255.0f;
        finalFloatBuffer[i * 4 + 2] = intermediateRgbaBuffer[i * 4 + 2] / 255.0f;
        finalFloatBuffer[i * 4 + 3] = intermediateRgbaBuffer[i * 4 + 3] / 255.0f;
    }

    // Create the texture. The caller is responsible for deleting it.
    auto* tex = new Q3D::Engine::Texture::Texture2D(videoCodecCtx->width, videoCodecCtx->height, finalFloatBuffer, 4);

    // Clean up
    delete[] intermediateRgbaBuffer;
    delete[] finalFloatBuffer;
    av_frame_free(&frame);

    return tex;
}


// --- ADDED --- New public function to get a frame as a PixelMap.
PixelMap* GameVideo::GetSingleFramePM(float timeInSeconds)
{
    AVFrame* frame = decodeFrameAtTime(timeInSeconds);
    if (!frame) return nullptr;

    // This conversion logic is the same as in your Update() function.
    if (!swsContext) {
        swsContext = sws_getContext(videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt,
            videoCodecCtx->width, videoCodecCtx->height, AV_PIX_FMT_RGBA,
            SWS_BILINEAR, nullptr, nullptr, nullptr);
    }
    if (!swsContext) {
        av_frame_free(&frame);
        return nullptr;
    }

    // Here we only need the 8-bit RGBA buffer for the PixelMap
    uint8_t* rgbaBuffer = new uint8_t[videoCodecCtx->width * videoCodecCtx->height * 4];
    int stride[1] = { videoCodecCtx->width * 4 };
    sws_scale(swsContext, frame->data, frame->linesize, 0, videoCodecCtx->height, &rgbaBuffer, stride);

    // Create a blank 8-bit PixelMap
    auto* pm = new PixelMap(videoCodecCtx->width, videoCodecCtx->height, PixelMapDataType::UINT8);
    // Copy the raw pixel data into the PixelMap's buffer
    memcpy(pm->GetData(), rgbaBuffer, videoCodecCtx->width * videoCodecCtx->height * 4);

    // Clean up
    delete[] rgbaBuffer;
    av_frame_free(&frame);

    return pm;
}

double GameVideo::GetDuration() const {
    if (formatCtx && formatCtx->duration != AV_NOPTS_VALUE) {
        // The duration is stored in a microsecond-based time base, convert it to seconds.
        return (double)formatCtx->duration / AV_TIME_BASE;
    }
    return 0.0;
}

std::vector<PixelMap*> GameVideo::GenerateThumbnails()
{
    std::vector<PixelMap*> thumbnails;
    if (m_Path.empty()) return thumbnails;

    // 1. Create a separate, temporary context for thumbnailing.
    AVFormatContext* thumbFormatCtx = nullptr;
    if (avformat_open_input(&thumbFormatCtx, m_Path.c_str(), nullptr, nullptr) != 0) {
        return thumbnails;
    }
    if (avformat_find_stream_info(thumbFormatCtx, nullptr) < 0) {
        avformat_close_input(&thumbFormatCtx);
        return thumbnails;
    }

    const AVCodec* thumbCodec = nullptr;
    int thumbVideoStreamIndex = av_find_best_stream(thumbFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &thumbCodec, 0);
    if (thumbVideoStreamIndex < 0) {
        avformat_close_input(&thumbFormatCtx);
        return thumbnails;
    }

    AVStream* stream = thumbFormatCtx->streams[thumbVideoStreamIndex];
    AVCodecContext* thumbCodecCtx = avcodec_alloc_context3(thumbCodec);
    avcodec_parameters_to_context(thumbCodecCtx, stream->codecpar);
    avcodec_open2(thumbCodecCtx, thumbCodec, nullptr);

    AVFrame* frame = av_frame_alloc();
    AVPacket* packet = av_packet_alloc();
    SwsContext* thumbSwsContext = sws_getContext(thumbCodecCtx->width, thumbCodecCtx->height, thumbCodecCtx->pix_fmt,
        thumbCodecCtx->width, thumbCodecCtx->height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (!thumbSwsContext) {
        av_frame_free(&frame);
        av_packet_free(&packet);
      //  avcodec_close(thumbCodecCtx);
        avformat_close_input(&thumbFormatCtx);
        return thumbnails;
    }

    double nextThumbnailTime = 0.0;
    double duration = GetDuration();

    // 2. Read through the file packet by packet.
    while (av_read_frame(thumbFormatCtx, packet) >= 0) {
        if (packet->stream_index == thumbVideoStreamIndex) {
            if (avcodec_send_packet(thumbCodecCtx, packet) >= 0) {

                // A single packet can produce multiple frames, so we need to loop here.
                int ret = 0;
                while (ret >= 0) {
                    ret = avcodec_receive_frame(thumbCodecCtx, frame);
                    if (ret == 0) {
                        // --- FIXED --- We decode the frame first...
                        // ...and THEN we check its accurate Presentation Time Stamp (PTS).
                        double frameTimestamp = frame->pts * av_q2d(stream->time_base);

                        if (frameTimestamp >= nextThumbnailTime) {
                            // This is a frame we want to keep.
                            uint8_t* rgbaBuffer = new uint8_t[thumbCodecCtx->width * thumbCodecCtx->height * 4];
                            int stride[1] = { thumbCodecCtx->width * 4 };
                            sws_scale(thumbSwsContext, frame->data, frame->linesize, 0, thumbCodecCtx->height, &rgbaBuffer, stride);

                            auto* pm = new PixelMap(thumbCodecCtx->width, thumbCodecCtx->height, PixelMapDataType::UINT8);
                            memcpy(pm->GetData(), rgbaBuffer, thumbCodecCtx->width * thumbCodecCtx->height * 4);
                            thumbnails.push_back(pm);
                            delete[] rgbaBuffer;

                            // Set our target to the next full second.
                            nextThumbnailTime += 1.0;
                            if (nextThumbnailTime > duration) {
                                // Use a goto to break out of the nested loops cleanly.
                                goto cleanup;
                            }
                        }
                    }
                }
            }
        }
        av_packet_unref(packet);
    }

cleanup:
    // 3. Clean up all temporary resources.
    av_packet_unref(packet);
    sws_freeContext(thumbSwsContext);
    av_frame_free(&frame);
  
    av_packet_free(&packet);
    //avcodec_close(thumbCodecCtx);
    avformat_close_input(&thumbFormatCtx);

    return thumbnails;
}