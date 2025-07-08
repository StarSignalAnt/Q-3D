

#include "SceneView.h"
#include <QPainter>
#include "QEngine.h"
#include "NodeTree.h"
#include "PropertiesEditor.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "MaterialPBR.h"
//Diligent Engine includes
#include "Physics.h"
#include "TerrainMeshComponent.h"
#include "TerrainRendererComponent.h"
#include "StaticRendererComponent.h"
#include "TerrainLayer.h"
#include "PixelMap.h"
#include "TerrainEditor.h"
#include "ConsoleOutput.h"
#include "GameVideo.h"
#include "GameFont.h"
#include "CameraComponent.h"
#include "OctreeScene.h"
#include "RenderTarget2D.h"
#include "MaterialProducer.h"

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
#include "TerrainDepthRenderer.h"
#include "SharpComponent.h"
#include "LogicGraphComponent.h"
#include "SkyComponent.h"
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
    
    setAcceptDrops(true);
    m_SceneGraph = new SceneGraph;
    QEngine::InitEngine();

    auto sky = new GraphNode;
    sky->SetName("Atmosphere");
    sky->AddComponent(new SkyComponent());

    m_SceneGraph->AddNode(sky);

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



  

    m_Grid = new SceneGrid(m_SceneGraph);



    //m_Test1 = Importer::ImportEntity("test/test1.gltf");
    auto test2 = Importer::ImportEntity("test/test3.fbx",false);
    auto sp1 = Importer::ImportEntity("test/sphere1.fbx",true);

  test2->SetRenderType(NodeRenderType::RenderType_Static);


 
   // m_SceneGraph->AddNode(m_Test1);
   m_SceneGraph->AddNode(test2);
    m_SceneGraph->AddNode(sp1);



    sp1->SetPosition(glm::vec3(0, 5,0));


    //auto sc = new ScriptComponent;
    //test2->AddComponent(sc);
    //sc->SetScript("scripts/test.py", "TestComp");

   


    auto cam = m_SceneGraph->GetCamera();
   
    cam->SetPosition(glm::vec3(0, 4, 4));
   
    auto cc = cam->GetComponent<CameraComponent>();
    cc->SetExtents(0.1f, 2000.0f);


    //auto tex1 = new Texture2D("test/tex1.png");
    
//    auto m1 = m_Test1->GetNodes()[0]->GetComponent<StaticMeshComponent>();

//    m_Test1->GetNodes()[0]->SetScale(glm::vec3(1, 1, 1));
    
 //   for (auto& sub : m1->GetSubMeshes()) {

     //   auto rmat = (MaterialBasic3D*)sub.m_Material;
   //     rmat->SetColorTexture(tex1);

   // }

    //m_Test1->SetRotation(glm::vec3(45, 45, 0));

  

    auto l2 = new GraphNode;
    l2->AddComponent(new LightComponent);

    l2->GetComponent<LightComponent>()->SetColor(glm::vec3(0,3,3));
    l2->GetComponent<LightComponent>()->SetRange(220);
    l2->GetComponent<LightComponent>()->SetIntensity(1700);
    l2->GetComponent<LightComponent>()->SetLightType(LightType::Point);
    m_SceneGraph->AddLight(l2);




    l2->SetPosition(glm::vec3(0, 15, 0));
   

//	l1->SetPosition(glm::vec3(15, 25, 0));

    movementTimer.setInterval(16); // ~60 FPS
    connect(&movementTimer, &QTimer::timeout, this, &SceneView::handleMovement);
    movementTimer.start();
    //m_SceneGraph->InitializeOctree();
    //m_SceneGraph->ExportOctree("oc/test1");
   // m_SceneGraph->ImportOctree("oc/test1");









   // auto c2 = new GraphNode;
   // c2->AddComponent(new CameraComponent());
   // m_SceneGraph->GetOctree()->SetViewCam(c2);
    //c2->GetComponent<CameraComponent>()->SetExtents(0.1f, 1000.0f);
    //m_SceneGraph->SetCamera(c2);
    //m_ViewCam = c2;

   // c2->SetPosition(cam->GetPosition());
   // c2->SetRotation(cam->GetRotation());

    

    m_L1 = l2;
    m_CameraController = new CameraController(cam);
    m_SceneController = new SceneController;
    m_SceneController->setCamera(cam);
    m_SceneController->setScene(m_SceneGraph);
    m_SceneController->Init();
 
    m_Draw = new Draw2D(cam);

    //m_Test1->SetBody(BodyType::T_TriMesh);
   // m_Test1->CreateBody();




    //m_Test1->CreateRB();
 

    NodeTree::m_Instance->SetRoot(m_SceneGraph->GetRootNode());

    QEngine::InitMono();

 //   auto cm1 = new SharpComponent;
