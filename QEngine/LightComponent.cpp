#include "LightComponent.h"
#include "RenderTargetCube.h"
#include "RenderTarget2D.h" // You will need to include your 2D render target header
#include <glm/gtc/matrix_transform.hpp>

const std::vector<std::pair<LightType, std::string>> LightTypeOptions = {
{ LightType::Point, "Point" },
{ LightType::Directional, "Directional" },
{ LightType::Spot,"Spot"}
};

LightComponent::LightComponent() {
    // Constructor no longer allocates a default shadow map.
    // Call SetLightType after creation to initialize resources.
    m_Properties.bind("Color", &m_Color);
    m_Properties.bind("Range", &m_Range);
    m_Properties.bind("Intensity", &m_Intensity);
    m_Name = "LightComponent";

    m_Properties.set("ComponentName", &m_Name);
    m_Properties.bind_enum("LightType", &m_Type, LightTypeOptions);

}

LightComponent::~LightComponent() {
    // Ensure we clean up any allocated memory
    ClearShadowMaps();
}

void LightComponent::ClearShadowMaps() {
    delete m_ShadowMapCube;
    m_ShadowMapCube = nullptr;

    delete m_DirectionalShadowMap;
    m_DirectionalShadowMap = nullptr;
}

void LightComponent::SetLightType(LightType type) {
    m_Type = type;

    // Clean up any existing maps before creating a new one
    ClearShadowMaps();

    // Allocate the appropriate shadow map for the new light type
    switch (m_Type) {
    case LightType::Point:
        m_ShadowMapCube = new RenderTargetCube(2048, 2048, true);
        break;
    case LightType::Directional:
        // For directional lights, we need a single 2D shadow map
        m_DirectionalShadowMap = new RenderTarget2D(4096, 4096, true); // Higher res is common for directional
        break;
    case LightType::Spot:
        // Spotlights also use a 2D shadow map
        // m_DirectionalShadowMap = new RenderTarget2D(2048, 2048, true);
        break;
    }
}

void LightComponent::SetDirection(glm::vec3 direction) {
    m_Direction = glm::normalize(direction);
}

void LightComponent::UpdateLightSpaceMatrix() {
    // This function is only relevant for lights that use a single projection,
    // like Directional or Spot lights.
    return;
    if (m_Type == LightType::Directional) {
        // 1. Create the Orthographic Projection Matrix
        // This defines the "box" that will be visible to the light.
        // You'll want to adjust these values to fit your scene's size.
        float near_plane = 1.0f;
        float far_plane = 100.0f;
        float shadow_area_size = 25.0f;
        glm::mat4 lightProjection = glm::ortho(-shadow_area_size, shadow_area_size,
            -shadow_area_size, shadow_area_size,
            near_plane, far_plane);

        // 2. Create the View Matrix
        // This positions the "camera" for the light. We place it somewhere looking
        // towards the scene's center from the direction of the light.
        // A simple approach is to place it at some distance from the origin.
        glm::vec3 lightPos = -m_Direction * 30.0f; // Move camera back along the light direction
        glm::vec3 lookAtTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Look at the world origin
        glm::vec3 up_vector = glm::vec3(0.0f, 1.0f, 0.0f);

        // Handle cases where light is pointing straight up or down
        if (abs(glm::dot(m_Direction, up_vector)) > 0.99f) {
            up_vector = glm::vec3(0.0f, 0.0f, 1.0f);
        }

        glm::mat4 lightView = glm::lookAt(lightPos, lookAtTarget, up_vector);

        // 3. Combine them to create the final matrix
        m_LightSpaceMatrix = lightProjection * lightView;
    }
}