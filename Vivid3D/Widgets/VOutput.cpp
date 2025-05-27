#include "VOutput.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include "Editor.h"
#include "EngineSettings.h"
#include "Engine.h"
#include "qtimer.h"
#include "Texture2D.h"
#include "MaterialBase.h"
#include "Mesh3D.h"
#include "VSceneGraph.h"
#include "NodeActor.h"
#include "MeshLines.h"
#include "MathsHelp.h"
#include "VPropertyEditor.h"
#include "Draw2D.h"
#include "Texture2D.h"
#include "SceneOctree.h"
#include "ScriptHost.h"
#include "CubeRenderer.h"
#include "MaterialMeshPBR.h"
#include "TextureCube.h"
#include "NodeTerrain.h"
#include "EditHelp.h"
#include "TerrainLayer.h"
#include "TerrainMesh.h"
#include "PixelMap.h"
#include "RenderTarget2D.h"
#include "PostProcessing.h"
#include "PPBloom.h"
#include "PPEmissive.h"
#include "NitroRenderer.h"
#include "SolarisRenderer.h"
#include "GameInput.h"

VOutput::VOutput(QWidget *parent)
	: QWidget(parent)
{
	//ui.setupUi(this);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::lightGray); // Correct enum value
 //   setAutoFillBackground(true);
    setPalette(pal);
    CreateDX12();
    m_This = this;
    Engine::m_pDevice = m_pDevice;
    Engine::m_pSwapChain = m_pSwapChain;
    Engine::m_pImmediateContext = m_pImmediateContext;
    Engine::m_pShaderFactory = m_pShaderFactory;
    
    EngineSettings::LoadSettings();

    Engine::m_ScriptHost = new ScriptHost;

    Texture2D::WhiteTex = new Texture2D("engine/white.png");

    setFocusPolicy(Qt::StrongFocus);
    m_Import = new Importer;
    m_Graph1 = new SceneGraph;

   // auto act1 = m_Import->ImportActor("test/act1.fbx");
   // act1->SetScale(float3(0.1, 0.1, 0.1));
   // auto n1 = m_Import->ImportNode("c:/content/mesh1.fbx");

   // m_Graph1->AddNode(act1);
 //   m_Graph1->AddNode(n1);

    //m_Graph1->AddNode(act1);

    m_Light1 = new NodeLight;
    m_Light1->SetPosition(float3(0, 8,0));
    m_Graph1->AddLight(m_Light1);
   



    auto l2 = new NodeLight;
    l2->SetPosition(float3(0, 12, 10));
    l2->SetDiffuse(float3(0, 2, 2));
//    m_Graph1->AddLight(l2);
    
    auto cam = m_Graph1->GetCamera();
    cam->SetPosition(float3(0, 8,0));
    setMouseTracking(true);
    m_UpdateTimer = new QTimer(this);

    m_EditCamera = cam;
    connect(m_UpdateTimer, &QTimer::timeout, this, &VOutput::onUpdate);
    m_UpdateTimer->setInterval(1000/60); // Set the interval in milliseconds
    m_UpdateTimer->start();

    m_RenderTimer = new QTimer(this);
    connect(m_RenderTimer, &QTimer::timeout, this, &VOutput::onRender);
    m_RenderTimer->setInterval(1000 / 60); // Set the interval in milliseconds
    m_RenderTimer->start();



    Editor::m_Graph = m_Graph1;
    Engine::m_ActiveGraph = m_Graph1;

   
    m_GizCam = (NodeEntity*)m_Import->ImportNode("engine/gizmo/cam1.fbx");

    auto m1 = m_GizCam->GetMesh(0);
    m1->GetMaterial()->SetDiffuse(new Texture2D("engine/gizmo/green.png"));

    m_GizTranslate = (NodeEntity*)m_Import->ImportNode("engine/gizmo/translate1.fbx");
    auto blue_mat = new MaterialBase;
    blue_mat->Create();
    blue_mat->SetDiffuse(new Texture2D("engine/gizmo/blue.png"));

    auto meshes = m_GizTranslate->GetMeshes();


    meshes[2]->SetMaterial(blue_mat);

    //mesh 2 = Y
    //mesh 1 = X 
    //mesh 0 = Z

    auto red_mat = new MaterialBase;
    red_mat->Create();
    red_mat->SetDiffuse(new Texture2D("engine/gizmo/red.png"));

    meshes[1]->SetMaterial(red_mat);


    auto green_mat = new MaterialBase;
    green_mat->Create();
    green_mat->SetDiffuse(new Texture2D("engine/gizmo/green.png"));

    meshes[0]->SetMaterial(green_mat);

    m_GizRotate = (NodeEntity*)m_Import->ImportNode("engine/gizmo/rotate1.fbx");

    meshes = m_GizRotate->GetMeshes();

    meshes[0]->SetMaterial(blue_mat);

    //0 = Yaw
    //1 = Roll
    //2 = pitch

    meshes[1]->SetMaterial(red_mat);
    meshes[2]->SetMaterial(green_mat);


    m_Gizmo = m_GizRotate;

    m_SceneGrid = new MeshLines;

    float grid_size = 80;

    int li = 0;

    for (float x = -grid_size; x < grid_size; x+=2) {

        LineVertex v1, v2;

        v1.position = float3(x, -0.1, -grid_size);
        v2.position = float3(x, -0.1, grid_size);
        v1.color = float4(0.4f,0.4f,0.4f, 1);
        v2.color = float4(0.4f,0.4f,0.4f, 1);

        Line l1;
        l1.V0 = li;
        l1.V1 = li+1;

        li += 2;

        m_SceneGrid->AddVertex(v1);
        m_SceneGrid->AddVertex(v2);

        m_SceneGrid->AddLine(l1);

        //

        v1.position = float3(-grid_size, -0.1, x);
        v2.position = float3(grid_size, -0.1, x);
        
        l1.V0 = li;
        l1.V1 = li + 1;
        li += 2;

        m_SceneGrid->AddVertex(v1);
        m_SceneGrid->AddVertex(v2);

        m_SceneGrid->AddLine(l1);
    }

    for (float x = -grid_size; x < grid_size; x+=4) {

        LineVertex v1, v2;

        v1.position = float3(x, -0.1, -grid_size);
        v2.position = float3(x, -0.1, grid_size);
        v1.color = float4(1, 1, 1, 1);
        v2.color = float4(1, 1, 1, 1);

        Line l1;
        l1.V0 = li;
        l1.V1 = li + 1;

        li += 2;

        m_SceneGrid->AddVertex(v1);
        m_SceneGrid->AddVertex(v2);

        m_SceneGrid->AddLine(l1);

        //

        v1.position = float3(-grid_size, -0.1, x);
        v2.position = float3(grid_size, -0.1, x);

        l1.V0 = li;
        l1.V1 = li + 1;
        li += 2;

        m_SceneGrid->AddVertex(v1);
        m_SceneGrid->AddVertex(v2);

        m_SceneGrid->AddLine(l1);
    }


    LineVertex x1, x2;

    x1.position = float3(-100, -0.05, 0);
    x2.position = float3(100, -0.05, 0);
    x1.color = float4(1, 0, 0, 1);
    x2.color = float4(1, 0, 0, 1);

    m_SceneGrid->AddVertex(x1);
    m_SceneGrid->AddVertex(x2);
    Line l1;
    l1.V0 = li++;
    l1.V1 = li++;
    m_SceneGrid->AddLine(l1);

    x1.position = float3(0, -0.05, -100);
    x2.position = float3(0, -0.05, 100);
    x1.color = float4(0, 1,0, 1);
    x2.color = float4(0, 1, 0, 1);

    m_SceneGrid->AddVertex(x1);
    m_SceneGrid->AddVertex(x2);
    
    l1.V0 = li++;
    l1.V1 = li++;
    m_SceneGrid->AddLine(l1);




    m_SceneGrid->CreateBuffer();
    //m_Graph1->AddLines(m_SceneGrid);


    m_Draw = new Draw2D;
    m_Tex1 = new Texture2D("test/test1.png");
    m_LightIcon = new Texture2D("edit/icons/lighticon.png");
    m_CamIcon = new Texture2D("edit/icons/camicon.png");
    m_CubeRen = new CubeRenderer(m_Graph1,nullptr);

    //m_Oct1 = new SceneOctree(m_Graph1);
    //int leafs = m_Oct1->LeafCount();
    int bb = 5;
    //auto cm1 = new TextureCube("test/test");
   // auto t1 = (NodeEntity*)m_Import->ImportNode("test/mesh2.fbx");
   // m_Graph1->AddNode(t1);
   // t1->GetMeshes()[0]->GetMaterial()->SetEnvironmentTex(cm1);
    //auto t = new NodeTerrain(128, 128, 2, 3);


    //m_Graph1->AddNode(t);
    m_BrushMaterial = new MaterialBase;
    m_BrushMaterial->Create();
    m_BrushMaterial->SetDiffuse(new Texture2D("edit/brush1.png"));
   
  
    auto input = new GameInput;

    //m_Nitro->SetGizmo(m_Gizmo);

}



