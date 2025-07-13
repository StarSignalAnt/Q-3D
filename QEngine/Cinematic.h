#pragma once

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <functional>

#include "GraphNode.h"
#include "CameraComponent.h"
// GLM includes for math operations
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
class GraphNode;
class CameraComponent;
struct TransformState {
    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f }; // Identity quaternion
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
};


// --- Keyframe System ---

// A single point in time on a track, holding a specific value.
template <typename T>
class Keyframe {
public:
    Keyframe(float time, const T& value) : m_time(time), m_value(value) {}

    float GetTime() const { return m_time; }
    const T& GetValue() const { return m_value; }

private:
    float m_time;
    T m_value;
};


// --- Track System (Interface and Generic Implementation) ---

// Abstract base class for all tracks. This allows the Cinematic class
// to hold a collection of different track types (e.g., Transform, Float, etc.).
class ITrack {
public:
    virtual ~ITrack() = default;
    virtual void Update(float time) = 0; // Applies the track's state at a given time
    virtual float GetStartTime() const = 0;
    virtual float GetEndTime() const = 0;
};

// A generic, templated track for animating a specific data type 'T'.
template <typename T>
class Track : public ITrack {
public:
    // Adds a new keyframe and keeps the list sorted by time.
    void AddKeyframe(const Keyframe<T>& keyframe) {
        m_keyframes.push_back(keyframe);
        std::sort(m_keyframes.begin(), m_keyframes.end(), [](const auto& a, const auto& b) {
            return a.GetTime() < b.GetTime();
            });
    }

    // Calculates the interpolated value at a specific time.
    T GetValueAtTime(float time) const {
        if (m_keyframes.empty()) {
            return T(); // Return default-constructed value if no keyframes
        }

        // Clamp to first or last keyframe if outside the time range
        if (time <= m_keyframes.front().GetTime()) {
            return m_keyframes.front().GetValue();
        }
        if (time >= m_keyframes.back().GetTime()) {
            return m_keyframes.back().GetValue();
        }

        // Find the two keyframes that bracket the current time
        for (size_t i = 0; i < m_keyframes.size() - 1; ++i) {
            const auto& startFrame = m_keyframes[i];
            const auto& endFrame = m_keyframes[i + 1];

            if (time >= startFrame.GetTime() && time <= endFrame.GetTime()) {
                float segmentDuration = endFrame.GetTime() - startFrame.GetTime();
                if (segmentDuration <= 1e-6f) {
                    return startFrame.GetValue(); // Avoid division by zero
                }
                // Calculate interpolation factor 't' (0 to 1)
                float t = (time - startFrame.GetTime()) / segmentDuration;
                return Interpolate(startFrame.GetValue(), endFrame.GetValue(), t);
            }
        }
        // Should be unreachable if logic is correct
        throw std::runtime_error("Cinematic Track: Failed to find keyframe segment.");
    }

    float GetStartTime() const override {
        return m_keyframes.empty() ? 0.0f : m_keyframes.front().GetTime();
    }

    float GetEndTime() const override {
        return m_keyframes.empty() ? 0.0f : m_keyframes.back().GetTime();
    }

protected:
    // This must be specialized by concrete track types to handle different interpolations
    virtual T Interpolate(const T& a, const T& b, float t) const = 0;
    std::vector<Keyframe<T>> m_keyframes;
};


// --- Concrete Track Implementations ---

// A track specifically for animating the transform of a GraphNode.
class TrackTransform : public Track<TransformState> {
public:
    TrackTransform(GraphNode* target) : m_targetNode(target) {
        if (!m_targetNode) {
            throw std::invalid_argument("TrackTransform must have a valid target GraphNode.");
        }
    }

    // Applies the interpolated transform to the target GraphNode.
    void Update(float time) override {
        if (!m_targetNode) return;

        TransformState state = GetValueAtTime(time);
        m_targetNode->SetPosition(state.position);
        m_targetNode->SetRotation(state.rotation); // Assumes GraphNode::SetRotation(glm::quat) exists
        m_targetNode->SetScale(state.scale);
    }

protected:
    // Handles interpolation for TransformState using Lerp for vectors and Slerp for quaternions.
    TransformState Interpolate(const TransformState& a, const TransformState& b, float t) const override {
        TransformState result;
        result.position = glm::lerp(a.position, b.position, t);
        result.rotation = glm::slerp(a.rotation, b.rotation, t);
        result.scale = glm::lerp(a.scale, b.scale, t);
        return result;
    }

private:
    GraphNode* m_targetNode;
};

// A highly flexible track for animating any single float property.
// It uses a std::function "setter" to apply the value, making it generic.
class TrackFloat : public Track<float> {
public:
    // The constructor takes a "setter" function (e.g., a lambda) that knows
    // how to apply the float value to the target object.
    TrackFloat(std::function<void(float)> setter) : m_setter(setter) {
        if (!m_setter) {
            throw std::invalid_argument("TrackFloat must have a valid setter function.");
        }
    }

    void Update(float time) override {
        if (m_setter) {
            m_setter(GetValueAtTime(time));
        }
    }

protected:
    // Simple linear interpolation for floats.
    float Interpolate(const float& a, const float& b, float t) const override {
        return a + (b - a) * t;
    }

private:
    std::function<void(float)> m_setter;
};


// Manages a collection of tracks and controls the overall playback.
class Cinematic {
public:
    void AddTrack(std::unique_ptr<ITrack> track) {
        if (track) {
            m_tracks.push_back(std::move(track));
        }
    }

    // Sets the current time for the entire cinematic, updating all tracks.
    void SetTime(float time) {
        m_currentTime = time;
        for (const auto& track : m_tracks) {
            track->Update(m_currentTime);
        }
    }

    float GetCurrentTime() const {
        return m_currentTime;
    }

    // Calculates the total duration of the cinematic based on the latest keyframe.
    float GetDuration() const {
        float maxTime = 0.0f;
        for (const auto& track : m_tracks) {
            if (track) {
                maxTime = std::max(maxTime, track->GetEndTime());
            }
        }
        return maxTime;
    }

private:
    float m_currentTime = 0.0f;
    std::vector<std::unique_ptr<ITrack>> m_tracks;
};