//	test2->AddComponent(cm1);
//    cm1->SetScript("TestLib.dll", "Test");

    //m_Vid1 = new GameVideo("test/video1.mp4");



   



    //m_Vid1->Play();

    m_timer = new QTimer(this);

    // 2. Connect the timer's timeout() signal to our desired function (onTimeout slot)
    //    This is the core of the mechanism.
    connect(m_timer, &QTimer::timeout, this, &QEngine::CheckDLL);

    QEngine::CheckDLL();
    // 3. Start the timer and set the interval to 0.5 seconds (500 milliseconds)
    m_timer->start(500);

    m_Font = new GameFont("Engine/system4.ttf", 20, m_SceneGraph->GetCamera());
   
    //exit(1);
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
    m_SceneGraph->Play();

    m_RunMode = RM_Running;
    keysHeld.clear();



}

void SceneView::Stop() {

    if (m_RunMode == RM_Stopped) return;
    m_RunMode = RM_Stopped;
    m_SceneGraph->Pop();
    m_SceneGraph->Stop();
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


    int fs = clock();

   // m_SceneGraph->GetOctree()->FinalizeStreamedNodes();
    fs = clock() - fs;
    if (fs > 0) {
        //QEngine::DebugLog("Stream MS:" + std::to_string(fs));

    }

    int start = clock();

    if (start > (m_LastUpdate+8)) {
        m_LastUpdate = start;
    }
    else {
        return;
    }


    auto* pContext = GetContext();
    auto* pSwapchain = GetSwapChain();

    ITextureView* pRTV = pSwapchain->GetCurrentBackBufferRTV();

    pContext->SetRenderTargets(1, &pRTV, pSwapchain->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    const float ClearColor[4] = { 0.02f,0.02f,0.02f,1.0 };
    pContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
    pContext->ClearDepthStencil(pSwapchain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    int tick = clock();
    int ttick = tick - LastTick;
    LastTick = tick;

    float tt = ( ((float)ttick) / 1000.0f);

    if (m_RunMode == RM_Running) {
        QEngine::m_Physics->Update(0.01);

    }
    
    m_SceneGraph->Update(tt);


   // m_SceneGraph->Update(tt);
    start = clock();
    m_SceneGraph->RenderShadows();
    int end = clock() - start;
  //  QEngine::DebugLog("MS1:" + std::to_string(end));
    start = clock();
    m_SceneGraph->Render();
    end = clock() - start;
//    QEngine::DebugLog("MS2:" + std::to_string(end));


    if (m_RunMode == RM_Stopped) {
        m_SceneController->Render();
    }

    //Vivid::ClearZ();
 
  //  QEngine::m_pImmediateContext->Flush();
 
    if (m_RunMode == RM_Stopped) {
        m_SelectionOverlay->Render();
    }

//    QEngine::ClearZ();

    if (m_BrushNode) {

        m_BrushNode->Render(m_SceneGraph->GetCamera());

    }

  
    for (int i = 0; i < 5; i++) {
    //    m_Vid1->Update();
    }




    //m_Font->DrawTextAsTexture("This is a simple test", glm::vec2(20, 150), 1.0f, glm::vec4(1, 1, 1, 1));

    QEngine::ClearZ();
    QEngine::SetScissor(0, 0, QEngine::GetFrameWidth(), QEngine::GetFrameHeight());
    m_Draw->BeginFrame();
  //  m_Draw->Rect(m_SceneGraph->GetLights()[0]->GetComponent<LightComponent>()->GetDirectionalShadowMap()->GetDepthTexture2D(), glm::vec2(20, 20), glm::vec2(320, 320), glm::vec4(1, 1, 1, 1));
    m_Draw->Flush();

//    auto tex = m_Vid1->GetFrame();

   // if (tex) {
  //      m_Draw->Rect(tex, glm::vec2(0, 0), glm::vec2(512, 512), glm::vec4(1, 1, 1, 1));
    //}

    pSwapchain->Present();



    //std::cout <<

   //     QEngine::DebugLog("RenderTime:" + std::to_string(end) + "ms\n");

    if (m_RunMode == RM_Stopped) {
        if (m_Mode == SceneMode::Mode_Translate || m_Mode == SceneMode::Mode_Rotate || m_Mode == SceneMode::Mode_Scale) {
            //    m_SceneController->RenderGizmos();

            if (m_Pick) {
                m_Pick = false;
                QPoint localPos = m_PickPos;
                qreal dpr = this->devicePixelRatioF(); // or use QWidget::window()->devicePixelRatioF()

                int x = static_cast<int>(localPos.x() * dpr);
                int y = static_cast<int>(localPos.y() * dpr);
                m_SceneController->onMouseClick(glm::vec2(x, y));
                //  SelectNode(m_Terrain);
                auto picked = m_SceneController->GetSelected();
                if (picked != nullptr) {
                    if (m_SelectedNode != picked) {
                        m_SelectedNode = picked;
                        SelectNode(m_SelectedNode);
                        //PropertiesEditor::m_Instance->SetNode(m_SelectedNode);
                        printf("Set Property Node\n");
                    }
                }

            }
        }
    }

    //m_Test1->SetRotation(glm::vec3(45, yv, 0));
   // yv = yv + 1;
    //update();


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
    //SCDesc.ColorBufferFormat = TEX_FORMAT_RGBA8_UNORM_SRGB;




    EngineD3D12CreateInfo EngineCI;

   

    EngineCI.SetValidationLevel(VALIDATION_LEVEL_DISABLED);
    pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &m_pDevice, &m_pImmediateContext);
    pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, Window, &m_pSwapChain);
    pFactoryD3D12->CreateDefaultShaderSourceStreamFactory("engine\\shader\\", &m_pShaderFactory);

	QEngine::m_pImmediateContext = m_pImmediateContext;
	QEngine::m_pDevice = m_pDevice;
	QEngine::m_pShaderFactory = m_pShaderFactory;
	QEngine::m_pSwapChain = m_pSwapChain;

    MaterialProducer* pro = new MaterialProducer;

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
	QEngine::SetFrameWidth(physicalWidth);
	QEngine::SetFrameHeight(physicalHeight);


    m_SelectionOverlay = new SceneSelectionOverlay(m_SceneGraph);

}