void VOutput::CreateDX12() {

    WId wid = winId();
    HWND hwnd = reinterpret_cast<HWND>(wid);

#if PLATFORM_WIN32
    Win32NativeWindow Window{ hwnd };
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

}

void VOutput::resizeEvent(QResizeEvent* event)
{
    // Call the base class implementation (optional, but good practice)
    QWidget::resizeEvent(event);

    // Handle the resize event
    QSize newSize = event->size();
    QSize oldSize = event->oldSize();
  //  if (pSelf->m_pSwapChain != nullptr)
     m_pSwapChain->Resize(static_cast<Uint32>(newSize.width()), static_cast<Uint32>(newSize.height()));
    // For demonstration, print the new size to the debug output
    qDebug() << "Widget resized from" << oldSize << "to" << newSize;
    Engine::SetFrameWidth(newSize.width());
    Engine::SetFrameHeight(newSize.height());

   // m_PP = new PostProcessing;
  //  m_ppBloom = new PPBloom;
  //  m_PP->AddPostProcess(m_ppBloom);
  //  m_ppBloom->SetGraph(m_Graph1);
    m_Solaris = new SolarisRenderer;
    m_Solaris->SetSceneGraph(m_Graph1);
    Engine::m_Solaris = m_Solaris;
    Engine::m_ActiveRenderer = m_Solaris;
    m_Nitro = new NitroRenderer;
    m_Nitro->SetSceneGraph(m_Graph1);

    m_Nitro->AddLines(m_SceneGrid);

   // m_ppEmissive = new PPEmissive;
  //  m_PP->AddPostProcess(m_ppEmissive);
   // m_ppEmissive->SetGraph(m_Graph1);
   // m_RT2 = new RenderTarget2D(Engine::GetFrameWidth(), Engine::GetFrameHeight());

}


