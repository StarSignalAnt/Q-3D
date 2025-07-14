#include "Cinematic.h"
#include "GraphNode.h" // The full definition of GraphNode is included here

// ===================================================================================
// TrackTransform Implementation
// ===================================================================================

TrackTransform::TrackTransform(GraphNode* target) : m_targetNode(target) {
    if (!m_targetNode) {
        throw std::invalid_argument("TrackTransform must have a valid target GraphNode.");
    }
    m_name = m_targetNode->GetName();
}

void TrackTransform::Update(float time) {
    if (!m_targetNode) return;

    TransformState state = GetValueAtTime(time);
    m_targetNode->SetPosition(state.position);
    m_targetNode->SetRotation(state.rotation);
    m_targetNode->SetScale(state.scale);
}

// --- MODIFIED --- This function now checks for nearby keyframes before creating a new one.
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