void SceneView::mousePressEvent(QMouseEvent* event)  {
    if (event->button() == Qt::RightButton) {
        rightMousePressed = true;
        lastMousePos = event->pos();
        GameInput::m_Button[1] = true;
      
    }
    if (event->button() == Qt::LeftButton) {

        GameInput::m_Button[0] = true;
        if (m_Mode == SceneMode::Mode_Paint)
        {
            //m_TerrainEditing = true;
            if (m_Terrain) {
                m_TerrainEditor->BeginPaint();
                m_TerrainEditor->Update();
            }
        }
        else if (m_Mode == SceneMode::Mode_Sculpt) {

            if (m_Terrain) {
                m_TerrainEditor->BeginSculpt();
                m_TerrainEditor->Update();

            }
        }else{
 
            m_PickPos = event->pos();
            m_Pick = true;
       }



        return;
       



    }
}

void SceneView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        rightMousePressed = false;
        GameInput::m_Button[1] = false;
    }
    else {
        GameInput::m_Button[0] = false;
        m_SceneController->onMouseUp();
        //m_
        //TerrainEditing = false;
        if (m_Terrain) {
            if (m_Mode == SceneMode::Mode_Paint)
            {
                //m_TerrainEditing = true;
                m_TerrainEditor->EndPaint();
            }
            else if (m_Mode == SceneMode::Mode_Sculpt) {
                m_TerrainEditor->EndSculpt();
            }

        }
    }

}
constexpr double MPI = 3.14159265358979323846;
static float Deg2Rad(float deg) {
    return deg * (float)MPI / 180.0;
}
static float Rad2Deg(float r)
{
    float degrees = r * (180.0f / static_cast<float>(M_PI));
    // Normalize degrees to be within 0 to 360

    return degrees;
}

