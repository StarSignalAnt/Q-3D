#pragma once
#include "Component.h"

#include <glm/glm.hpp>

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


private:

    LightType m_Type;
    glm::vec3 m_Color = glm::vec3(1, 1, 1);
    float m_Intensity = 1.0f;
    float m_Range = 100.0f;

};

