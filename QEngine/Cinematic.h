#pragma once

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <cmath> // For std::abs

// GLM includes for math operations
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/epsilon.hpp>

// Forward-declare the GraphNode class to avoid circular dependencies
class GraphNode;

// Enum to define how to interpolate from a keyframe.
enum class InterpolationType {
    Linear, // Default smooth curve (lerp/slerp)
    Stepped // Holds value until the next keyframe
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
    // --- ADDED --- New method to update a keyframe's value.
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
    virtual void Update(float time) = 0;
    virtual void RecordKeyframe(float time, InterpolationType interpType) = 0;
    virtual void RecordKeyframe(float time) { RecordKeyframe(time, InterpolationType::Linear); }
    virtual const std::string& GetName() const = 0;
    virtual float GetStartTime() const = 0;
    virtual float GetEndTime() const = 0;
    virtual std::vector<float> GetKeyframeTimes() const = 0;
    virtual std::vector<InterpolationType> GetKeyframeInterpolationTypes() const = 0;
    virtual size_t ModifyKeyframeTime(size_t keyframeIndex, float newTime) = 0;
    virtual void SetKeyframeInterpolation(size_t keyframeIndex, InterpolationType interp) = 0;
};

// A generic, templated track for animating a specific data type 'T'.
template <typename T>
class Track : public ITrack {
public:
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
                if (startFrame.GetInterpolation() == InterpolationType::Stepped) {
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

// A track specifically for animating the transform of a GraphNode.
class TrackTransform : public Track<TransformState> {
public:
    TrackTransform(GraphNode* target);
    void Update(float time) override;
    void RecordKeyframe(float time, InterpolationType interpType) override;
    const std::string& GetName() const override { return m_name; }

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

    void Update(float time) override { m_setter(GetValueAtTime(time)); }

    // --- MODIFIED --- This function now checks for nearby keyframes before creating a new one.
    void RecordKeyframe(float time, InterpolationType interpType) override {
        constexpr float threshold = 0.1f;

        // Check if a keyframe exists nearby
        for (size_t i = 0; i < m_keyframes.size(); ++i) {
            if (std::abs(m_keyframes[i].GetTime() - time) <= threshold) {
                // If so, update its value and interpolation type, then return.
                m_keyframes[i].SetValue(m_getter());
                m_keyframes[i].setInterpolation(interpType);
                return;
            }
        }
        // If no nearby keyframe was found, create a new one.
        AddKeyframe(Keyframe<float>(time, m_getter(), interpType));
    }

    const std::string& GetName() const override { return m_name; }

protected:
    float Interpolate(const float& a, const float& b, float t) const override { return a + (b - a) * t; }

private:
    std::string m_name;
    std::function<void(float)> m_setter;
    std::function<float()> m_getter;
};

// Manages a collection of tracks and controls the overall playback.
class Cinematic {
public:
    void AddTrack(std::unique_ptr<ITrack> track) {
        if (track) { m_tracks.push_back(std::move(track)); }
    }

    void SetTime(float time) {
        m_currentTime = time;
        for (const auto& track : m_tracks) { track->Update(m_currentTime); }
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