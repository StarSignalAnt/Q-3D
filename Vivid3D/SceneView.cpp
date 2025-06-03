#define NOMINMAX

#include "SceneView.h"
#include <QPainter>
#include "Vivid.h"
#include "PropertiesEditor.h"
#include "MaterialPBR.h"
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
#include "TextureCube.h"
#include "StaticMeshComponent.h"
#include "MaterialBasic3D.h"
#include "LightComponent.h"
#include "CameraController.h"
#include "SceneController.h"
#include "SceneGizmo.h"
#include "ScriptHost.h"
#include "ScriptComponent.h"
#include "NodeTree.h"

using namespace Diligent;



// End

SceneView* SceneView::m_Instance = nullptr;

SceneView::SceneView(QWidget *parent)
	: QWidget(parent)
{
	//ui.setupUi(this);
	setWindowTitle("Scene View");
    CreateGraphics();
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    m_Instance = this;
    //Vivid::InitPython();
    
    ScriptHost *h = new ScriptHost;
/*
    auto inst = h->CreateInstance("MyCustomNode");
    //h->TestFunc(inst, "DebugInfo");
    //h->CallFunc(inst, "greet", "Alice", 30);
    Pars p(45, 2, "Antony");
    //p.AddInt(45);
    //p.AddInt(20);
    //p.AddString("Antony");

     h->callFunc(inst, "greet", p);

    //h->CallFunc(inst, std::string("greet"), p);

*/



    m_SceneGraph = new SceneGraph;

    m_Test1 = Importer::ImportEntity("test/test1.gltf");
    auto test2 = Importer::ImportEntity("test/test2.gltf");
    m_SceneGraph->AddNode(m_Test1);
    m_SceneGraph->AddNode(test2);

    auto sc = new ScriptComponent;
    test2->AddComponent(sc);
    sc->SetScript("scripts/test.py", "TestComp");

   

    auto act1 = Importer::ImportSkeletal("test/actor1.fbx");

 //   m_SceneGraph->AddNode(act1);
    act1->SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
 //   m_Test1->SetRotation(glm::vec3(-90, 0, 0));

    //for (auto& e : m_Test1->GetNodes()) {

    auto e = m_Test1;

    


		auto smc = e->GetComponent<StaticMeshComponent>();
        for (auto m : smc->GetSubMeshes()) {

			auto m1 = (MaterialPBR*)m->m_Material;
			m1->SetColorTexture(new Texture2D("test/tex_color.png"));
			m1->SetNormalTexture(new Texture2D("test/tex_normal.png"));
            m1->SetMetallicTexture(new Texture2D("test/tex_metal.png"));
			m1->SetRoughnessTexture(new Texture2D("test/tex_rough.png"));
		//	m1->SetHeightTexture(new Texture2D("test/tex_height.png"));


			//m1->SetIRR(new TextureCube("test/cube1.tex"));
            //m1->SetEnvironmentMap(new TextureCube("test/cube1.tex"));




        }

    

    auto cam = m_SceneGraph->GetCamera();
   
    cam->SetPosition(glm::vec3(0, 4, 4));
    auto tex1 = new Texture2D("test/tex1.png");
    
//    auto m1 = m_Test1->GetNodes()[0]->GetComponent<StaticMeshComponent>();
    m_Test1->SetScale(glm::vec3(1, 1, 1));
//    m_Test1->GetNodes()[0]->SetScale(glm::vec3(1, 1, 1));
    
 //   for (auto& sub : m1->GetSubMeshes()) {

     //   auto rmat = (MaterialBasic3D*)sub.m_Material;
   //     rmat->SetColorTexture(tex1);

   // }

    //m_Test1->SetRotation(glm::vec3(45, 45, 0));

    auto l1 = new GraphNode;

    auto lc = new LightComponent;

	l1->AddComponent(lc);

    m_SceneGraph->AddLight(l1);

    auto l2 = new GraphNode;
    l2->AddComponent(new LightComponent);

    l2->GetComponent<LightComponent>()->SetColor(glm::vec3(0,3,3));

    m_SceneGraph->AddLight(l2);
    l2->SetPosition(glm::vec3(0, 5, 6));


	l1->SetPosition(glm::vec3(0, 4, 4));

    movementTimer.setInterval(16); // ~60 FPS
    connect(&movementTimer, &QTimer::timeout, this, &SceneView::handleMovement);
    movementTimer.start();

    m_L1 = l2;
    m_CameraController = new CameraController(cam);
    m_SceneController = new SceneController;
    m_SceneController->setCamera(cam);
    m_SceneController->setScene(m_SceneGraph);
    m_SceneController->Init();
    NodeTree::m_Instance->SetRoot(m_SceneGraph->GetRootNode());
}

SceneView::~SceneView()
{}

float yv = 0;

void SceneView::SetMode(SceneMode mode) {
    m_Mode = mode;
    switch (mode) {
    case Mode_Translate:
        m_SceneController->SetMode(GizmoMode::Gizmo_Translate);
        break;
    case Mode_Rotate:
        m_SceneController->SetMode(GizmoMode::Gizmo_Rotate);
        break;
    case Mode_Scale:
        m_SceneController->SetMode(GizmoMode::Gizmo_Scale);
        break;
    }
}

