#pragma once
#include "Component.h"
#include <glm/glm.hpp>

// Forward declare both types of render targets
class RenderTargetCube;
class RenderTarget2D; // Assuming you have a 2D render target class

enum class LightType {
    Point,
    Directional,
    Spot
};

class LightComponent :
    public Component
{
public:
    LightComponent();
    ~LightComponent(); // Destructor is now needed to clean up pointers

    // Common properties
    float GetIntensity() { return m_Intensity; }
    glm::vec3 GetColor() { return m_Color; }
    LightType GetLightType() { return m_Type; }
    void SetIntensity(float i) { m_Intensity = i; }
    void SetColor(glm::vec3 color) { m_Color = color; }

    // Type-specific setup
    void SetLightType(LightType type);

    // Point Light specific
    float GetRange() { return m_Range; }
    void SetRange(float range) { m_Range = range; }
    RenderTargetCube* GetShadowMap() { return m_ShadowMapCube; }

    // Directional Light specific
    void SetDirection(glm::vec3 direction);
    glm::vec3 GetDirection() { return m_Direction; }
    const glm::mat4& GetLightSpaceMatrix() const { return m_LightSpaceMatrix; }
    RenderTarget2D* GetDirectionalShadowMap() { return m_DirectionalShadowMap; }

    // This function will calculate the view/projection matrix for the light
    void UpdateLightSpaceMatrix();
    void SetLightSpaceMatrix(glm::mat4 mat) {
        m_LightSpaceMatrix = mat;
    }

private:
    void ClearShadowMaps(); // Helper function to deallocate memory

    LightType m_Type = LightType::Point;
    glm::vec3 m_Color = glm::vec3(1, 1, 1);
    float m_Intensity = 40.0f;

    // --- Point Light Properties ---
    float m_Range = 55.0f;
    RenderTargetCube* m_ShadowMapCube = nullptr;

    // --- Directional Light Properties ---
    glm::vec3 m_Direction = glm::vec3(0.0f, -1.0f, 0.0f);
    RenderTarget2D* m_DirectionalShadowMap = nullptr;
    glm::mat4 m_LightSpaceMatrix = glm::mat4(1.0f); // Stores combined view*projection
};