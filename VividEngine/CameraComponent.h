#pragma once
#include "Component.h"
#include <glm/glm.hpp>



class CameraComponent :
    public Component
{
public:

    glm::mat4 GetProjectionMatrix();
    void SetFOV(float fov) {
        m_FOV = fov;
    }
    glm::vec4 GetExtents() {
        return glm::vec4(m_NearPlane, m_FarPlane, 0, 0);
    }
    void SetExtents(float nearPlane, float farPlane) {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
	}


private:

    float m_FOV = 60.0f;
    float m_NearPlane = 0.1f;
    float m_FarPlane = 100.0f;
 


};

