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