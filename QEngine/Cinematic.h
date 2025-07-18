#pragma once

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <cmath> // For std::abs
#include <map>
// GLM includes for math operations
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "PixelMap.h"
#include <glm/gtc/epsilon.hpp>


// Forward-declare engine classes
class GraphNode;
struct GSound;
class GameAudio; // For the singleton access
class SceneGraph;
class GameVideo;
// Enum to define how to interpolate from a keyframe.
enum class InterpolationType {
    Linear, // Default smooth curve (lerp/slerp)
    Snapped // Holds value until the next keyframe
};

// Represents the transform state (position, rotation, scale) of a GraphNode.
struct TransformState {
    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f }; // Identity quaternion
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

    bool operator==(const TransformState& other) const {
        constexpr float epsilon = 1e-5f;
        return glm::all(glm::epsilonEqual(position, other.position, epsilon)) &&
            glm::all(glm::epsilonEqual(rotation, other.rotation, epsilon)) &&
            glm::all(glm::epsilonEqual(scale, other.scale, epsilon));
    }
};

// A single point in time on a track.
template <typename T>
class Keyframe {
public:
    Keyframe(float time, const T& value, InterpolationType interp = InterpolationType::Linear)
        : m_time(time), m_value(value), m_interpolation(interp) {
    }

    float GetTime() const { return m_time; }
    const T& GetValue() const { return m_value; }
    InterpolationType GetInterpolation() const { return m_interpolation; }

    void setInterpolation(InterpolationType interp) { m_interpolation = interp; }
    void SetValue(const T& val) { m_value = val; }

private:
    float m_time;
    T m_value;
    InterpolationType m_interpolation;
};

// Abstract base class for all tracks.
class ITrack {
public:
    virtual ~ITrack() = default;
    virtual void Update(float time, bool isScrubbing) = 0;

    virtual void RecordKeyframe(float time, InterpolationType interpType) = 0;
    virtual void RecordKeyframe(float time) { RecordKeyframe(time, InterpolationType::Linear); }
    virtual const std::string& GetName() const = 0;
    virtual void SetName(const std::string& newName) = 0;
    virtual float GetStartTime() const = 0;
    virtual float GetEndTime() const = 0;
    virtual std::vector<float> GetKeyframeTimes() const = 0;
    virtual std::vector<InterpolationType> GetKeyframeInterpolationTypes() const = 0;
    virtual size_t ModifyKeyframeTime(size_t keyframeIndex, float newTime) = 0;
    virtual void SetKeyframeInterpolation(size_t keyframeIndex, InterpolationType interp) = 0;
    virtual void StopAllSounds() {}
};

// A generic, templated track for animating a specific data type 'T'.
template <typename T>
class Track : public ITrack {
public:
    const std::vector<Keyframe<T>>& GetKeyframes() const { return m_keyframes; }
    void Update(float time, bool isScrubbing) override {}
    void AddKeyframe(const Keyframe<T>& keyframe) {
        m_keyframes.push_back(keyframe);
        std::sort(m_keyframes.begin(), m_keyframes.end(), [](const auto& a, const auto& b) {
            return a.GetTime() < b.GetTime();
            });
    }

    T GetValueAtTime(float time) const {
        if (m_keyframes.empty()) { return T(); }
        if (time <= m_keyframes.front().GetTime()) { return m_keyframes.front().GetValue(); }
        if (time >= m_keyframes.back().GetTime()) { return m_keyframes.back().GetValue(); }
        for (size_t i = 0; i < m_keyframes.size() - 1; ++i) {
            const auto& startFrame = m_keyframes[i];
            const auto& endFrame = m_keyframes[i + 1];
            if (time >= startFrame.GetTime() && time < endFrame.GetTime()) {
                if (startFrame.GetInterpolation() == InterpolationType::Snapped) {
                    return startFrame.GetValue();
                }
                else {
                    float segmentDuration = endFrame.GetTime() - startFrame.GetTime();
                    if (segmentDuration <= 1e-6f) { return startFrame.GetValue(); }
                    float t = (time - startFrame.GetTime()) / segmentDuration;
                    return Interpolate(startFrame.GetValue(), endFrame.GetValue(), t);
                }
            }
        }
        return m_keyframes.back().GetValue();
    }

