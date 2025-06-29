#pragma once
#include "GraphNode.h"
#include <QWidget>


class CameraController
{
public:
    CameraController(GraphNode* camera)
        : camera(camera) {
    }

    void update(float deltaTime, const QSet<int>& keysHeld, const QPoint& mouseDelta, bool rmbDown);
    void updateMouse(float deltaTime,const QPoint& mouseDelta, bool rmbDown);
    void SetCamera(GraphNode* cam) {
        camera = cam;
    }
private:
    GraphNode* camera;
    float moveSpeed = 40.0f;
    float quick = 2.5f;
    float rotationSpeed = 0.2f;
    float pitch = 0;
    float yaw = 0;

};

