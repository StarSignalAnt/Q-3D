#include "Cinematic.h"
#include "GraphNode.h"
#include "GameAudio.h" // Required for the audio singleton

// ===================================================================================
// TrackTransform Implementation
// ===================================================================================

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