GraphNode* CreateTerrainBrush(float mx, float h, float my, float size, float strength) {

    /*
    h = 0.0001;


    //var ent = new Entity();
    GraphNode* ent = new GraphNode;


    // if (BMESH == null)
   //  {
         //   Vivid.Meshes.Mesh bmesh = new Mesh(ent);
      //   BMESH = new Mesh(ent);
 //    }


    StaticMeshComponent* sm = new StaticMeshComponent;

    ent->AddComponent(sm);
    ent->AddComponent(new StaticRendererComponent);
    SubMesh* msh = new SubMesh;
    sm->AddSubMesh(msh);
    //auto tex = ((MaterialPBR*)sm->GetSubMeshes()[0]->m_Material)->GetColorTexture();
    sm->GetSubMeshes()[0]->m_Material = new MaterialBasic3D;

    auto b3 = (MaterialBasic3D*)sm->GetSubMeshes()[0]->m_Material;
    b3->SetColorTexture(SceneView::m_Instance->GetWhite());
    b3->SetTexture(SceneView::m_Instance->GetWhite(), 0);



    //Mesh3D* bmesh = new Mesh3D;
    //bmesh->SetOwner(ent);


    //Vivid.Meshes.Mesh bmesh = BMESH;
    //BMESH.Owner = ent;


    ent->SetPosition(glm::vec3(mx, h, my));


    //ent.Position = new System.Numerics.Vector3(mx, 0, my);


    Vertex3 cv;
    cv.color = glm::vec4(1, 1, 1, 0.85f * strength);
    cv.uv = glm::vec3(0, 0, 0);
    cv.position = glm::vec3(0, h, 0);
    
  
    
    msh->m_Vertices.push_back(cv);
    bool first = true;  
    int vi = 1;

    //bmesh->GetVertices.Clear();
    for (int ang = 0; ang <= 360; ang += 20)
    {

        float ax = cos(Deg2Rad(ang)) * size;
        float ay = sin(Deg2Rad(ang)) * size;


        //Ray ray = new Ray();
       // ray.Pos = new System.Numerics.Vector3(mx + ax, 30.0f, my + ay);
       // ray.Dir = new System.Numerics.Vector3(0, -35, 0);

        //var res = Vivid.Scene.Scene.CurrentScene.Raycast(ray);

        auto pos = glm::vec3(mx + ax, 30.0f, my + ay);
        auto dir = glm::vec3(0, -35, 0);

        // auto res = Editor::m_Graph->RayCast(pos, dir);


        float hy = 0;

        //if (res.m_Hit)
        {

            //     int b = 5;
          //  hy = res.m_Point.y;

           // h = hy + 0.1f;
            //    Random r = new Random(Environment.TickCount);
                // Console.WriteLine("Hitting Terrain:"+r.Next(20,2000));
                 //hy = hy + 0.225f;




        }
        //else
        {
            //   h = -5.0f;
        }

        //                hy = 0;

                      //  float y1 = GetHeight(ax, ay);

        Vertex3 v1, v2, v3;

        // v1 =new Vertex();

        v1.color = glm::vec4(1, 1, 1, 0);

        v1.uv = glm::vec3(0, 1, 0);

        v1.position = glm::vec3(ax, h, ay);

        msh->m_Vertices.push_back(v1);



        if (!first)
        {

            Tri3 nt;// = new Triangle();
            nt.v0 = vi - 1;
            nt.v2 = vi;
            nt.v1 = 0;
            msh->m_Triangles.push_back(nt);

        }
        first = false;
        vi += 1;

        //bmesh.AddTriangle(nt);

    }
    //ent.Meshes.Add(bmesh);
    //ent->AddMesh(bmesh);
    
    sm->Finalize();

    //bmesh.CreateBuffers()
    //bmesh->Build();
    //if (BM == null)
   // {
    //    BM = new Vivid.Materials.MaterialBasic(false, false, true);
     //   CM = new Vivid.Texture.Texture2D("engine\\terrain\\brush1.png");
        //   bmesh.BasicMaterial = new Vivid.Materials.MaterialBasic(false, false, true);
      //     bmesh.LightMaterial.ColorMap = CM;// new Vivid.Texture.Texture2D("engine\\terrain\\brush1.png");

    //}
   // else
    {


    }
    //bmesh.BasicMaterial = BM;
    //bmesh.LightMaterial.ColorMap = CM;



    return ent;
    */
return nullptr;
}

