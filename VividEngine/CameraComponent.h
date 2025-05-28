#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class CameraComponent :
    public Component
{
public:

    glm::mat4 GetProjectionMatrix();


private:

    float m_FOV = 60.0f;
    float m_NearPlane = 0.1f;
    float m_FarPlane = 100.0f;


};

