#include "CameraController.h"
#include <qvector3d.h>

void CameraController::update(float dt, const QSet<int>& keysHeld, const QPoint& mouseDelta, bool rmbDown) {
    QVector3D movement;
   // if (keysHeld.contains(Qt::Key_W)) movement += camera->forward();
   // if (keysHeld.contains(Qt::Key_S)) movement -= camera->forward();
   // if (keysHeld.contains(Qt::Key_A)) movement -= camera->right();
   // if (keysHeld.contains(Qt::Key_D)) movement += camera->right();


    glm::vec3 move(0, 0, 0);

    if (keysHeld.contains(Qt::Key_W))
        move += glm::vec3(0, 0, -0.1f);
    if (keysHeld.contains(Qt::Key_S))
        move -= glm::vec3(0, 0, -0.1f);


    if (keysHeld.contains(Qt::Key_A))
        move += glm::vec3(-0.1, 0, 0);
    if (keysHeld.contains(Qt::Key_D))
        move -= glm::vec3(-0.1, 0, 0);
    //    direction += right;

    float speed = moveSpeed;

    if (keysHeld.contains(Qt::Key_Shift))
    {
        speed = speed * quick;
    }

    camera->Move(move * speed * dt);


    if (rmbDown) {
        float sensitivity = 0.1f;
        yaw -= mouseDelta.x() * sensitivity;
        pitch -= mouseDelta.y() * sensitivity;

        // Clamp pitch if needed
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // Update your camera orientation here using yaw & pitch
        // E.g. camera->SetRotationFromYawPitch(yaw, pitch);
        camera->SetRotation(glm::vec3(pitch, yaw, 0));
        
        //camera->yaw += mouseDelta.x() * rotationSpeed * dt;
        //camera->pitch += -mouseDelta.y() * rotationSpeed * dt;
        //camera->updateVectors();

    }
}

void CameraController::updateMouse(float deltaTime, const QPoint& mouseDelta, bool rmbDown)
{
    if (rmbDown) {
        float sensitivity = 0.1f;
        yaw -= mouseDelta.x() * sensitivity;
        pitch -= mouseDelta.y() * sensitivity;

        // Clamp pitch if needed
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // Update your camera orientation here using yaw & pitch
        // E.g. camera->SetRotationFromYawPitch(yaw, pitch);
        camera->SetRotation(glm::vec3(pitch, yaw, 0));

        //camera->yaw += mouseDelta.x() * rotationSpeed * dt;
        //camera->pitch += -mouseDelta.y() * rotationSpeed * dt;
        //camera->updateVectors();

    }
}