void SceneView::mouseMoveEvent(QMouseEvent* event)  {
 

    GameInput::MousePosition = glm::vec2(event->pos().x(), event->pos().y());
    if (m_RunMode == RM_Running) return;
    if (rightMousePressed) {
        QPoint delta = event->pos() - lastMousePos;
        bool up = m_CameraController->updateMouse(1.0f / 60.0f, delta, true);
        if (up) {
            update();
        }

        lastMousePos = event->pos();
    }
    else {

        QPoint localPos = event->pos();
        qreal dpr = this->devicePixelRatioF(); // or use QWidget::window()->devicePixelRatioF()

        int x = static_cast<int>(localPos.x() * dpr);
        int y = static_cast<int>(localPos.y() * dpr);
        //int x = localPos.x();
        //int y = localPos.y();

      //  std::cout << "MX:" << x << " MY:" << y << std::endl;
        if (m_Terrain) {
            if (m_Mode == SceneMode::Mode_Paint || m_Mode == SceneMode::Mode_Sculpt) {



                auto res = m_SceneGraph->MousePickTerrain(x, y, m_Terrain->GetComponent<TerrainMeshComponent>());


                if (res.m_Hit) {

                    //       std::cout << "THit: X:" << res.m_Point.x << " Y:" << res.m_Point.y << " Z:" << res.m_Point.z << std::endl;
                    m_TerrainX = res.m_Point.x;
                    m_TerrainZ = res.m_Point.z;
                    m_TerrainEditor->SetPosition(res.m_Point);


                    //m_BrushNode = CreateTerrainBrush(res.m_Point.x, 0, res.m_Point.z, m_TerrainBrushSize, m_TerrainStrength);


                }
                m_TerrainEditor->Update();

                //if (m_TerrainEditing) {
                switch (m_Mode) {
                case SceneMode::Mode_Paint:



                    break;
                }

                //}
            }
        }

        QPoint delta = event->pos() - lastMousePos;
        m_SceneController->onMouseMove(glm::vec2(delta.x(), delta.y()));
        if (m_SelectedNode != nullptr) {
            PropertiesEditor::m_Instance->UpdateNode(m_SelectedNode);
        }
      

        lastMousePos = event->pos();

   
    }

}


