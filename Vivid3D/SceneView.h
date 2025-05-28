#pragma once

#include <QWidget>
#include "ui_SceneView.h"


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
private:
	RefCntAutoPtr<IRenderDevice>  m_pDevice;
	RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
	RefCntAutoPtr<ISwapChain>     m_pSwapChain;
	RefCntAutoPtr<IShaderSourceInputStreamFactory> m_pShaderFactory;
	Ui::SceneViewClass ui;

	//TEST members
	GraphNode* m_Test1;
	SceneGraph* m_SceneGraph;

};

