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

class CameraController;
class SceneController;
class SceneGraph;
class GraphNode;

#include <QResizeEvent>

class SceneView : public QWidget
{
	Q_OBJECT

public:
	SceneView(QWidget *parent = nullptr);
	~SceneView();
    GraphNode* GetTest() {
        return m_Test1;
    }
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
	
	void focusOutEvent(QFocusEvent* event) override {
		keysHeld.clear(); // clear all keys on focus loss
		rightMousePressed = false;
		QWidget::focusOutEvent(event); // call base implementation
	
	}
	void leaveEvent(QEvent* event) override {
		// Optional: handle any visual/UI logic when mouse leaves
		keysHeld.clear(); // clear all keys on focus loss

		rightMousePressed = false;
		QWidget::leaveEvent(event);

	}
	void mousePressEvent(QMouseEvent* event) override;
	
	void mouseReleaseEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void keyReleaseEvent(QKeyEvent* event) override;

    void handleMovement();
   
private:
	RefCntAutoPtr<IRenderDevice>  m_pDevice;
	RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
	RefCntAutoPtr<ISwapChain>     m_pSwapChain;
	RefCntAutoPtr<IShaderSourceInputStreamFactory> m_pShaderFactory;
	Ui::SceneViewClass ui;
	bool m_Pick = false;

	//TEST members
	GraphNode* m_Test1;
	SceneGraph* m_SceneGraph;
	GraphNode* m_L1;
    int LastTick = 0;
    float m_DeltaTime = 0.0f;
	CameraController* m_CameraController;
	SceneController* m_SceneController; 
	QPoint m_PickPos;
};

