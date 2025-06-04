#include "CameraComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Vivid.h"

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