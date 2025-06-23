#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class GraphNode; // Forward declaration
class GraphNode; // Forward declaration
struct Bounds;   // Forward declaration for the Bounds struct
class CameraComponent :
    public Component
{
public:

    glm::mat4 GetProjectionMatrix();
    glm::mat4 Get2DProjectionMatrix();
    glm::mat4 GetViewMatrix();
    bool InFrustum(const Bounds& bounds);
    // Project 3D world position to screen coordinates
    glm::vec2 WorldToScreen(const glm::vec3& worldPos);

    void SetFOV(float fov) {
        m_FOV = fov;
    }
    float GetFOV() {
        return m_FOV;
    }
    glm::vec4 GetExtents() {
        return glm::vec4(m_NearPlane, m_FarPlane, 0, 0);
    }
    void SetExtents(float nearPlane, float farPlane) {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
    }

private:
    struct FrustumPlane
    {
        glm::vec3 normal;
        float distance;
    };

    // Updates the frustum planes based on the current view-projection matrix
    void UpdateFrustumPlanes();

    FrustumPlane m_FrustumPlanes[6];
    glm::mat4 m_LastViewProjMatrix = glm::mat4(0.0f); // 
    float m_FOV = 60.0f;
    float m_NearPlane = 0.1f;
    float m_FarPlane = 100.0f;
};