int mapQtKeyToGameInput(int qtKey)
{
    switch (qtKey)
    {
        // Standard (unshifted) characters
    case Qt::Key_Space:         return KEY_SPACE;
    case Qt::Key_Apostrophe:    return KEY_APOSTROPHE;
    case Qt::Key_Comma:         return KEY_COMMA;
    case Qt::Key_Minus:         return KEY_MINUS;
    case Qt::Key_Period:        return KEY_PERIOD;
    case Qt::Key_Slash:         return KEY_SLASH;
    case Qt::Key_0:             return KEY_0;
    case Qt::Key_1:             return KEY_1;
    case Qt::Key_2:             return KEY_2;
    case Qt::Key_3:             return KEY_3;
    case Qt::Key_4:             return KEY_4;
    case Qt::Key_5:             return KEY_5;
    case Qt::Key_6:             return KEY_6;
    case Qt::Key_7:             return KEY_7;
    case Qt::Key_8:             return KEY_8;
    case Qt::Key_9:             return KEY_9;
    case Qt::Key_Semicolon:     return KEY_SEMICOLON;
    case Qt::Key_Equal:         return KEY_EQUAL;
    case Qt::Key_A:             return KEY_A; case Qt::Key_B: return KEY_B;
    case Qt::Key_C:             return KEY_C; case Qt::Key_D: return KEY_D;
    case Qt::Key_E:             return KEY_E; case Qt::Key_F: return KEY_F;
    case Qt::Key_G:             return KEY_G; case Qt::Key_H: return KEY_H;
    case Qt::Key_I:             return KEY_I; case Qt::Key_J: return KEY_J;
    case Qt::Key_K:             return KEY_K; case Qt::Key_L: return KEY_L;
    case Qt::Key_M:             return KEY_M; case Qt::Key_N: return KEY_N;
    case Qt::Key_O:             return KEY_O; case Qt::Key_P: return KEY_P;
    case Qt::Key_Q:             return KEY_Q; case Qt::Key_R: return KEY_R;
    case Qt::Key_S:             return KEY_S; case Qt::Key_T: return KEY_T;
    case Qt::Key_U:             return KEY_U; case Qt::Key_V: return KEY_V;
    case Qt::Key_W:             return KEY_W; case Qt::Key_X: return KEY_X;
    case Qt::Key_Y:             return KEY_Y; case Qt::Key_Z: return KEY_Z;
    case Qt::Key_BracketLeft:   return KEY_LBRACKET;
    case Qt::Key_Backslash:     return KEY_BACKSLASH;
    case Qt::Key_BracketRight:  return KEY_RBRACKET;
    case Qt::Key_QuoteLeft:     return KEY_GRAVE;

        // Qt's special keys for shifted symbols
    case Qt::Key_Exclam:        return KEY_EXCLAM;
    case Qt::Key_At:            return KEY_AT;
    case Qt::Key_NumberSign:    return KEY_HASH;
    case Qt::Key_Dollar:        return KEY_DOLLAR;
    case Qt::Key_Percent:       return KEY_PERCENT;
    case Qt::Key_AsciiCircum:   return KEY_CARET;
    case Qt::Key_Ampersand:     return KEY_AMPERSAND;
    case Qt::Key_Asterisk:      return KEY_ASTERISK;
    case Qt::Key_ParenLeft:     return KEY_LPAREN;
    case Qt::Key_ParenRight:    return KEY_RPAREN;
    case Qt::Key_Underscore:    return KEY_UNDERSCORE;
    case Qt::Key_Plus:          return KEY_PLUS;
    case Qt::Key_BraceLeft:     return KEY_LCURLY;
    case Qt::Key_BraceRight:    return KEY_RCURLY;
    case Qt::Key_Bar:           return KEY_PIPE;
    case Qt::Key_Colon:         return KEY_COLON;
    case Qt::Key_QuoteDbl:      return KEY_DQUOTE;
    case Qt::Key_Less:          return KEY_LANGLE;
    case Qt::Key_Greater:       return KEY_RANGLE;
    case Qt::Key_Question:      return KEY_QUESTION;
    case Qt::Key_AsciiTilde:    return KEY_TILDE;

        // Non-printable special keys
    case Qt::Key_Shift:         return KEY_SHIFT;
    case Qt::Key_Control:       return KEY_CTRL;
    case Qt::Key_Alt:           return KEY_ALT;
    case Qt::Key_Up:            return KEY_UP;
    case Qt::Key_Down:          return KEY_DOWN;
    case Qt::Key_Left:          return KEY_LEFT;
    case Qt::Key_Right:         return KEY_RIGHT;
    case Qt::Key_Escape:        return KEY_ESC;
    case Qt::Key_Backspace:     return KEY_BACKSPACE;
    case Qt::Key_Delete:        return KEY_DELETE;
    case Qt::Key_Enter:         return KEY_ENTER;
    case Qt::Key_Return:        return KEY_ENTER;
    case Qt::Key_Tab:           return KEY_TAB;

    default:
        // For keys not explicitly mapped, return -1 to ignore them.
        return -1;
    }
}


void SceneView::keyPressEvent(QKeyEvent* event) {
    if (m_RunMode == RM_Stopped) {
        keysHeld.insert(event->key());
    }
    int gameKey = mapQtKeyToGameInput(event->key());

    if (gameKey != -1) {
        GameInput::m_Key[gameKey] = true;
    }
    if (event->key() == Qt::Key::Key_Space) {

        
    }
}

void SceneView::keyReleaseEvent(QKeyEvent* event) {
    if (m_RunMode == RM_Stopped) {
        keysHeld.remove(event->key());
    }
    int gameKey = mapQtKeyToGameInput(event->key());

    if (gameKey != -1) {
        GameInput::m_Key[gameKey] = false;
    }
}

void SceneView::handleMovement() {
    bool up = m_CameraController->update(1.0f / 60.0f, keysHeld, QPoint(),rightMousePressed);
    auto gizNode = m_SceneController->GetGizNode();


    if (gizNode != nullptr) {
        float dist = glm::distance(gizNode->GetPosition(), m_SceneGraph->GetCamera()->GetPosition());

        if (dist < 1.0) {
            dist = -(1.0 - dist) * 12.0;

        }

        auto scale = 1.0 + dist * 0.1;
     
        gizNode->SetScale(glm::vec3(scale, scale, scale));
    }

    if (up) {
        update();
    }

}

