#pragma once

#include <QWidget>
#include "ui_SceneView.h"
#include "SceneGraph.h"
#include "GraphNode.h"
#include <qtimer.h>

#if D3D11_SUPPORTED
#    include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#endif
#if D3D12_SUPPORTED
#    include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#endif
#if GL_SUPPORTED
#    include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#endif
#if VULKAN_SUPPORTED
#    include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#endif
#if METAL_SUPPORTED
#    include "Graphics/GraphicsEngineMetal/interface/EngineFactoryMtl.h"
#endif

#ifdef GetObject
#    undef GetObject
#endif
#ifdef CreateWindow
#    undef CreateWindow
#endif

#include <optional>
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "BasicMath.hpp"

using namespace Diligent;

class SceneGraph;
class GraphNode;

#include <QResizeEvent>

class SceneView : public QWidget
{
	Q_OBJECT

public:
	SceneView(QWidget *parent = nullptr);
	~SceneView();
protected:
	void CreateGraphics();
	void paintEvent(QPaintEvent* event) override;
	IEngineFactory* GetEngineFactory() { return m_pDevice->GetEngineFactory(); }
	IRenderDevice* GetDevice() { return m_pDevice; }
	IDeviceContext* GetContext() { return m_pImmediateContext; }
	ISwapChain* GetSwapChain() { return m_pSwapChain; }
	void resizeEvent(QResizeEvent* event) override;
    bool rightMousePressed = false;
    QPoint lastMousePos;
    QTimer movementTimer;
    // Movement keys
    QSet<int> keysHeld;
    float yaw = 0.0f, pitch = 0.0f;
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::RightButton) {
            rightMousePressed = true;
            lastMousePos = event->pos();
        }
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::RightButton) {
            rightMousePressed = false;
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (rightMousePressed) {
            QPoint delta = event->pos() - lastMousePos;
            lastMousePos = event->pos();

            // Sensitivity multiplier
            float sensitivity = 0.1f;
            yaw -= delta.x() * sensitivity;
            pitch -= delta.y() * sensitivity;

            // Clamp pitch if needed
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            // Update your camera orientation here using yaw & pitch
            // E.g. camera->SetRotationFromYawPitch(yaw, pitch);
            m_SceneGraph->GetCamera()->SetRotation(glm::vec3(pitch, yaw, 0));
            update(); // trigger repaint
        }
    }
    void keyPressEvent(QKeyEvent* event) override {
        keysHeld.insert(event->key());
    }

    void keyReleaseEvent(QKeyEvent* event) override {
        keysHeld.remove(event->key());
    }

    void handleMovement() {
        QVector3D direction(0.0f, 0.0f, 0.0f);

        // Construct forward vector from yaw/pitch
        QVector3D front;
        //front.setX(cos(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
       // front.setY(sin(qDegreesToRadians(pitch)));
       // front.setZ(sin(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
       // front.normalize();


        QVector3D right = QVector3D::crossProduct(front, QVector3D(0.0f, 1.0f, 0.0f)).normalized();
        QVector3D up = QVector3D::crossProduct(right, front).normalized();

        glm::vec3 move = glm::vec3(0, 0, 0);

        if (keysHeld.contains(Qt::Key_W))
            move += glm::vec3(0, 0, -0.1f);
        if (keysHeld.contains(Qt::Key_S))
            move -= glm::vec3(0, 0, -0.1f);


        if (keysHeld.contains(Qt::Key_A))
            move += glm::vec3(-0.1, 0, 0);
        if (keysHeld.contains(Qt::Key_D))
            move -= glm::vec3(-0.1, 0, 0);
            //    direction += right;

        if (keysHeld.contains(Qt::Key_Space))
        {
            m_L1->SetPosition(m_SceneGraph->GetCamera()->GetPosition());
        }

           // direction.normalize();
            //float speed = 0.1f; // units per frame
            //QVector3D movement = direction * speed;
            m_SceneGraph->GetCamera()->Move(move);

    }
private:
	RefCntAutoPtr<IRenderDevice>  m_pDevice;
	RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
	RefCntAutoPtr<ISwapChain>     m_pSwapChain;
	RefCntAutoPtr<IShaderSourceInputStreamFactory> m_pShaderFactory;
	Ui::SceneViewClass ui;

	//TEST members
	GraphNode* m_Test1;
	SceneGraph* m_SceneGraph;
	GraphNode* m_L1;
};