void VOutput::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {



        for (auto c : m_Graph1->GetCameras()) {

            float2 sp;

            if (c->GetRoot() != nullptr) {
                sp = m_Graph1->ToScreenSpace(c->GetPosition() * c->GetRoot()->GetWorldMatrix());
            }
            else {
                sp = m_Graph1->ToScreenSpace(c->GetPosition());
            }

            //            m_Draw->Rect(m_LightIcon, float2(sp.x - 32, sp.y - 32), float2(64, 64), float4(1, 1, 1, 1));
            if (m_MousePosition.x() >= sp.x - 32 && m_MousePosition.x() <= sp.x + 32)
            {
                if (m_MousePosition.y() >= sp.y - 32 && m_MousePosition.y() <= sp.y + 32)
                {
                    Editor::m_CurrentNode = (Node*)c;// m_Entity;
                    Editor::m_SceneGraph->SetNode((Node*)c);
                    Editor::m_PropEditor->SetNode((Node*)c);
                    
                    return;
                }
            }

        }
        for (auto l : m_Graph1->GetLights()) {

            float2 sp;

            if (l->GetRoot() != nullptr) {
                sp = m_Graph1->ToScreenSpace(l->GetPosition() * l->GetRoot()->GetWorldMatrix());
            }
            else {
                sp = m_Graph1->ToScreenSpace(l->GetPosition());
            }

//            m_Draw->Rect(m_LightIcon, float2(sp.x - 32, sp.y - 32), float2(64, 64), float4(1, 1, 1, 1));
            if (m_MousePosition.x() >= sp.x - 32 && m_MousePosition.x() <= sp.x + 32)
            {
                if (m_MousePosition.y() >= sp.y - 32 && m_MousePosition.y() <= sp.y + 32)
                {
                    Editor::m_CurrentNode = (Node*)l;// m_Entity;
                    Editor::m_SceneGraph->SetNode((Node*)l);
                    Editor::m_PropEditor->SetNode((Node*)l);
                    return;
                }
            }

        }


        //m_Graph1->MousePick(m_MousePosition.x(), m_MousePosition.y());
        // Handle left mouse button press

        if (Editor::m_CurrentNode != nullptr) {
            m_Gizmo->BuildGeo();
            auto res = m_Graph1->MousePick(m_MousePosition.x(), m_MousePosition.y(), m_Gizmo);

            switch (Editor::m_GizmoMode) {
            case GM_Translate:
                if (res.m_Hit) {
                    printf("Hit!!\n");
                    if (res.m_Entity->IsMeshIndex(res.m_Mesh, 2)) {
                        // printf("Y!!!\n");
                        m_LockY = true;
                        m_LockX = 0;
                        m_LockZ = 0;

                    }
                    if (res.m_Entity->IsMeshIndex(res.m_Mesh, 1)) {
                        // printf("Y!!!\n");
                        m_LockY = false;
                        m_LockX = true;
                        m_LockZ = 0;

                    }
                    if (res.m_Entity->IsMeshIndex(res.m_Mesh, 0)) {
                        // printf("Y!!!\n");
                        m_LockY = false;
                        m_LockX = false;
                        m_LockZ = true;

                    }
                    return;
                }

                break;
            case GM_Rotate:
                if (res.m_Hit) {
                    if (res.m_Entity->IsMeshIndex(res.m_Mesh, 2)) {
                        // printf("Y!!!\n");
                        m_LockY = false;
                        m_LockX = true;
                        m_LockZ = 0;

                    }
                    if (res.m_Entity->IsMeshIndex(res.m_Mesh, 1)) {
                        // printf("Y!!!\n");
                        m_LockY = false;
                        m_LockX = false;
                        m_LockZ = true;

                    }
                    if (res.m_Entity->IsMeshIndex(res.m_Mesh, 0)) {
                        // printf("Y!!!\n");
                        m_LockY = true;
                        m_LockX = false;
                        m_LockZ = false;

                    }
                    return;
                }

                //return;
                break;
            }
        }
            int ts = clock();

        


        auto res1 = m_Graph1->MousePick(m_MousePosition.x(), m_MousePosition.y());
        int fs = clock() - ts;
       // printf("Pick Time:%d\n", fs);
        
        if (res1.m_Hit) {
            if (res1.m_Node != nullptr) {

                auto p_node = Editor::m_CurrentNode;
                Editor::m_CurrentNode = (Node*)res1.m_Node;

                NodeTerrain* ter = dynamic_cast<NodeTerrain*>(Editor::m_CurrentNode);
                if (ter != nullptr) {
                    
                    if (p_node != Editor::m_CurrentNode)
                    {
                        Editor::m_PropEditor->SetTerrain(ter);
                   
                    }
                    m_PlotTerrain = true;
                    if (m_PlotTerrain) {

                        switch (Editor::m_TerrainEditMode) {
                        case EM_Paint:
                            TerrainPlot();
                            break;
                        case EM_Sculpt:
                            TerrainSculpt();
                            break;
                        }
                        return;

                    }
                    return;
                }
                else {
                    m_PlotTerrain = false;
                }
            }
            else {
                Editor::m_CurrentNode = res1.m_Entity;
            }
            Editor::m_SceneGraph->SetNode((Node*)res1.m_Entity);
            Editor::m_PropEditor->SetNode((Node*)res1.m_Entity);
            return;
        }
        else {
            Editor::m_SceneGraph->SetNode(nullptr);
            Editor::m_CurrentNode = nullptr;
            Editor::m_PropEditor->SetNode(nullptr);
        }

    }
    else if (event->button() == Qt::RightButton)
    {


        float2 ps = m_Graph1->ToScreenSpace(m_Gizmo->GetPosition());

       // printf("XP:%f YP:%f\n", ps.x, ps.y);

        m_CamRotate = true;
        // Handle right mouse button press
    }
    // Add other buttons if needed

    // Call base class implementation if necessary
    QWidget::mousePressEvent(event);
}