    float GetStartTime() const override { return m_keyframes.empty() ? 0.0f : m_keyframes.front().GetTime(); }
    float GetEndTime() const override { return m_keyframes.empty() ? 0.0f : m_keyframes.back().GetTime(); }

    std::vector<float> GetKeyframeTimes() const override {
        std::vector<float> times;
        times.reserve(m_keyframes.size());
        for (const auto& kf : m_keyframes) {
            times.push_back(kf.GetTime());
        }
        return times;
    }

    std::vector<InterpolationType> GetKeyframeInterpolationTypes() const override {
        std::vector<InterpolationType> types;
        types.reserve(m_keyframes.size());
        for (const auto& kf : m_keyframes) {
            types.push_back(kf.GetInterpolation());
        }
        return types;
    }

    size_t ModifyKeyframeTime(size_t keyframeIndex, float newTime) override {
        if (keyframeIndex >= m_keyframes.size()) {
            throw std::out_of_range("Keyframe index is out of range.");
        }
        T value = m_keyframes[keyframeIndex].GetValue();
        InterpolationType interp = m_keyframes[keyframeIndex].GetInterpolation();
        m_keyframes.erase(m_keyframes.begin() + keyframeIndex);
        AddKeyframe({ newTime, value, interp });
        auto it = std::find_if(m_keyframes.begin(), m_keyframes.end(),
            [&](const Keyframe<T>& kf) {
                return std::abs(kf.GetTime() - newTime) < 1e-5f && kf.GetValue() == value;
            });
        if (it != m_keyframes.end()) {
            return std::distance(m_keyframes.begin(), it);
        }
        throw std::runtime_error("FATAL: Could not find keyframe after modification.");
    }

    void SetKeyframeInterpolation(size_t keyframeIndex, InterpolationType interp) override {
        if (keyframeIndex < m_keyframes.size()) {
            m_keyframes[keyframeIndex].setInterpolation(interp);
        }
    }

protected:
    virtual T Interpolate(const T& a, const T& b, float t) const = 0;
    std::vector<Keyframe<T>> m_keyframes;
};

class TrackVideo : public Track<GameVideo*> {
public:
    TrackVideo(std::string name);
    ~TrackVideo() override;
    void Update(float time, bool isScrubbing) override;

    // Video keyframes are added programmatically or via drag-and-drop, not the record button.
    void RecordKeyframe(float time, InterpolationType interpType) override {}
    void AddVideoKeyframe(float time, GameVideo* video);

    const std::string& GetName() const override { return m_name; }
    void SetName(const std::string& newName) override { m_name = newName; }
    float GetEndTime() const override;
    void StopAllSounds() override; // This will also be used to stop video
    const std::map<GameVideo*, std::vector<PixelMap*>>& GetThumbnailCache() const { return m_thumbnailCache; }

protected:
    // Interpolation isn't applicable to video clips.
    GameVideo* Interpolate(GameVideo* const& a, GameVideo* const& b, float t) const override;

private:
    std::string m_name;
    GameVideo* m_activeVideo = nullptr;
    float m_lastTime = 0.0f;
    void generateThumbnailsForVideo(GameVideo* video);
    std::map<GameVideo*, std::vector<PixelMap*>> m_thumbnailCache;
};

// A track specifically for animating the transform of a GraphNode.
class TrackTransform : public Track<TransformState> {
public:
    TrackTransform(GraphNode* target);
    void Update(float time, bool isScrubbing) override;
    void RecordKeyframe(float time, InterpolationType interpType) override;
    const std::string& GetName() const override { return m_name; }
    void SetName(const std::string& newName) override { m_name = newName; }
    GraphNode* GetTargetNode() const { return m_targetNode; }
protected:
    TransformState Interpolate(const TransformState& a, const TransformState& b, float t) const override;

private:
    GraphNode* m_targetNode;
    std::string m_name;
};

