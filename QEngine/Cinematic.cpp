#include "Cinematic.h"
#include "GraphNode.h"
#include "GameAudio.h" // Required for the audio singleton
#include "SceneGraph.h"
// ===================================================================================
// TrackTransform Implementation
// ===================================================================================
enum class TrackTypeID : uint8_t {
    Transform = 0,
    Audio = 1,
    Float = 2
};
TrackTransform::TrackTransform(GraphNode* target) : m_targetNode(target) {
    if (!m_targetNode) {
        throw std::invalid_argument("TrackTransform must have a valid target GraphNode.");
    }
    m_name = m_targetNode->GetName();
}
void TrackTransform::Update(float time, bool isScrubbing) {
    if (!m_targetNode) return;
    TransformState state = GetValueAtTime(time);
    m_targetNode->SetPosition(state.position);
    m_targetNode->SetRotation(state.rotation);
    m_targetNode->SetScale(state.scale);
}
void TrackTransform::RecordKeyframe(float time, InterpolationType interpType) {
    if (!m_targetNode) return;

    constexpr float threshold = 0.1f;

    // Check for a nearby keyframe to update
    for (size_t i = 0; i < m_keyframes.size(); ++i) {
        if (std::abs(m_keyframes[i].GetTime() - time) <= threshold) {
            // Found one, so update its value and interpolation type.
            TransformState currentState;
            currentState.position = m_targetNode->GetPosition();
            currentState.rotation = glm::quat_cast(m_targetNode->GetRotation());
            currentState.scale = m_targetNode->GetScale();
            m_keyframes[i].SetValue(currentState);
            m_keyframes[i].setInterpolation(interpType);
            return; // Exit the function after updating.
        }
    }

    // If the loop completes, no nearby keyframe was found, so create a new one.
    TransformState currentState;
    currentState.position = m_targetNode->GetPosition();
    currentState.rotation = glm::quat_cast(m_targetNode->GetRotation());
    currentState.scale = m_targetNode->GetScale();
    AddKeyframe({ time, currentState, interpType });
}

TransformState TrackTransform::Interpolate(const TransformState& a, const TransformState& b, float t) const {
    TransformState result;
    result.position = glm::lerp(a.position, b.position, t);
    result.rotation = glm::slerp(a.rotation, b.rotation, t);
    result.scale = glm::lerp(a.scale, b.scale, t);
    return result;
}

float max4(float a, float b) {
    return (a > b) ? a : b;
}

float min4(float a, float b) {
    return (a < b) ? a : b;
}
// ===================================================================================
// TrackAudio Implementation
// ===================================================================================
void TrackAudio::Update(float time, bool isScrubbing) {
    if (!GameAudio::m_Instance) return;

    // --- SCRUBBING LOGIC ---
    if (isScrubbing) {
        // When scrubbing, we check every keyframe to see if the playhead is inside its duration.
        for (const auto& kf : m_keyframes) {
            GSound* sound = kf.GetValue();
            if (!sound) continue;

            float startTime = kf.GetTime();
            float endTime = startTime + sound->Length();

            if (time >= startTime && time <= endTime) {
                // Playhead is inside this clip's range.
                int handle;
                if (m_activeHandles.find(sound) == m_activeHandles.end()) {
                    // If it's not already playing, start it and store the handle.
                    handle = GameAudio::m_Instance->PlaySound(sound);
                    m_activeHandles[sound] = handle;
                }
                else {
                    handle = m_activeHandles[sound];
                }
                // Seek to the correct position and ensure it's not paused.
                GameAudio::m_Instance->gSoloud.setPause(handle, false);
                GameAudio::m_Instance->SeekSound(handle, time - startTime);
            }
            else {
                // Playhead is outside this clip's range.
                if (m_activeHandles.count(sound)) {
                    // If it's playing, just pause it. This avoids jarring stops while scrubbing.
                    GameAudio::m_Instance->gSoloud.setPause(m_activeHandles[sound], true);
                }
            }
        }
    }
    // --- PLAYBACK LOGIC ---
    else {
        // If seeking backwards, reset the time tracker.
        if (time < m_lastTime) { m_lastTime = 0.0f; }

        for (const auto& kf : m_keyframes) {
            if (kf.GetTime() > m_lastTime && kf.GetTime() <= time) {
                GSound* sound = kf.GetValue();
                if (sound) {
                    int handle = GameAudio::m_Instance->PlaySound(sound);
                    m_activeHandles[sound] = handle; // Track this new sound
                }
            }
        }
    }

    // Clean up any sounds that have finished playing on their own.
    for (auto it = m_activeHandles.begin(); it != m_activeHandles.end(); ) {
        if (!GameAudio::m_Instance->gSoloud.isValidVoiceHandle(it->second)) {
            it = m_activeHandles.erase(it);
        }
        else {
            ++it;
        }
    }

    m_lastTime = time; // Update time for the next frame.
}

float TrackAudio::GetEndTime() const
{
    float maxEndTime = 0.0f;
    if (m_keyframes.empty()) {
        return 0.0f;
    }

    // Find the keyframe that ends the latest.
    for (const auto& kf : m_keyframes) {
        float keyframeStartTime = kf.GetTime();
        GSound* sound = kf.GetValue();
        float keyframeEndTime = keyframeStartTime;

        // Add the sound's duration to its start time.
        if (sound) {
            keyframeEndTime += sound->Length();
        }

        if (keyframeEndTime > maxEndTime) {
            maxEndTime = keyframeEndTime;
        }
    }
    return maxEndTime;
}

