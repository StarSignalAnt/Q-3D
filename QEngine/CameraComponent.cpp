#include "CameraComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include "QEngine.h"
#include "GraphNode.h"
#include "GameInput.h"


CameraComponent::CameraComponent() {

    m_Name = "Camera Component";
    m_Category = "Rendering";
    
}

glm::mat4 CameraComponent::GetProjectionMatrix() {
    if (useOver) {
        return m_ProjOver;
    }
    float aspectRatio = ((float)Q3D::Engine::QEngine::GetFrameWidth()) / ((float)Q3D::Engine::QEngine::GetFrameHeight());
    glm::mat4 projection = glm::perspective(
        glm::radians(m_FOV), // glm expects radians
        aspectRatio,
        m_NearPlane,
        m_FarPlane
    );

    return projection;
}

glm::mat4 CameraComponent::Get2DProjectionMatrix() {
    return glm::orthoRH<float>(0, Q3D::Engine::QEngine::GetFrameWidth(), Q3D::Engine::QEngine::GetFrameHeight(), 0, 1, -1);

    float aspectRatio = ((float)Q3D::Engine::QEngine::GetFrameWidth()) / ((float)Q3D::Engine::QEngine::GetFrameHeight());
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
    float viewportWidth = (float)Q3D::Engine::QEngine::GetFrameWidth();
    float viewportHeight = (float)Q3D::Engine::QEngine::GetFrameHeight();

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

void CameraComponent::UpdateFrustumPlanes() {
    glm::mat4 vp = GetProjectionMatrix() * GetViewMatrix();

    // Avoid recalculation if the view-projection matrix has not changed
    if (vp == m_LastViewProjMatrix) {
        return;
    }
    m_LastViewProjMatrix = vp;

    // Transpose the matrix to make row access easier
    glm::mat4 vpt = glm::transpose(vp);

    // Extract the 6 planes
    m_FrustumPlanes[0] = { glm::vec3(vpt[3] + vpt[0]), vpt[3].w + vpt[0].w }; // Left
    m_FrustumPlanes[1] = { glm::vec3(vpt[3] - vpt[0]), vpt[3].w - vpt[0].w }; // Right
    m_FrustumPlanes[2] = { glm::vec3(vpt[3] + vpt[1]), vpt[3].w + vpt[1].w }; // Bottom
    m_FrustumPlanes[3] = { glm::vec3(vpt[3] - vpt[1]), vpt[3].w - vpt[1].w }; // Top
    m_FrustumPlanes[4] = { glm::vec3(vpt[3] + vpt[2]), vpt[3].w + vpt[2].w }; // Near
    m_FrustumPlanes[5] = { glm::vec3(vpt[3] - vpt[2]), vpt[3].w - vpt[2].w }; // Far

    // Normalize the plane equations
    for (int i = 0; i < 6; ++i) {
        float length = glm::length(m_FrustumPlanes[i].normal);
        m_FrustumPlanes[i].normal /= length;
        m_FrustumPlanes[i].distance /= length;
    }
}

/**
 * @brief Performs an AABB-Frustum intersection test.
*/
bool CameraComponent::InFrustum(const Bounds& bounds) {
    // Update the frustum planes to the camera's current position and orientation
    UpdateFrustumPlanes();

    // An invalid bounding box cannot be in the frustum
    if (!bounds.IsValid()) {
        return false;
    }

    // Check the AABB against each of the 6 frustum planes
    for (int i = 0; i < 6; ++i) {
        const auto& plane = m_FrustumPlanes[i];

        // Find the vertex of the AABB that is furthest along the plane's normal
        glm::vec3 p_vertex;
        p_vertex.x = (plane.normal.x > 0) ? bounds.max.x : bounds.min.x;
        p_vertex.y = (plane.normal.y > 0) ? bounds.max.y : bounds.min.y;
        p_vertex.z = (plane.normal.z > 0) ? bounds.max.z : bounds.min.z;

        // Calculate the signed distance of this vertex to the plane.
        // If the distance is negative, the vertex is outside the plane.
        // Since this is the "most inside" vertex, if it's outside, the whole box is.
        if (glm::dot(plane.normal, p_vertex) + plane.distance < 0) {
            // The entire box is outside this plane, so it cannot be in the frustum.
            return false;
        }
    }

    // If the box was not fully outside any plane, it is intersecting or inside the frustum.
    return true;
}

glm::vec2 CameraComponent::GetMouseCoords()
{
    // This function needs to get the mouse position from your engine's input manager.
    // Based on your code, it's likely QEngine handles this.
    // NOTE: You may need to adjust this if your engine uses a different function name.
    return glm::vec2((float)GameInput::MousePosition.x, (float)GameInput::MousePosition.y);
}

void CameraComponent::ScreenToWorldRay(glm::vec2 mouseCoords, glm::vec3& out_origin, glm::vec3& out_direction)
{
    // The ray's origin is simply the camera's position in the world.
    out_origin = m_Owner->GetPosition();

    // 1. Convert 2D screen coordinates to 3D Normalized Device Coordinates (NDC)
    //    The coordinate system goes from [-1, 1] on all axes.
    float x = (2.0f * mouseCoords.x) / (float)Q3D::Engine::QEngine::GetFrameWidth() - 1.0f;
    float y = 1.0f - (2.0f * mouseCoords.y) / (float)Q3D::Engine::QEngine::GetFrameHeight(); // Y is inverted in NDC
    glm::vec3 ray_ndc = glm::vec3(x, y, 1.0f);

    // 2. Convert NDC to Homogeneous Clip Coordinates
    //    We want a direction vector, so we set z to -1.0 (forward) and w to 1.0.
    glm::vec4 ray_clip = glm::vec4(ray_ndc.x, ray_ndc.y, -1.0, 1.0);

    // 3. Convert Homogeneous Clip Coordinates to Eye (Camera) Coordinates
    //    We do this by multiplying by the inverse of the projection matrix.
    glm::vec4 ray_eye = glm::inverse(GetProjectionMatrix()) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0); // Set w to 0 to make it a direction

    // 4. Convert Eye Coordinates to World Coordinates
    //    We do this by multiplying by the inverse of the view matrix (which is the world matrix).
    glm::vec3 ray_world = glm::vec3(m_Owner->GetWorldMatrix() * ray_eye);

    out_direction = glm::normalize(ray_world);
}