// A highly flexible track for animating any single float property.
class TrackFloat : public Track<float> {
public:
    TrackFloat(std::string name, std::function<void(float)> setter, std::function<float()> getter)
        : m_name(std::move(name)), m_setter(setter), m_getter(getter) {
        if (!m_setter || !m_getter) {
            throw std::invalid_argument("TrackFloat must have a valid setter and getter function.");
        }
    }

    void Update(float time, bool isScrubbing) override { m_setter(GetValueAtTime(time)); } // Added flag

    void RecordKeyframe(float time, InterpolationType interpType) override {
        constexpr float threshold = 0.1f;
        for (size_t i = 0; i < m_keyframes.size(); ++i) {
            if (std::abs(m_keyframes[i].GetTime() - time) <= threshold) {
                m_keyframes[i].SetValue(m_getter());
                m_keyframes[i].setInterpolation(interpType);
                return;
            }
        }
        AddKeyframe(Keyframe<float>(time, m_getter(), interpType));
    }

    const std::string& GetName() const override { return m_name; }
    void SetName(const std::string& newName) override { m_name = newName; }
protected:
    float Interpolate(const float& a, const float& b, float t) const override { return a + (b - a) * t; }

private:
    std::string m_name;
    std::function<void(float)> m_setter;
    std::function<float()> m_getter;
};

// --- ADDED --- A track for triggering audio events.
class TrackAudio : public Track<GSound*> {
public:
    TrackAudio(std::string name) : m_name(std::move(name)), m_lastTime(0.0f) {}
    ~TrackAudio() override;
    // Update is called every frame; it checks if the playhead has crossed a keyframe.
    void Update(float time, bool isScrubbing) override;

    // This track type doesn't support generic recording from the UI.
    // Keyframes must be added programmatically with a specific sound.
    void RecordKeyframe(float time, InterpolationType interpType) override {
        // This method is not applicable to audio tracks in the same way as transform tracks.
        // The UI would need to pop up a file dialog to select a sound.
        // For now, we do nothing.
    }

    // A specific method to add audio keyframes.
    void AddAudioKeyframe(float time, GSound* sound) {
        // Audio events are instantaneous, so they should always be Snapped.
        AddKeyframe(Keyframe<GSound*>(time, sound, InterpolationType::Snapped));
    }
    float GetEndTime() const override;

    const std::string& GetName() const override { return m_name; }
    void SetName(const std::string& newName) override { m_name = newName; }
    void StopAllSounds() override;
protected:
    // Interpolation doesn't make sense for sound events. We just return the value.
    GSound* Interpolate(GSound* const& a, GSound* const& b, float t) const override {
        (void)b; (void)t; // Unused parameters
        return a;
    }

private:
    std::string m_name;
    float m_lastTime;
    std::map<GSound*, int> m_activeHandles;
};


// Manages a collection of tracks and controls the overall playback.
class Cinematic {
public:

    static std::unique_ptr<Cinematic> Load(const std::string& path, SceneGraph* scene);
    void Save(const std::string& path) const;

    void AddTrack(std::unique_ptr<ITrack> track) {
        if (track) { m_tracks.push_back(std::move(track)); }
    }
    void SetTime(float time, bool isScrubbing) {
        m_currentTime = time;
        for (const auto& track : m_tracks) {
            track->Update(m_currentTime, isScrubbing);
        }
    }

    float GetCurrentTime() const { return m_currentTime; }
    const std::vector<std::unique_ptr<ITrack>>& GetTracks() const { return m_tracks; }

    float GetDuration() const {
        float maxTime = 0.0f;
        for (const auto& track : m_tracks) {
            if (track) { maxTime = std::max(maxTime, track->GetEndTime()); }
        }
        return maxTime;
    }

private:
    float m_currentTime = 0.0f;
    std::vector<std::unique_ptr<ITrack>> m_tracks;
};