void SceneView::SetSpace(SceneSpace space) {

    m_Space = space;
    switch (space) {
    case Space_Local:
        m_SceneController->SetSpace(EditSpace::Edit_Local);
        break;
    case Space_World:
        m_SceneController->SetSpace(EditSpace::Edit_World);
        break;
    case Space_Smart:

        break;
    }

}

void SceneView::Run() {

    if (m_RunMode == RM_Running) return;
    m_SceneGraph->Push();
    m_RunMode = RM_Running;
    keysHeld.clear();



}

void SceneView::Stop() {

    if (m_RunMode == RM_Stopped) return;
    m_RunMode = RM_Stopped;
    m_SceneGraph->Pop();
    if (m_SelectedNode != nullptr) {
        PropertiesEditor::m_Instance->SetNode(m_SelectedNode);
    }
}

void SceneView::AlignGizmo() {

    m_SceneController->AlignGizmos();

}

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

    int tick = clock();
    int ttick = tick - LastTick;
    LastTick = tick;

    float tt = ( ((float)ttick) / 1000.0f);

    if (m_RunMode == RM_Running) {
        m_SceneGraph->Update(tt);
    }
    
    m_SceneGraph->RenderShadows();
    m_SceneGraph->Render();


    if (m_RunMode == RM_Stopped) {
        m_SceneController->Render();
    }

    pSwapchain->Present();


    if (m_RunMode == RM_Stopped) {
        if (m_Pick) {
            m_Pick = false;
            QPoint localPos = m_PickPos;
            qreal dpr = this->devicePixelRatioF(); // or use QWidget::window()->devicePixelRatioF()

            int x = static_cast<int>(localPos.x() * dpr);
            int y = static_cast<int>(localPos.y() * dpr);
            m_SceneController->onMouseClick(glm::vec2(x, y));
            auto picked = m_SceneController->GetSelected();
            if (picked != nullptr) {
                if (m_SelectedNode != picked) {
                    m_SelectedNode = picked;

                    PropertiesEditor::m_Instance->SetNode(m_SelectedNode);
                    printf("Set Property Node\n");
                }
            }

        }
    }

    //m_Test1->SetRotation(glm::vec3(45, yv, 0));
   // yv = yv + 1;
    update();


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
    SCDesc.DepthBufferFormat = TEX_FORMAT_D32_FLOAT;

    EngineD3D12CreateInfo EngineCI;

    EngineCI.SetValidationLevel(VALIDATION_LEVEL_DISABLED);
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




}


void SceneView::mousePressEvent(QMouseEvent* event)  {
    if (event->button() == Qt::RightButton) {
        rightMousePressed = true;
        lastMousePos = event->pos();
    }
    if (event->button() == Qt::LeftButton) {

  
        m_PickPos = event->pos();
        m_Pick = true;
        return;
       


    }
}

void SceneView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        rightMousePressed = false;
    }
    else {
        m_SceneController->onMouseUp();
    }
}

void SceneView::mouseMoveEvent(QMouseEvent* event)  {
    if (m_RunMode == RM_Running) return;
    if (rightMousePressed) {
        QPoint delta = event->pos() - lastMousePos;
        m_CameraController->updateMouse(1.0f / 60.0f, delta, true);
        lastMousePos = event->pos();
    }
    else {
        QPoint delta = event->pos() - lastMousePos;
        m_SceneController->onMouseMove(glm::vec2(delta.x(), delta.y()));
        if (m_SelectedNode != nullptr) {
            PropertiesEditor::m_Instance->UpdateNode(m_SelectedNode);
        }
        lastMousePos = event->pos();
   
    }
   
}
void SceneView::keyPressEvent(QKeyEvent* event) {
    if (m_RunMode == RM_Stopped) {
        keysHeld.insert(event->key());
    }
    switch (event->key()) {
    case Qt::Key::Key_W:
        GameInput::m_Key[Key::Key_W] = true;
        break;
    case Qt::Key::Key_A:
        GameInput::m_Key[Key::Key_A] = true;
        break;
    case Qt::Key::Key_S:
        GameInput::m_Key[Key::Key_S] = true;
        break;
    case Qt::Key::Key_D:
        GameInput::m_Key[Key::Key_D] = true;
        break;
    case Qt::Key::Key_Space:
        GameInput::m_Key[Key::Key_Space] = true;
        break;
    }
}

void SceneView::keyReleaseEvent(QKeyEvent* event) {
    if (m_RunMode == RM_Stopped) {
        keysHeld.remove(event->key());
    }
    switch (event->key()) {
    case Qt::Key::Key_W:
        GameInput::m_Key[Key::Key_W] = false;
        break;
    case Qt::Key::Key_A:
        GameInput::m_Key[Key::Key_A] = false;
        break;
    case Qt::Key::Key_S:
        GameInput::m_Key[Key::Key_S] = false;
        break;
    case Qt::Key::Key_D:
        GameInput::m_Key[Key::Key_D] = false;
        break;
    case Qt::Key::Key_Space:
        GameInput::m_Key[Key::Key_Space] = false;
        break;
    }
}

void SceneView::handleMovement() {
    m_CameraController->update(1.0f / 60.0f, keysHeld, QPoint(),rightMousePressed);
 

}

void SceneView::SelectNode(GraphNode* node)
{

    if (node == nullptr) {
        return;
    }
    m_SelectedNode = node;
    m_SceneController->SelectNode(node);
    PropertiesEditor::m_Instance->SetNode(node);

}