void VOutput::mouseReleaseEvent(QMouseEvent* event)
{
    m_PlotTerrain = false;
    if (event->button() == Qt::LeftButton)
    {
      
        m_LockX = false;
        m_LockY = false;
        m_LockZ = false;
        Editor::m_PropEditor->UpdateNode();
      
        // Handle left mouse button release
    }
    else if (event->button() == Qt::RightButton)
    {
        m_CamRotate = false;
        // Handle right mouse button release
    }
    // Add other buttons if needed

    // Call base class implementation if necessary
    QWidget::mouseReleaseEvent(event);
}
float ay = 0;

void VOutput::mouseMoveEvent(QMouseEvent* event)
{


  


    QPoint delta = event->pos() - m_MouseLast;
    m_MouseLast = event->pos();
    m_MousePosition = event->pos();

    // Handle mouse move event
    // Use event->pos() or event->globalPos() to get the cursor position
    if (m_CamRotate) {
        m_ViewYaw += delta.x() * 0.2f;
        m_ViewPitch += delta.y() * 0.2f;

        //  update();
    }

    //Translate from origin along selected axis, then into screen space. if final x>y use mouse x else use mouse y
    NodeTerrain* pTerrain = dynamic_cast<NodeTerrain*>(Editor::m_CurrentNode);

    if (pTerrain)
    {

        auto res = m_Graph1->MousePick((int)m_MousePosition.x(), (int)m_MousePosition.y(), pTerrain);
      
        if (res.m_Hit) {
           // printf("!!!!!!!!!!!!!!!!!\n");
            Editor::TerrainX = res.m_Point.x;
            Editor::TerrainZ = res.m_Point.z;
            m_TerrainBrush = EditHelp::CreateTerrainBrush(res.m_Point.x, res.m_Point.y+0.1f, res.m_Point.z, Editor::TerrainBrushSize, 1.0f);
        }
        else {
            m_TerrainBrush = nullptr;
       }
        //update();
        if (m_PlotTerrain) {

            switch (Editor::m_TerrainEditMode) {
            case EM_Paint:
                TerrainPlot();
                break;
            case EM_Sculpt:
                TerrainSculpt();
                break;
            }
            return;

        }

       // 
        //Edit.EditorGlobal.EditorRenderer.RenderNodeBasic(TerrainBrush);



        int b = 5;

        return;
    }

    if (Editor::m_CurrentNode != nullptr) {
        switch (Editor::m_GizmoMode)
        {
        case GM_Translate:

            if (Editor::m_SpaceMode == SM_Local)
            {
                if (m_LockY) {
                    //Editor::m_CurrentNode->Translate(float3(0, -delta.y() * 0.01f, 0));
                    float3 s_w = Editor::m_CurrentNode->GetPosition();
                    float2 s_p = m_Graph1->ToScreenSpace(s_w);

                    float3 x_w = s_w + Editor::m_CurrentNode->TransformVector(float3(0, 2, 0));
                    float2 x_p = m_Graph1->ToScreenSpace(x_w);

                    int b = 5;

                    float xd = x_p.x - s_p.x;
                    float yd = x_p.y - s_p.y;

                    if (abs(xd) > abs(yd)) {

                        float s = 1.0f;

                        if (xd < 0) {
                            s = -1.0f;
                        }
                        Editor::m_CurrentNode->Move(float3(0, delta.x() * 0.01f * s, 0));

                    }
                    else {

                        float s = 1.0f;
                        if (yd < 0) {
                            s = -1.0f;
                        }

                        Editor::m_CurrentNode->Move(float3(0, delta.y() * 0.01f * s, 0));

                    }


                    //Editor::m_CurrentNode->Translate(float3(0,0, -delta.x() * 0.01f));


                }
                if (m_LockX) {
                    //Editor::m_CurrentNode->Translate(float3(-delta.x() * 0.01f,0, 0));

                    float3 s_w = Editor::m_CurrentNode->GetPosition();
                    float2 s_p = m_Graph1->ToScreenSpace(s_w);

                    float3 x_w = s_w + Editor::m_CurrentNode->TransformVector(float3(2, 0, 0));
                    float2 x_p = m_Graph1->ToScreenSpace(x_w);

                    int b = 5;

                    float xd = x_p.x - s_p.x;
                    float yd = x_p.y - s_p.y;

                    if (abs(xd) > abs(yd)) {

                        float s = 1.0f;

                        if (xd < 0) {
                            s = -1.0f;
                        }
                        Editor::m_CurrentNode->Move(float3(delta.x() * 0.01f * s, 0, 0));

                    }
                    else {

                        float s = 1.0f;
                        if (yd < 0) {
                            s = -1.0f;
                        }

                        Editor::m_CurrentNode->Move(float3(delta.y() * 0.01f * s, 0, 0));

                    }


                }
                if (m_LockZ) {
                    float3 s_w = Editor::m_CurrentNode->GetPosition();
                    float2 s_p = m_Graph1->ToScreenSpace(s_w);

                    float3 x_w = s_w + Editor::m_CurrentNode->TransformVector(float3(0, 0, 2));
                    float2 x_p = m_Graph1->ToScreenSpace(x_w);

                    int b = 5;

                    float xd = x_p.x - s_p.x;
                    float yd = x_p.y - s_p.y;

                    if (abs(xd) > abs(yd)) {

                        float s = 1.0f;

                        if (xd < 0) {
                            s = -1.0f;
                        }
                        Editor::m_CurrentNode->Move(float3(0, 0, delta.x() * 0.01f * s));

                    }
                    else {

                        float s = 1.0f;
                        if (yd < 0) {
                            s = -1.0f;
                        }

                        Editor::m_CurrentNode->Move(float3(0, 0, delta.y() * 0.01f * s));

                    }


                    //Editor::m_CurrentNode->Translate(float3(0,0, -delta.x() * 0.01f));

                }
            }
            if (Editor::m_SpaceMode == SM_Global) {

                if (m_LockY) {
                    Editor::m_CurrentNode->Translate(float3(0, -delta.y() * 0.01f, 0));
                }
                if (m_LockX) {
                    //Editor::m_CurrentNode->Translate(float3(-delta.x() * 0.01f,0, 0));

                    float3 s_w = Editor::m_CurrentNode->GetPosition();
                    float2 s_p = m_Graph1->ToScreenSpace(s_w);

                    float3 x_w = s_w + float3(2, 0, 0);
                    float2 x_p = m_Graph1->ToScreenSpace(x_w);

                    int b = 5;

                    float xd = x_p.x - s_p.x;
                    float yd = x_p.y - s_p.y;

                    if (abs(xd) > abs(yd)) {

                        float s = 1.0f;

                        if (xd < 0) {
                            s = -1.0f;
                        }
                        Editor::m_CurrentNode->Translate(float3(delta.x() * 0.01f * s, 0, 0));

                    }
                    else {

                        float s = 1.0f;
                        if (yd < 0) {
                            s = -1.0f;
                        }

                        Editor::m_CurrentNode->Translate(float3(delta.y() * 0.01f * s, 0, 0));

                    }


                }
                if (m_LockZ) {
                    float3 s_w = Editor::m_CurrentNode->GetPosition();
                    float2 s_p = m_Graph1->ToScreenSpace(s_w);

                    float3 x_w = s_w + float3(0, 0, 2);
                    float2 x_p = m_Graph1->ToScreenSpace(x_w);

                    int b = 5;

                    float xd = x_p.x - s_p.x;
                    float yd = x_p.y - s_p.y;

                    if (abs(xd) > abs(yd)) {

                        float s = 1.0f;

                        if (xd < 0) {
                            s = -1.0f;
                        }
                        Editor::m_CurrentNode->Translate(float3(0, 0, delta.x() * 0.01f * s));

                    }
                    else {

                        float s = 1.0f;
                        if (yd < 0) {
                            s = -1.0f;
                        }

                        Editor::m_CurrentNode->Translate(float3(0, 0, delta.y() * 0.01f * s));

                    }


                    //Editor::m_CurrentNode->Translate(float3(0,0, -delta.x() * 0.01f));

                }
            }

            break;
        case GM_Rotate:

            if (Editor::m_SpaceMode == SM_Local) {


                if (m_LockY) {

                    Editor::m_CurrentNode->Turn(0, delta.x() * 0.1f, 0, false);
                    //Editor::m_PropEditor->UpdateNode();

                }
                //Pitch
                if (m_LockX) {


                    float3 s_w = Editor::m_CurrentNode->GetPosition();
                    float2 s_p = m_Graph1->ToScreenSpace(s_w);

                    float3 x_w = s_w + Editor::m_CurrentNode->TransformVector(float3(2, 0, 0));
                    float2 x_p = m_Graph1->ToScreenSpace(x_w);

                    int b = 5;

                    float xd = x_p.x - s_p.x;
                    float yd = x_p.y - s_p.y;

                    if (abs(xd) > abs(yd)) {

                        float s = -1.0f;

                        if (xd < 0) {
                            s = 1.0f;
                        }
                        Editor::m_CurrentNode->Turn(delta.y() * 0.1f * s, 0, 0, true);
                     //   Editor::m_PropEditor->UpdateNode();

                    }
                    else {

                        float s = 1.0f;
                        if (yd < 0) {
                            s = -1.0f;
                        }

                        Editor::m_CurrentNode->Turn(delta.x() * 0.1f * s, 0, 0, true);
                       // Editor::m_PropEditor->UpdateNode();

                    }


                    //Editor::m_CurrentNode->Turn(delta.x() * 0.1f, 0, 0, true);

                }
                if (m_LockZ) {

                    // Editor::m_CurrentNode->Turn(0, 0,delta.y() * 0.1f, true);

                    float3 s_w = Editor::m_CurrentNode->GetPosition();
                    float2 s_p = m_Graph1->ToScreenSpace(s_w);

                    float3 x_w = s_w + Editor::m_CurrentNode->TransformVector(float3(0, 0, 2));
                    float2 x_p = m_Graph1->ToScreenSpace(x_w);

                    int b = 5;

                    float xd = x_p.x - s_p.x;
                    float yd = x_p.y - s_p.y;

                    if (abs(xd) > abs(yd)) {

                        float s = -1.0f;

                        if (xd < 0) {
                            s = 1.0f;
                        }

                        Editor::m_CurrentNode->Turn(0, 0, delta.y() * 0.1f * s, true);
                        //Editor::m_PropEditor->UpdateNode();
                    }
                    else {

                        float s = 1.0f;
                        if (yd < 0) {
                            s = -1.0f;
                        }


                        Editor::m_CurrentNode->Turn(0, 0, delta.x() * 0.1f * s, true);
                        //Editor::m_PropEditor->UpdateNode();
                    }


                }
            }

            if (Editor::m_SpaceMode == SM_Global) {
                //Yaw
                if (m_LockY) {

                    Editor::m_CurrentNode->Turn(0, delta.x() * 0.1f, 0, false);

                }
                //Pitch
                if (m_LockX) {

                    float3 s_w = Editor::m_CurrentNode->GetPosition();
                    float2 s_p = m_Graph1->ToScreenSpace(s_w);

                    float3 x_w = s_w + float3(2, 0, 0);
                    float2 x_p = m_Graph1->ToScreenSpace(x_w);

                    int b = 5;

                    float xd = x_p.x - s_p.x;
                    float yd = x_p.y - s_p.y;

                    if (abs(xd) > abs(yd)) {

                        float s = -1.0f;

                        if (xd < 0) {
                            s = 1.0f;
                        }
                        Editor::m_CurrentNode->Turn(delta.y() * 0.1f * s, 0, 0, false);
                    //    Editor::m_PropEditor->UpdateNode();
                    }
                    else {

                        float s = 1.0f;
                        if (yd < 0) {
                            s = -1.0f;
                        }

                        Editor::m_CurrentNode->Turn(delta.x() * 0.1f * s, 0, 0, false);
                      //  Editor::m_PropEditor->UpdateNode();
                    }


                    //Editor::m_CurrentNode->Turn(delta.x() * 0.1f, 0, 0, true);

                }
                if (m_LockZ) {
                    // Editor::m_CurrentNode->Turn(0, 0,delta.y() * 0.1f, true);

                    float3 s_w = Editor::m_CurrentNode->GetPosition();
                    float2 s_p = m_Graph1->ToScreenSpace(s_w);

                    float3 x_w = s_w + float3(2, 0, 0);
                    float2 x_p = m_Graph1->ToScreenSpace(x_w);

                    int b = 5;

                    float xd = x_p.x - s_p.x;
                    float yd = x_p.y - s_p.y;

                    if (abs(xd) > abs(yd)) {

                        float s = -1.0f;

                        if (xd < 0) {
                            s = 1.0f;
                        }

                        Editor::m_CurrentNode->Turn(0, 0, delta.x() * 0.1f * s, false);
                       // Editor::m_PropEditor->UpdateNode();
                    }
                    else {

                        float s = -1.0f;
                        if (yd < 0) {
                            s = 1.0f;
                        }


                        Editor::m_CurrentNode->Turn(0, 0, delta.y() * 0.1f * s, false);
                        //Editor::m_PropEditor->UpdateNode();
                    }


                }
            }
            else {

            }

            break;
        }

    }
    // Call base class implementation if necessary
    QWidget::mouseMoveEvent(event);

}