void SceneView::SelectNode(GraphNode* node)
{
    m_SelectionOverlay->SelectNode(node);
    if (node == nullptr) {
        
        PropertiesEditor::m_Instance->SetNode(nullptr);
        return;
    }
    m_SelectedNode = node;
    m_SceneController->SelectNode(node);

    if (node->GetComponent<TerrainMeshComponent>()) {
        PropertiesEditor::m_Instance->SetTerrain(node);
        return; 
    }
    else {
        if(m_Mode == SceneMode::Mode_Paint || m_Mode == SceneMode::Mode_Sculpt) {
			m_Mode = SceneMode::Mode_Translate;
            // m_TerrainEditor->SetNode(node);
        }
        else {
         //   m_TerrainEditor->SetNode(nullptr);
		}
    }

    PropertiesEditor::m_Instance->SetNode(node);

}

void SceneView::dragEnterEvent(QDragEnterEvent* event)
{
    // Check if the dragged data contains a file path with a valid extension.
    if (event->mimeData()->hasText())
    {
        QString path = event->mimeData()->text();
        if (path.endsWith(".fbx", Qt::CaseInsensitive) || path.endsWith(".gltf", Qt::CaseInsensitive))
        {
            // If the file type is valid, accept the drag to allow a drop.
            event->acceptProposedAction();
            return;
        }
    }
    // Ignore all other types of drags.
    event->ignore();
}

void SceneView::dropEvent(QDropEvent* event)
{
    const QMimeData* mime = event->mimeData();

    // Final check to ensure the data is a valid file path.
    if (mime->hasText())
    {
        QString fullPath = mime->text();
        if (fullPath.endsWith(".fbx", Qt::CaseInsensitive) || fullPath.endsWith(".gltf", Qt::CaseInsensitive))
        {
            // 1. Import the entity using the file path.
            GraphNode* importedNode = Importer::ImportEntity(fullPath.toStdString());

            if (importedNode)
            {
                // 2. Add the newly created node to the scene graph.
                m_SceneGraph->AddNode(importedNode);
                importedNode->SetScale(glm::vec3(1, 1, 1));

                // 3. Update the NodeTree to show the new node.
                // Calling SetRoot will rebuild the entire tree from the scene graph's root.
                if (NodeTree::m_Instance)
                {
                    NodeTree::m_Instance->SetRoot(m_SceneGraph->GetRootNode());
                }
            }
            event->acceptProposedAction();
        }
        else
        {
            event->ignore();
        }
    }
    else
    {
        event->ignore();
    }
}

void SceneView::SetScene(SceneGraph* graph)
{

    m_SceneGraph->SetRootNode(graph->GetRootNode());
    m_Grid = new SceneGrid(m_SceneGraph);
    //m_SceneGraph->SetCamera(graph->GetCamera());

    NodeTree::m_Instance->SetRoot(m_SceneGraph->GetRootNode());

}

void SceneView::Update() {

    NodeTree::m_Instance->SetRoot(m_SceneGraph->GetRootNode());
    m_Grid = new SceneGrid(m_SceneGraph);
}

void SceneView::TerrainPlot() {
    std::cout << "!!!!!!!!!!!!!!!!!!!!! PLoting" << std::endl;
    int b = 5;
    //var ter = Edit.EditorGlobal.CurrentTerrain;
  
}

void SceneView::SetEditLayer(int layer) {

    if (m_Terrain == nullptr) return;
    m_TerrainEditor->SetEditLayer(layer);

}

void SceneView::CreateTerrain() {

    m_Terrain = new GraphNode;

    m_Terrain->SetName("TerrainNode");

    auto ter = new TerrainMeshComponent(32, 32, 4, 3);
    auto ter_ren = new TerrainRendererComponent;
    m_Terrain->AddComponent(ter);
    m_Terrain->AddComponent(ter_ren);
    m_Terrain->AddComponent(new TerrainDepthRenderer);

    // m_SceneGraph->SetTerrain(m_Terrain);
    m_White = new Texture2D("engine/white.png");

     m_TerrainEditor = new TerrainEditor(m_Terrain);
     m_SceneGraph->SetTerrain(m_Terrain);

     NodeTree::m_Instance->SetRoot(m_SceneGraph->GetRootNode());

}