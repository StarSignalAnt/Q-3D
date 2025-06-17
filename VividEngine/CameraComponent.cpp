#include "CameraComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Vivid.h"
#include "GraphNode.h"

glm::mat4 CameraComponent::GetProjectionMatrix() {

    float aspectRatio = ((float)Vivid::GetFrameWidth()) / ((float)Vivid::GetFrameHeight());
    glm::mat4 projection = glm::perspective(
        glm::radians(m_FOV), // glm expects radians
        aspectRatio,
        m_NearPlane,
        m_FarPlane
    );

    return projection;
}

glm::mat4 CameraComponent::Get2DProjectionMatrix() {
    return glm::orthoRH<float>(0, Vivid::GetFrameWidth(), Vivid::GetFrameHeight(), 0, 1, -1);

    float aspectRatio = ((float)Vivid::GetFrameWidth()) / ((float)Vivid::GetFrameHeight());
    glm::mat4 projection = glm::perspective(
        glm::radians(m_FOV), // glm expects radians
        aspectRatio,
        m_NearPlane,
        m_FarPlane
    );

    return projection;
}

glm::mat4 CameraComponent::GetViewMatrix() {
    if (!m_Owner) {
        return glm::mat4(1.0f); // Identity matrix if no owner
    }

    // Get the inverse of the camera's world transform matrix
    // This transforms from world space to camera/view space
    return glm::inverse(m_Owner->GetWorldMatrix());
}

glm::vec2 CameraComponent::WorldToScreen(const glm::vec3& worldPos) {
    // Get viewport dimensions
    float viewportWidth = (float)Vivid::GetFrameWidth();
    float viewportHeight = (float)Vivid::GetFrameHeight();

    // Transform world position to clip space
    glm::vec4 clipSpacePos = GetProjectionMatrix() * GetViewMatrix() * glm::vec4(worldPos, 1.0f);

    // Perform perspective divide to get normalized device coordinates (NDC)
    if (clipSpacePos.w == 0.0f) {
        // Point is at infinity, return invalid screen coordinates
        return glm::vec2(-1.0f, -1.0f);
    }

    glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w;

    // Check if point is behind the camera (negative Z in view space)
    if (clipSpacePos.w < 0.0f) {
        // Point is behind camera, return invalid screen coordinates
        return glm::vec2(-1.0f, -1.0f);
    }

    // Convert from NDC [-1, 1] to screen coordinates [0, viewport_size]
    glm::vec2 screenPos;
    screenPos.x = (ndcPos.x + 1.0f) * 0.5f * viewportWidth;
    screenPos.y = (1.0f - ndcPos.y) * 0.5f * viewportHeight; // Flip Y axis (OpenGL convention)

    return screenPos;
}