void VOutput::keyPressEvent(QKeyEvent* event)
{

    if (Editor::m_RunMode == RM_Playing) {

        m_MoveW = m_MoveD = m_MoveA = m_MoveS = false;

        GameInput::This->SetKey(event->key(), true);



    }
    else {

        if (event->key() == Qt::Key_Space)
        {
            m_Light1->SetPosition(m_EditCamera->GetPosition());
        }
        // Check which key was pressed
        if (event->key() == Qt::Key_W) {
            //qDebug() << "Key A is pressed";
            m_MoveW = true;
        }
        else if (event->key() == Qt::Key_A) {
            m_MoveA = true;
            //    qDebug() << "Space bar is pressed";
        }
        else if (event->key() == Qt::Key_D)
        {
            m_MoveD = True;
        }
        else if (event->key() == Qt::Key_S)
        {
            m_MoveS = true;
        }
        if (event->key() == Qt::Key_Shift)
        {
            m_MoveFast = true;
        }
    }
    // Call base class implementation (if needed)
    QWidget::keyPressEvent(event);
}

void VOutput::keyReleaseEvent(QKeyEvent* event)
{
    if (Editor::m_RunMode == RM_Playing) {

        m_MoveW = m_MoveD = m_MoveA = m_MoveS = false;

        GameInput::This->SetKey(event->key(), false);



    }
    else {
        // Check which key was released
        if (event->key() == Qt::Key_W) {
            //qDebug() << "Key A is pressed";
            m_MoveW = false;
        }
        else if (event->key() == Qt::Key_A) {
            m_MoveA = false;
            //    qDebug() << "Space bar is pressed";
        }
        else if (event->key() == Qt::Key_D)
        {
            m_MoveD = false;
        }
        else if (event->key() == Qt::Key_S)
        {
            m_MoveS = false;
        }
        if (event->key() == Qt::Key_Shift)
        {
            m_MoveFast = false;
        }
    }
    // Call base class implementation (if needed)
    QWidget::keyReleaseEvent(event);
}

