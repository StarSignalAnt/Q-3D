#pragma once
#include "Component.h"

#include <glm/glm.hpp>

class RenderTargetCube;

enum class LightType {
    Directional,
    Point,
    Spot
};

class LightComponent :
    public Component
{
public:

    LightComponent();
    float GetRange();
    glm::vec3 GetColor();
    float GetIntensity() {
        return m_Intensity;
    }
    void SetColor(glm::vec3 color) {
        m_Color = color;
    }
    RenderTargetCube* GetShadowMap() {
        return m_ShadowMap;
	}
    void SetRange(float range) {
        m_Range = range;
    }
    void SetIntensity(float i)
    {
        m_Intensity = i;
    }
private:

    LightType m_Type;
    glm::vec3 m_Color = glm::vec3(1, 1, 1);
    float m_Intensity = 40.0f;
    float m_Range = 55.0f;
    RenderTargetCube* m_ShadowMap = nullptr;
};