void TrackAudio::StopAllSounds()
{
    m_activeHandles.clear();
}

void Cinematic::Save(const std::string& path) const
{
    VFile file(path.c_str(), FileMode::Write);
  
    // --- File Header ---
    file.WriteString("CINE"); // Magic number to identify file type
    int version = 1;
    file.WriteInt(version);

    // --- Write Tracks ---
    file.WriteInt(m_tracks.size()); // Write number of tracks

    for (const auto& track : m_tracks) {
        // -- Write Track Header --
        if (auto* t = dynamic_cast<TrackTransform*>(track.get())) {
            file.WriteByte((char)TrackTypeID::Transform);
            file.WriteString(t->GetName().c_str());
            // For transform tracks, save the node's unique long name.
            if (t->GetTargetNode()) {
                file.WriteString(t->GetTargetNode()->GetLongName().c_str());
            }
            else {
                file.WriteString(""); // Write an empty string if node is null
            }
        }
        else if (auto* t = dynamic_cast<TrackAudio*>(track.get())) {
            file.WriteByte((char)TrackTypeID::Audio);
            file.WriteString(t->GetName().c_str());
        }
        else {
            // Skip unknown track types for now
            continue;
        }

        // -- Write Keyframes for this track --
        if (auto* t_transform = dynamic_cast<Track<TransformState>*>(track.get())) {
            const auto& keyframes = t_transform->GetKeyframes();
            file.WriteInt(keyframes.size());
            for (const auto& kf : keyframes) {
                file.WriteFloat(kf.GetTime());
                file.WriteByte((char)kf.GetInterpolation());
                const auto& val = kf.GetValue();
                file.WriteVec3(val.position);
                file.WriteQuat(val.rotation);
                file.WriteVec3(val.scale);
            }
        }
        else if (auto* t_audio = dynamic_cast<Track<GSound*>*>(track.get())) {
            const auto& keyframes = t_audio->GetKeyframes();
            file.WriteInt(keyframes.size());
            for (const auto& kf : keyframes) {
                file.WriteFloat(kf.GetTime());
                file.WriteByte((char)kf.GetInterpolation());
                const auto& val = kf.GetValue();
                if (val) {
                    file.WriteString(val->filePath.c_str());
                }
                else {
                    file.WriteString("");
                }
            }
        }
    }

    file.Close();
}

std::unique_ptr<Cinematic> Cinematic::Load(const std::string& path, SceneGraph* scene)
{
    if (!VFile::Exists(path.c_str())) return nullptr;

    VFile file(path.c_str(), FileMode::Read);
    

    // --- Read and Validate Header ---
    if (std::string(file.ReadString()) != "CINE") {
        file.Close();
        return nullptr; // Not a valid cinematic file
    }
    int version = file.ReadInt();
    if (version != 1) {
        // Handle different file versions here if needed in the future
        file.Close();
        return nullptr;
    }

    auto cinematic = std::make_unique<Cinematic>();
    int trackCount = file.ReadInt();

    for (int i = 0; i < trackCount; ++i) {
        // --- Read Track Data ---
        TrackTypeID typeId = (TrackTypeID)file.ReadByte();
        std::string trackName = file.ReadString();

        std::unique_ptr<ITrack> newTrack = nullptr;

        if (typeId == TrackTypeID::Transform) {
            std::string nodeLongName = file.ReadString();
            GraphNode* targetNode = scene->FindNode(nodeLongName);
            if (targetNode) {
                newTrack = std::make_unique<TrackTransform>(targetNode);
            }
        }
        else if (typeId == TrackTypeID::Audio) {
            newTrack = std::make_unique<TrackAudio>(trackName);
        }

        if (!newTrack) continue; // Skip if track couldn't be created (e.g., node not found)
        newTrack->SetName(trackName);

        // --- Read Keyframes ---
        int keyframeCount = file.ReadInt();
        if (auto* t_transform = dynamic_cast<Track<TransformState>*>(newTrack.get())) {
            for (int j = 0; j < keyframeCount; ++j) {
                float time = file.ReadFloat();
                auto interp = (InterpolationType)file.ReadByte();
                TransformState val;
                val.position = file.ReadVec3();
                val.rotation = file.ReadQuat();
                val.scale = file.ReadVec3();
                t_transform->AddKeyframe({ time, val, interp });
            }
        }
        else if (auto* t_audio = dynamic_cast<TrackAudio*>(newTrack.get())) {
            for (int j = 0; j < keyframeCount; ++j) {
                float time = file.ReadFloat();
                auto interp = (InterpolationType)file.ReadByte(); // Interpolation type isn't used for audio but we read it to advance the file stream
                std::string soundPath = file.ReadString();
                if (!soundPath.empty()) {
                    GSound* sound = GameAudio::m_Instance->LoadSound(soundPath);
                    if (sound) t_audio->AddAudioKeyframe(time, sound);
                }
            }
        }
        cinematic->AddTrack(std::move(newTrack));
    }

    file.Close();
    return cinematic;
}