void VOutput::onUpdate()
{

    QPoint globalPos = QCursor::pos();
    QPoint localPos = mapFromGlobal(globalPos);

    QPoint delta = localPos - m_MouseLast2;

    m_MouseLast2 = localPos;
//    m_MousePosition = event->pos();
    GameInput::This->m_MousePos = float2(localPos.x(), localPos.y());
    GameInput::This->m_MouseDelta = float2(delta.x(), delta.y());
    int tick = clock();

    if (m_LastTick == 0) {
        m_LastTick = tick;
    }

    int ntick = clock();

    int tick_r = ntick - m_LastTick;

    if (tick_r > 0) {

        m_TimeDelta = (float)((float)tick_r) / 1000.f;

        m_Graph1->Update(m_TimeDelta);

        m_LastTick = ntick;
    }

    float scale = 1.0f * m_TimeDelta * 70.0f;

    if (m_MoveFast) {
        scale = 2.5f * m_TimeDelta * 70.0f;
    }

    bool up = false;
    if (m_MoveW) {
        m_EditCamera->Move(float3(0, 0, 0.1f*scale));
        up = true;
    }
    if (m_MoveA) {
        m_EditCamera->Move(float3(-0.1f*scale, 0,0));
        up = true;
    }
    if (m_MoveS) {
        m_EditCamera->Move(float3(0, 0, -0.1f*scale));
        up = true;
    }
    if (m_MoveD) {
        m_EditCamera->Move(float3(0.1f*scale, 0, 0));
        up = true;
    }
 
   // update();
    // Action to perform while the key is held down
    //qDebug() << "Key A is held down";
}


