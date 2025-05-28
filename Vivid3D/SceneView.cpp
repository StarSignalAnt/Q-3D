#include "SceneView.h"
#include <QPainter>
#include "Vivid.h"
//Diligent Engine includes


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
#include "SceneGraph.h"
#include "GraphNode.h"
#include "Importer.h"
#include "Texture2D.h"
#include "StaticMeshComponent.h"
#include "MaterialBasic3D.h"

using namespace Diligent;



// End

SceneView::SceneView(QWidget *parent)
	: QWidget(parent)
{
	//ui.setupUi(this);
	setWindowTitle("Scene View");
    CreateGraphics();

    m_SceneGraph = new SceneGraph;

    m_Test1 = Importer::ImportEntity("test/test.fbx");
    m_SceneGraph->SetRootNode(m_Test1);

    auto cam = m_SceneGraph->GetCamera();

    cam->SetPosition(glm::vec3(0, 0, 4));
    auto tex1 = new Texture2D("test/tex1.png");
    
    auto m1 = m_Test1->GetComponent<StaticMeshComponent>();
    
    for (auto& sub : m1->GetSubMeshes()) {

        auto rmat = (MaterialBasic3D*)sub.m_Material;
        rmat->SetColorTexture(tex1);

    }
    m_Test1->SetRotation(glm::vec3(0, 45, 0));

}

SceneView::~SceneView()
{}

void SceneView::paintEvent(QPaintEvent* event)
{
	//QPainter painter(this);
	//painter.fillRect(rect(), QColor("#ADD8E6"));

    auto* pContext = GetContext();
    auto* pSwapchain = GetSwapChain();

    ITextureView* pRTV = pSwapchain->GetCurrentBackBufferRTV();

    pContext->SetRenderTargets(1, &pRTV, pSwapchain->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    const float ClearColor[4] = { 1.0f,0.02f,0.02f,1.0 };
    pContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
    pContext->ClearDepthStencil(pSwapchain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    m_SceneGraph->Render();

    pSwapchain->Present();

}

void SceneView::CreateGraphics() {

	WId wid = winId();
	HWND hwnd = reinterpret_cast<HWND>(wid);


    #if PLATFORM_WIN32  
					Diligent::Win32NativeWindow Window{ hwnd };  
    #endif

	SwapChainDesc SCDesc;

#if ENGINE_DLL
    // Load the dll and import GetEngineFactoryD3D12() function
    auto* GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
#endif
    auto* pFactoryD3D12 = GetEngineFactoryD3D12();

    //SCDesc.ColorBufferFormat = TEX_FORMAT_RGBA16_FLOAT;

   // SCDesc.ColorBufferFormat = TEX_FORMAT_RGB32_FLOAT;
   // SCDesc.DepthBufferFormat = TEX_FORMAT_D32_FLOAT;

    EngineD3D12CreateInfo EngineCI;
    pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &m_pDevice, &m_pImmediateContext);
    pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, Window, &m_pSwapChain);
    pFactoryD3D12->CreateDefaultShaderSourceStreamFactory("engine\\shader\\", &m_pShaderFactory);

	Vivid::m_pImmediateContext = m_pImmediateContext;
	Vivid::m_pDevice = m_pDevice;
	Vivid::m_pShaderFactory = m_pShaderFactory;
	Vivid::m_pSwapChain = m_pSwapChain;


}



void SceneView::resizeEvent(QResizeEvent* event)
{
    // Call the base class implementation (optional, but good practice)
    QWidget::resizeEvent(event);

    // Handle the resize event
    QSize newSize = event->size();
    qreal dpr = this->devicePixelRatioF();

    // Convert logical size to physical size
    int physicalWidth = static_cast<int>(newSize.width() * dpr);
    int physicalHeight = static_cast<int>(newSize.height() * dpr);

    m_pSwapChain->Resize(static_cast<Uint32>(physicalWidth), static_cast<Uint32>(physicalHeight));
	Vivid::SetFrameWidth(physicalWidth);
	Vivid::SetFrameHeight(physicalHeight);



    // m_ppEmissive = new PPEmissive;
   //  m_PP->AddPostProcess(m_ppEmissive);
    // m_ppEmissive->SetGraph(m_Graph1);
    // m_RT2 = new RenderTarget2D(Engine::GetFrameWidth(), Engine::GetFrameHeight());

}