void VOutput::onRender() {

    update();

}

VOutput::~VOutput()
{}


void VOutput::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    printf("PaintStart\n");

    // Perform rendering using DX12
    
    auto* pContext = GetContext();
    auto* pSwapchain = GetSwapChain();

    ITextureView* pRTV = pSwapchain->GetCurrentBackBufferRTV();

    pContext->SetRenderTargets(1, &pRTV,pSwapchain->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    const float ClearColor[4] = { 0.02f,0.02f,0.02f,1.0 };
    pContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
    pContext->ClearDepthStencil(pSwapchain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    
    auto cam = m_Graph1->GetCamera();
    ay = ay + 0.1f;

  //  m_Node1->SetRotation(0, ay, 0);
    m_Graph1 = Editor::m_Graph;
//    m_ppBloom->SetGraph(m_Graph1);
//    m_Nitro->SetSceneGraph(m_Graph1);


    if (Editor::m_RunMode != RM_Playing) {
        cam->SetRotation(m_ViewPitch, m_ViewYaw, 0);
    }

  //  m_Graph1->RenderShadows();
   // m_CubeRen->RenderEnvironment(m_Node2->GetPosition());



   // auto pbr = (MaterialMeshPBR*)m_Node2->GetMesh(0)->GetMaterial();
   // pbr->SetEnvironment(m_CubeRen->GetTarget());

    // pbr = (MaterialMeshPBR*)m_Node1->GetMesh(0)->GetMaterial();
   // pbr->SetEnvironment(m_CubeRen->GetTarget());


    int ss = clock();

    //m_RT2->Bind();
    //m_Graph1->Render();

   // m_RT2->Release();
    // m_Oct1->RenderBF();
    //m_Graph1->RenderLines();
    //m_RT2->Bind();
    //m_Graph1->Render();
    //m_RT2->Release();

    //m_PP->Process(new Texture2D(m_RT2));

    if (Editor::m_RunMode == RM_Playing) {

        switch (EngineSettings::UsedRenderer) {
        case RendererSetting::R_Nitro:
            Engine::m_ActiveRenderer = m_Nitro;
            m_Nitro->PreRender();
            m_Nitro->Render(false);
            break;
        case RendererSetting::R_Solaris:
            Engine::m_ActiveRenderer =  m_Solaris;
            m_Solaris->PreRender();
            m_Solaris->Render(false);
            break;
        }

    }
    else {
        m_Nitro->PreRender();
        m_Nitro->Render(false);;
        Engine::m_ActiveRenderer = m_Nitro;
    }


    Engine::m_Camera = m_Graph1->GetCamera();
    m_Solaris->SetSceneGraph(Editor::m_Graph);
 //      m_Solaris->PreRender();
   //     m_Solaris->Render(false);


     //   auto frame = m_Solaris->GetRenderedFrame();



       // if (frame != nullptr) {
         //   m_Draw->Rect(frame, float2(20, 20), float2(400, 400), float4(1, 1, 1, 1));
       //     m_PP->Process(frame);
        //}
    int ts = clock() - ss;

   // printf("RenderTime:%d\n", ts);


    if (Editor::m_CurrentNode!=nullptr &&  m_Gizmo != nullptr && Editor::m_RunMode != RM_Playing) {

        m_Nitro->ClearGizmo();
        m_Nitro->AddGizmo(m_Gizmo);
        if (dynamic_cast<NodeCamera*>(Editor::m_CurrentNode)) {
            m_Nitro->AddGizmo(m_GizCam);
            m_GizCam->SetPosition(Editor::m_CurrentNode->GetPosition() * Editor::m_CurrentNode->GetRoot()->GetWorldMatrix());
            if (Editor::m_CurrentNode->GetRoot() != nullptr) {
                m_GizCam->SetRotation(Editor::m_CurrentNode->GetRotation()*Editor::m_CurrentNode->GetRoot()->GetWorldRotation());
            }
            else {
                m_GizCam->SetRotation(Editor::m_CurrentNode->GetRotation());
            }
        }
    }
    else {
        m_Nitro->SetGizmo(nullptr);
    }

  

    switch (Editor::m_GizmoMode) {
    case GM_Translate:
        m_Gizmo = m_GizTranslate;
        break;
    case GM_Rotate:
        m_Gizmo = m_GizRotate;
        break;
    }

    if (Editor::m_CurrentNode != nullptr) {
        m_Gizmo->SetPosition(Editor::m_CurrentNode->GetPosition()*Editor::m_CurrentNode->GetRoot()->GetWorldMatrix());
        if (Editor::m_SpaceMode == SM_Local) {
            m_Gizmo->SetRotation(Editor::m_CurrentNode->GetRotation()*Editor::m_CurrentNode->GetRoot()->GetRotation());
        }
        else {
            m_Gizmo->SetRotation(0, 0, 0);
        }

        float dis = MathsHelp::Distance(m_EditCamera->GetPosition(), m_Gizmo->GetPosition());

         dis = dis / 10.0f;

         if (dis < 0.65f) dis = 0.65f;

         if (dis > 6.0f) dis = 6.0f;


        m_Gizmo->SetScale(float3(dis, dis, dis));

      //  m_Gizmo->Render(false);
    }


    if (Editor::m_RunMode != RM_Playing) {
        for (auto l : m_Graph1->GetLights()) {

            float2 sp;

            if (l->GetRoot() != nullptr) {
                sp = m_Graph1->ToScreenSpace(l->GetPosition() * l->GetRoot()->GetWorldMatrix());
            }
            else {
                sp = m_Graph1->ToScreenSpace(l->GetPosition());
            }

            m_Draw->Rect(m_LightIcon, float2(sp.x - 32, sp.y - 32), float2(64, 64), float4(1, 1, 1, 1));


        }

        for (auto c : m_Graph1->GetCameras()) {

            float2 sp;

            if (c->GetRoot() != nullptr) {
                sp = m_Graph1->ToScreenSpace(c->GetPosition() * c->GetRoot()->GetWorldMatrix());
            }
            else {
                sp = m_Graph1->ToScreenSpace(c->GetPosition());
            }

            m_Draw->Rect(m_CamIcon, float2(sp.x - 32, sp.y - 32), float2(64, 64), float4(1, 1, 1, 1));


        }

    }

    if (m_TerrainBrush != nullptr) {


       m_TerrainBrush->SetMaterial(m_BrushMaterial);
  //      m_TerrainBrush->Render(false);

    }

    //m_Draw->Rect(m_LightIcon, float2(20, 20), float2(256, 256), float4(1, 1, 1, 1));

    //pContext->Flush();
 
  //  pContext->FinishFrame();
  //  pContext->Flush();

    pSwapchain->Present();
    printf("Paint End\n");

 

  //  QWidget::paintEvent(event);
}



void VOutput::TerrainPlot() {


   

    //var ter = Edit.EditorGlobal.CurrentTerrain;
    auto ter = (NodeTerrain*)Editor::m_CurrentNode;

    //var l1 = ter.Layers[TerrainEditorForm.ActiveLayer];
    auto layer = ter->GetLayer(Editor::TerrainEditLayer);

    auto bb = ter->GetTerrainBounds();

    float tw = bb.Size().x;
    float th = bb.Size().z;

    float tx = Editor::TerrainX;
    float ty = Editor::TerrainZ;


    float tx1 = tx - bb.Min.x;
    float ty1 = ty - bb.Min.z;

    

    float mxi = ((layer->GetPixels()->GetWidth()) / tw);// * TerrainBrushSize;
    float myi = ((layer->GetPixels()->GetHeight()) / th);


    float w1 = Editor::TerrainBrushSize * mxi;
    float h1 = Editor::TerrainBrushSize * myi;



    float xi = tx1 / tw;
    float yi = ty1 / th;

    //                Console.WriteLine("XI:" + xi + " YI:" + yi);

    //return;


    //     xi = 1.0f - xi;
    //    yi = 1.0f - yi;

    layer->PlotBrush(xi, yi, (int)w1, (int)h1,Editor::TerrainBrushStrength);

    //Lower other layers by a similar(Inverse?) amount.
    for(auto l : ter->GetLayers())
    {
        if (l == layer) continue;
        l->PlotBrush(xi, yi, (int)w1, (int)h1, -Editor::TerrainBrushStrength);
    }

}

void VOutput::TerrainSculpt() {

    //var ter = Edit.EditorGlobal.CurrentTerrain;

    auto ter =(NodeTerrain*) Editor::m_CurrentNode;


    //List<TerrainVertex> vertices = new List<TerrainVertex>();

    std::vector<TerrainVertex>& verts = ter->GetMesh()->GetVertices();


    int v = 0;
    for(auto vert : verts)
    {

        float xd = (ter->GetPosition().x + vert.position.x) - Editor::TerrainX;
        float zd = (ter->GetPosition().z + vert.position.z) - Editor::TerrainZ;

        float dis = (float)sqrt(xd * xd + zd * zd);

        if (dis < Editor::TerrainBrushSize)
        {

            dis = dis / Editor::TerrainBrushSize;
            if (dis > 1.0) dis = 1.0f;
            dis = 1.0f - dis;

            TerrainVertex nv = vert;

            //nv.position.y = nv.position.y + 0.02f * Editor::TerrainBrushStrength;
            verts[v].position.y = vert.position.y + 0.02f * Editor::TerrainBrushStrength;
            
            //ter.Mesh.Vertices[v] = nv;



        }


        v++;
    }

   // ter->GetMesh()->SetVertices(verts);
    ter->GetMesh()->CalculateNormals();
    ter->GetMesh()->Build();

//    ter.Mesh.CalculateNormalsBinormalsAndTangents();
//    ter.Mesh.CreateBuffers();
//    ter.LookUpMesh = null;

}

void VOutput::SetCam(float p, float y) {

    m_ViewPitch = p;
    m_ViewYaw = y;

}

VOutput* VOutput::m_This = nullptr;