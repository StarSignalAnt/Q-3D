#include "pch.h"
#include "NitroRenderer.h"
#include "SceneGraph.h"
#include "Engine.h"
#include <vector>
#include "NodeLight.h"
#include "RenderTargetCube.h"
#include "Texture2D.h"

#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Buffer.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
#include <MapHelper.hpp> // Add this line
#include "TextureCube.h"
#include "NodeEntity.h"
#include "NodeActor.h"
#include "NodeTerrain.h"
#include "TerrainLayer.h"
#include "Mesh3D.h"
#include "TerrainMesh.h"
#include "TerrainMeshBuffer.h"
#include "MeshBuffer.h"
#include "MaterialBase.h"
#include "CubeRenderer.h"
#include "MeshLines.h"
#include "MeshLinesBuffer.h"
#include "PostProcessing.h"
#include "PostProcess.h"
#include "PPBloom.h"
#include "PPEmissive.h"
#include "RenderTarget2D.h"
#include "Draw2D.h"
#include "TextureDepth.h"
#include "PPSSR.h"
using namespace Diligent;

//Constants

struct MeshPBRConstants {

    float4x4 g_MVP;
    float4x4 g_Proj;
    float4x4 g_Model;
    float4x4 g_View;
    float4x4 g_ModelInv;
    float4 v_CameraPosition;
    float4 v_LightPosition;
    float4 v_ExtraProp;
    float4 v_LightDiffuse;
    int4 v_LightMode;
    float4 v_LightDir;
    float4 v_ViewDir;
    float4 v_LightCone;


};


struct NormalsConstants {

    float4x4 g_MVP;
    float4x4 g_Proj;
    float4x4 g_Model;
    float4x4 g_View;
    float4x4 g_ModelInv;
    float4 v_CameraPosition;
    float4 v_LightPosition;
    float4 v_ExtraProp;
    float4 v_LightDiffuse;
    int4 v_LightMode;
    float4 v_LightDir;
    float4 v_ViewDir;
    float4 v_LightCone;


};



struct PositionsConstants {

    float4x4 g_MVP;
    float4x4 g_Proj;
    float4x4 g_Model;
    float4x4 g_View;
    float4x4 g_ModelInv;
    float4 v_CameraPosition;
    float4 v_LightPosition;
    float4 v_ExtraProp;
    float4 v_LightDiffuse;
    int4 v_LightMode;
    float4 v_LightDir;
    float4 v_ViewDir;
    float4 v_LightCone;


};


struct ActorConstants
{

    float4x4 v_MVP;
    float4x4 v_Proj;
    float4x4 v_Model;
    float4x4 v_View;
    float4x4 v_ModelInv;
    float4 v_CameraPos;
    float4 v_LightPos;
    float4 v_LightProp;
    float4 v_LightDiff;
    float4 v_LightSpec;
    float4 v_LenderProps;
    int4 v_LightMode;
    float4 v_LightDir;
    float4 v_LightCone;
    float4 v_MatDiff;
    float4 v_MatSpec;
    float4 v_CameraExt;
    float4x4 bones[100];
};

struct DepthConstants {

    float4x4 v_MVP;
    float4x4 v_Proj;
    float4x4 v_Model;
    float4x4 v_View;
    float4x4 v_ModelInv;
    float4 v_CameraPos;
    float4 v_LightPos;
    float4 v_LightProp;
    float4 v_LightDiff;
    float4 v_LightSpec;
    float4 v_RenderProps;
    int4 v_LightMode;
    float4 v_LightDir;
    float4 v_LightCone;
    float4 v_MatDiff;
    float4 v_MatSpec;
    float4 v_CameraExt;

};


struct ActorDepthConstants
{

    float4x4 v_MVP;
    float4x4 v_Proj;
    float4x4 v_Model;
    float4x4 v_View;
    float4x4 v_ModelInv;
    float4 v_CameraPos;
    float4 v_LightPos;
    float4 v_LightProp;
    float4 v_LightDiff;
    float4 v_LightSpec;
    float4 v_LenderProps;
    int4 v_LightMode;
    float4 v_LightDir;
    float4 v_LightCone;
    float4 v_MatDiff;
    float4 v_MatSpec;
    float4 v_CameraExt;
    float4x4 bones[100];
};


struct TerrainConstants {

    float4x4 v_MVP;
    float4x4 v_Proj;
    float4x4 v_Model;
    float4x4 v_View;
    float4x4 v_ModelInv;
    float4 v_CameraPos;
    float4 v_LightPos;
    float4 v_LightProp;
    float4 v_LightDiff;
    float4 v_LightSpec;
    float4 v_RenderProps;
    int4 v_LightMode;
    float4 v_LightDir;
    float4 v_LightCone;
    float4 v_MatDiff;
    float4 v_MatSpec;
    float4 v_CameraExt;
    int4 v_Layers;

};



NitroRenderer::NitroRenderer() {

	CreateMeshPBRPipeline();
    CreateActorPipeline();
    CreateMeshDepthPipeline();
    CreateActorDepthPipeline();
    CreateTerrainPipeline();
    CreateTerrainDepthPipeline();
    CreateGizmoPipeline();
    CreateLinesPipeline();
    CreateNormalsPipeline();
    CreateExtrasPipeline();
    CreatePositionsPipeline();
    CreatePostProcessing();

    //EnableBloom(true);
    EnableSSR(true);
    m_Frame = new RenderTarget2D(Engine::GetFrameWidth(), Engine::GetFrameHeight());
    m_Normals = new RenderTarget2D(Engine::GetFrameWidth(), Engine::GetFrameHeight());
    m_Positions = new RenderTarget2D(Engine::GetFrameWidth(), Engine::GetFrameHeight());
    m_Draw = new Draw2D;

}

void NitroRenderer::CreatePostProcessing() {

    m_PostProcessing = new PostProcessing;
    m_PPBloom = new PPBloom;
    m_PPSSR = new PPSSR;
}

void NitroRenderer::SetupPP() {

    if (m_PPChanged) {
        m_PostProcessing->Clear();

        if (m_BloomEnabled)
        {
            m_PostProcessing->AddPostProcess(m_PPBloom);
        }
        if (m_SSREnabled) {
            m_PostProcessing->AddPostProcess(m_PPSSR);
        }
        m_PPChanged = false;
    }
}

void NitroRenderer::PreRender()
{

    if (m_PPChanged) {
        SetupPP();
    }
    if (GetSceneGraph()->IsUpdated()) {

        int b = 5;
        m_Entities.clear();
        for (auto entity : GetSceneGraph()->GetEntities()) {

            m_Entities.push_back(entity);

        }
        m_Actors.clear();
        for (auto actor : GetSceneGraph()->GetActors()) {
            m_Actors.push_back(actor);
        }
        m_ActiveTerrain = GetSceneGraph()->GetTerrain();
    }

    RenderShadows();

//	GetSceneGraph()->RenderShadows();

}

void NitroRenderer::Render(bool no_render) {

    //GetSceneGraph()->Render();




    if (m_PPEnabled)
    {
        m_Frame->Bind();
        for (auto actor : m_Actors) {

            RenderActor(actor);

        }
        for (auto entity : m_Entities) {

            RenderEntityPBR(entity);

        }

        if (m_ActiveTerrain) {
            RenderTerrain(m_ActiveTerrain);
        }

        for (auto lines : m_Lines) {

            RenderLines(lines);

        }



        //Engine::ClearZ();
     //   m_Frame->ClearZ();

        for (auto giz : m_Gizmos) {
       //     RenderGizmo(giz);

        }

        m_Frame->Release();



        m_Normals->Bind();

        for (auto entity : m_Entities) {

            RenderNormals(entity);

        }

        m_Normals->Release();


        m_Positions->Bind();
        for (auto entity : m_Entities) {

            RenderPositions(entity);

        }
        m_Positions->Release();

        m_PPSSR->SetPositions(new Texture2D(m_Positions));
        m_PPSSR->SetColor(new Texture2D(m_Frame));
        m_PPSSR->SetNormals(new Texture2D(m_Normals));
        m_PPSSR->SetDepth(new TextureDepth(m_Frame));

        m_PostProcessing->Process(new Texture2D(m_Frame));

        //m_Draw->Rect(new Texture2D(m_Frame), float2(0, 0), float2(256, 256), float4(1, 1, 1, 1));


    }
    else {
        for (auto actor : m_Actors) {

            RenderActor(actor);

        }
        for (auto entity : m_Entities) {

            RenderEntityPBR(entity);

        }

        if (m_ActiveTerrain) {
            RenderTerrain(m_ActiveTerrain);
        }

        for (auto lines : m_Lines) {

            RenderLines(lines);

        }



        Engine::ClearZ();

        for (auto giz : m_Gizmos) {
            RenderGizmo(giz);

        }

    }
    
}

//Renderers

void NitroRenderer::RenderEntityPBR(NodeEntity* entity) {

    
    auto camera = GetSceneGraph()->GetCamera();
    auto lights = GetSceneGraph()->GetLights();
    auto light = lights[0];

    float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);

    float4x4 mvp = camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


    float4x4 view = camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

    float4x4 model = entity->GetWorldMatrix();

    float4x4 id = float4x4::Identity().Inverse();

    //mvp = mvp*id;

    //mvp.Transpose();


    mvp = model * view * mvp;

    MeshPBRConstants dd;

   dd.g_MVP = mvp.Transpose();
  dd.g_Model = model.Transpose();
  dd.g_ModelInv = model.Inverse().Transpose();
  dd.g_Proj = camera->GetProjection().Transpose();
  dd.g_View = view.Transpose();
  dd.v_LightDiffuse = light->GetDiffuse();
  dd.v_LightPosition = float4(light->GetPosition(), 1.0f);
//  dd.vlightSpec = float4(light->GetSpecular(), 1.0f);


  dd.v_CameraPosition = float4(camera->GetPosition(), 1.0);



  dd.v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
  dd.v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);


  
    {

        
        printf("Mapping!\n");
        MapHelper<MeshPBRConstants> map_data(Engine::m_pImmediateContext, m_EntityPBRConstants,MAP_TYPE::MAP_WRITE, MAP_FLAG_DISCARD);
       





        *map_data = dd;

        //map_data[0].g_MVP = mvp.Transpose();
        //map_data[0].g_Model = model.Transpose();
        //map_data[0].g_ModelInv = model.Inverse().Transpose();
        //map_data[0].g_Proj = camera->GetProjection().Transpose();
        //map_data[0].g_View = view.Transpose();
        //map_data[0].v_LightDiffuse = light->GetDiffuse();
        //map_data[0].v_LightPosition = float4(light->GetPosition(), 1.0f);
        //  map_data[0].lightSpec = float4(light->GetSpecular(), 1.0f);

        //map_data[0].v_CameraPosition = float4(camera->GetPosition(), 1.0);



        //map_data[0].v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
        //map_data[0].v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);

        //map_data.Unmap();
        printf("Unampping!\n");
        //return;


    }
    
    //return;



    for (auto mesh : entity->GetMeshes())
    {

        auto material = mesh->GetMaterial();

        auto albedo = material->GetDiffuse();
        auto norm = material->GetNormal();
        auto rough = material->GetRough();
        auto metal = material->GetMetal();

  //      return;
        //Bind Textures

        m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(albedo->GetView(),SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureNorm")->Set(norm->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureRough")->Set(rough->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureMetal")->Set(metal->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
      
        m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Shadow")->Set(light->GetShadowMap()->GetTexView());

        //

        //

        int b = 5;

        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { mesh->GetBuffer()->GetVertexBuffer() };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

   //     return;
        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets,flags);
        Engine::m_pImmediateContext->SetIndexBuffer(mesh->GetBuffer()->GetIndexBuffer(), 0, flags );

     //   return;


        Engine::m_pImmediateContext->SetPipelineState(m_PS_EntityPBR);

        Engine::m_pImmediateContext->CommitShaderResources(m_EntityPBR_SRB, flags);

        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;

        attrib.NumIndices = mesh->GetTris().size() * 3;

        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);

    }

    Engine::m_pImmediateContext->Flush();

}

void NitroRenderer::RenderActor(NodeActor* actor) {

    auto camera = GetSceneGraph()->GetCamera();
    auto lights = GetSceneGraph()->GetLights();
    auto light = lights[0];

    float4x4 mvp = camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


    float4x4 view = camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

    float4x4 model = actor->GetWorldMatrix();

    float4x4 id = float4x4::Identity().Inverse();

    //mvp = mvp*id;

    //mvp.Transpose();


    mvp = model * view * mvp;

    auto m_Bones = actor->GetBones();


    ActorConstants dd;

    dd.v_MVP = mvp.Transpose();
    dd.v_Model = model.Transpose();
    dd.v_ModelInv = model.Inverse().Transpose();
    dd.v_Proj = camera->GetProjection().Transpose();
    dd.v_View = view.Transpose();
    dd.v_LightDiff = light->GetDiffuse();
    dd.v_LightPos = float4(light->GetPosition(), 1.0f);
    dd.v_LightSpec = float4(light->GetSpecular(), 1.0f);
    dd.v_MatDiff = float4(1, 1, 1, 1);
    dd.v_MatSpec = float4(1, 1, 1, 1);
    dd.v_CameraPos = float4(camera->GetPosition(), 1.0);
    dd.v_LightProp = float4(light->GetRange(), 0, 0, 0);
    for (int i = 0; i < 100; i++) {
        dd.bones[i] = m_Bones[i].Transpose();

    }



    {
        printf("Mapping!\n");
        MapHelper<ActorConstants> map_data(Engine::m_pImmediateContext, m_ActorConstants, MAP_WRITE, MAP_FLAG_DISCARD);
        float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);






        *map_data = dd;
        //map_data.Unmap();
        printf("Unmapping.");
    }


    for (auto mesh : actor->GetMeshes())
    {

        auto material = mesh->GetMaterial();

        auto albedo = material->GetDiffuse();
        auto norm = material->GetNormal();
        auto spec = material->GetSpecular();


        //Bind Textures

        m_Actor_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(albedo->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        m_Actor_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureNorm")->Set(norm->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        m_Actor_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureSpec")->Set(spec->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        //m_Actor_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureMetal")->Set(metal->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        m_Actor_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Shadow")->Set(light->GetShadowMap()->GetTexView());


        //
      

        //

        int b = 5;

        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { mesh->GetBuffer()->GetVertexBuffer() };

        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        Engine::m_pImmediateContext->SetIndexBuffer(mesh->GetBuffer()->GetIndexBuffer(), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        Engine::m_pImmediateContext->SetPipelineState(m_PS_Actor);

        Engine::m_pImmediateContext->CommitShaderResources(m_Actor_SRB,RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;

        attrib.NumIndices = mesh->GetTris().size() * 3;

        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);

    }

}

RefCntAutoPtr<IBuffer> CreateUniform(int size, std::string path) {

    BufferDesc desc;
    desc.Name = path.c_str();
    desc.Size = (Uint64)size;
    desc.Usage = USAGE_DYNAMIC;
    desc.BindFlags = BIND_UNIFORM_BUFFER;
    desc.CPUAccessFlags = CPU_ACCESS_WRITE;

    RefCntAutoPtr<IBuffer> buffer;

    Engine::m_pDevice->CreateBuffer(desc, nullptr, &buffer);

    return buffer;

}


//MESH PBR PIPELINE

void NitroRenderer::CreateMeshPBRPipeline() {

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "EntityPBR PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;

  
    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "MeshPBR VS";
        ShaderCI.FilePath = "pbr_lit.vsh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        m_EntityPBRConstants = CreateUniform(sizeof(MeshPBRConstants), "Mesh PBR");
    }



    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "MeshPBR PS";
        ShaderCI.FilePath = "pbr_lit.psh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pPS);
    }


    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 4, VT_FLOAT32, False},
        LayoutElement{2, 0, 3, VT_FLOAT32, False},
        LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
        LayoutElement{5, 0, 3, VT_FLOAT32, False},
        LayoutElement(6,0,4,VT_FLOAT32,False),
        LayoutElement(7,0,4,VT_FLOAT32,False)

    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis
    ShaderResourceVariableDesc Vars[] =
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        {SHADER_TYPE_PIXEL, "g_TextureNorm", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        { SHADER_TYPE_PIXEL, "g_TextureRough", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
        { SHADER_TYPE_PIXEL, "g_TextureMetal", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
        {SHADER_TYPE_PIXEL, "v_Shadow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
    };

    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureNorm", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureRough", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureMetal", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_Shadow", SamLinearClampDesc}

    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

    Engine::m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PS_EntityPBR);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_PS_EntityPBR->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_EntityPBRConstants);


    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_PS_EntityPBR->CreateShaderResourceBinding(&m_EntityPBR_SRB, true);


}


//Actor Pipeline
void NitroRenderer::CreateActorPipeline() {

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Actor PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;


    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Actor VS";
        ShaderCI.FilePath = "mesh_actorlit.vsh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        m_ActorConstants = CreateUniform(sizeof(ActorConstants), "Actor Constants");
    }



    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Actor PS";
        ShaderCI.FilePath = "mesh_actorlit.psh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pPS);
    }


    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 4, VT_FLOAT32, False},
        LayoutElement{2, 0, 3, VT_FLOAT32, False},
        LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
        LayoutElement{5, 0, 3, VT_FLOAT32, False},
        LayoutElement(6,0,4,VT_FLOAT32,False),
        LayoutElement(7,0,4,VT_FLOAT32,False)

    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis
    ShaderResourceVariableDesc Vars[] =
    {
        {SHADER_TYPE_PIXEL, "v_Texture", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        {SHADER_TYPE_PIXEL, "v_TextureNorm", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        { SHADER_TYPE_PIXEL, "v_TextureSpec", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
         { SHADER_TYPE_PIXEL, "v_Shadow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC }
    };

    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "v_Texture", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_TextureNorm", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_TextureSpec", SamLinearClampDesc},
           {SHADER_TYPE_PIXEL, "v_Shadow", SamLinearClampDesc}


    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

    Engine::m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PS_Actor);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_PS_Actor->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_ActorConstants);


    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_PS_Actor->CreateShaderResourceBinding(&m_Actor_SRB, true);


}

void NitroRenderer::RenderEntityDepth(NodeEntity* entity) {


    auto camera = GetSceneGraph()->GetCamera();
    auto lights = GetSceneGraph()->GetLights();
    auto light = lights[0];

    float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);

    float4x4 mvp = camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


    float4x4 view = camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

    float4x4 model = entity->GetWorldMatrix();

    float4x4 id = float4x4::Identity().Inverse();

    //mvp = mvp*id;

    //mvp.Transpose();


    mvp = model * view * mvp;

    DepthConstants dd;

    dd.v_MVP = mvp.Transpose();
    dd.v_Model = model.Transpose();
    dd.v_ModelInv = model.Inverse().Transpose();
    dd.v_Proj = camera->GetProjection().Transpose();
    dd.v_View = view.Transpose();
  //  dd.v_LightDiffuse = light->GetDiffuse();
  //  dd.v_LightPosition = float4(light->GetPosition(), 1.0f);
    //  dd.vlightSpec = float4(light->GetSpecular(), 1.0f);


    dd.v_CameraPos = float4(camera->GetPosition(), 1.0);




    //dd.v_CameraExt = float4(light->GetRange(), 0, 0, 0);
    //dd.v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);
    dd.v_CameraExt = float4(camera->GetNearZ(),camera->GetFarZ(), 0, 0);



    {


        printf("Mapping!\n");
        MapHelper<DepthConstants> map_data(Engine::m_pImmediateContext, m_EntityDepthConstants, MAP_TYPE::MAP_WRITE, MAP_FLAG_DISCARD);






        *map_data = dd;

        //map_data[0].g_MVP = mvp.Transpose();
        //map_data[0].g_Model = model.Transpose();
        //map_data[0].g_ModelInv = model.Inverse().Transpose();
        //map_data[0].g_Proj = camera->GetProjection().Transpose();
        //map_data[0].g_View = view.Transpose();
        //map_data[0].v_LightDiffuse = light->GetDiffuse();
        //map_data[0].v_LightPosition = float4(light->GetPosition(), 1.0f);
        //  map_data[0].lightSpec = float4(light->GetSpecular(), 1.0f);

        //map_data[0].v_CameraPosition = float4(camera->GetPosition(), 1.0);



        //map_data[0].v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
        //map_data[0].v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);

        //map_data.Unmap();
        printf("Unampping!\n");
        //return;


    }

    //return;



    for (auto mesh : entity->GetMeshes())
    {

        auto material = mesh->GetMaterial();

        auto albedo = material->GetDiffuse();
        auto norm = material->GetNormal();
        auto rough = material->GetRough();
        auto metal = material->GetMetal();

        //      return;
              //Bind Textures

       // m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(albedo->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
       // m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureNorm")->Set(norm->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
       // m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureRough")->Set(rough->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
       // m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureMetal")->Set(metal->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);

       // m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Shadow")->Set(light->GetShadowMap()->GetTexView());


        //

        //

        int b = 5;

        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { mesh->GetBuffer()->GetVertexBuffer() };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Engine::m_pImmediateContext->SetIndexBuffer(mesh->GetBuffer()->GetIndexBuffer(), 0, flags);

        //   return;


        Engine::m_pImmediateContext->SetPipelineState(m_PS_EntityDepth);

        Engine::m_pImmediateContext->CommitShaderResources(m_EntityDepth_SRB, flags);

        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;

        attrib.NumIndices = mesh->GetTris().size() * 3;

        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);

    }

    Engine::m_pImmediateContext->Flush();

}


void NitroRenderer::CreateMeshDepthPipeline() {

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Entity Depth PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;


    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Mesh Depth VS";
        ShaderCI.FilePath = "mat_depth.vsh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        m_EntityDepthConstants = CreateUniform(sizeof(DepthConstants), "Mesh Depth");
    }



    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Mesh Depth PS";
        ShaderCI.FilePath = "mat_depth.psh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pPS);
    }


    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 4, VT_FLOAT32, False},
        LayoutElement{2, 0, 3, VT_FLOAT32, False},
        LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
        LayoutElement{5, 0, 3, VT_FLOAT32, False},
        LayoutElement(6,0,4,VT_FLOAT32,False),
        LayoutElement(7,0,4,VT_FLOAT32,False)

    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis

    // clang-format on
  //  PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = 0;

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureNorm", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureRough", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureMetal", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_Shadow", SamLinearClampDesc}

    };
    // clang-format on
  //  PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = 0;

    Engine::m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PS_EntityDepth);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_PS_EntityDepth->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_EntityDepthConstants);



    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_PS_EntityDepth->CreateShaderResourceBinding(&m_EntityDepth_SRB, true);




}




void NitroRenderer::RenderShadows() {

    auto lights = GetSceneGraph()->GetLights();
    auto cam = GetSceneGraph()->GetCamera();
    NodeCamera* new_cam = nullptr;

    for (auto light : lights) {

        auto target = light->GetShadowMap();

        auto pos = light->GetPosition();
        auto maxZ = light->GetRange();

        auto prev_cam = cam;
        if (new_cam == nullptr) {
            new_cam = new NodeCamera;
        }
        else {

        }
        GetSceneGraph()->SetCamera(new_cam);

        new_cam->SetPosition(pos);
        new_cam->SetNearZ(prev_cam->GetNearZ());
        new_cam->SetFarZ(maxZ);
        new_cam->SetFov(90.0f);
        for (int i = 0; i < 6; i++) {

            switch (i) {
            case 0:
                new_cam->SetRotation(float4x4::RotationY(PI_F / 2.0f));
                break;
            case 1:
                new_cam->SetRotation(float4x4::RotationY(-PI_F / 2.0f));
                break;
            case 2:
                new_cam->SetRotation(float4x4::RotationX(-PI_F / 2.0f));
                break;
            case 3:
                new_cam->SetRotation(float4x4::RotationX(PI_F / 2.0f));
                break;
            case 4:
                new_cam->SetRotation(float4x4::Identity());
                break;
            case 5:
                new_cam->SetRotation(float4x4::RotationY(PI_F));
                break;
            }


            target->Bind(i);
          //  m_Graph->RenderDepth();
           
            for (auto entity : m_Entities) {

                RenderEntityDepth(entity);

            }

            for (auto actor : m_Actors) {

                RenderActorDepth(actor);

            }

            if (m_ActiveTerrain) {

                RenderTerrainDepth(m_ActiveTerrain);

            }

            target->Release(i);


        }


        GetSceneGraph()->SetCamera(prev_cam);

    }

}


void NitroRenderer::CreateActorDepthPipeline() {

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Actor Depth PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;


    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Actor Depth VS";
        ShaderCI.FilePath = "mesh_actordepth.vsh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        m_ActorDepthConstants = CreateUniform(sizeof(ActorDepthConstants), "Actor Depth");
    }



    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Actorh Depth PS";
        ShaderCI.FilePath = "mesh_actordepth.psh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pPS);
    }


    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 4, VT_FLOAT32, False},
        LayoutElement{2, 0, 3, VT_FLOAT32, False},
        LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
        LayoutElement{5, 0, 3, VT_FLOAT32, False},
        LayoutElement(6,0,4,VT_FLOAT32,False),
        LayoutElement(7,0,4,VT_FLOAT32,False)

    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis

    // clang-format on
  //  PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = 0;

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureNorm", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureRough", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureMetal", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_Shadow", SamLinearClampDesc}

    };
    // clang-format on
  //  PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = 0;

    Engine::m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PS_ActorDepth);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_PS_ActorDepth->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_ActorDepthConstants);




    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_PS_ActorDepth->CreateShaderResourceBinding(&m_ActorDepth_SRB, true);




}


void NitroRenderer::RenderActorDepth(NodeActor* actor) {


    auto camera = GetSceneGraph()->GetCamera();
    auto lights = GetSceneGraph()->GetLights();
    auto light = lights[0];

    float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);

    float4x4 mvp = camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


    float4x4 view = camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

    float4x4 model = actor->GetWorldMatrix();

    float4x4 id = float4x4::Identity().Inverse();

    //mvp = mvp*id;

    //mvp.Transpose();


    mvp = model * view * mvp;

    ActorDepthConstants dd;
    auto m_Bones = actor->GetBones();

    dd.v_MVP = mvp.Transpose();
    dd.v_Model = model.Transpose();
    dd.v_ModelInv = model.Inverse().Transpose();
    dd.v_Proj = camera->GetProjection().Transpose();
    dd.v_View = view.Transpose();
    for (int i = 0; i < 100; i++) {
        dd.bones[i] = m_Bones[i].Transpose();

    }
    
    //  dd.v_LightDiffuse = light->GetDiffuse();
    //  dd.v_LightPosition = float4(light->GetPosition(), 1.0f);
      //  dd.vlightSpec = float4(light->GetSpecular(), 1.0f);


    dd.v_CameraPos = float4(camera->GetPosition(), 1.0);




    //dd.v_CameraExt = float4(light->GetRange(), 0, 0, 0);
    //dd.v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);
    dd.v_CameraExt = float4(camera->GetNearZ(), camera->GetFarZ(), 0, 0);



    {


        printf("Mapping!\n");
        MapHelper<ActorDepthConstants> map_data(Engine::m_pImmediateContext, m_ActorDepthConstants, MAP_TYPE::MAP_WRITE, MAP_FLAG_DISCARD);






        *map_data = dd;

        //map_data[0].g_MVP = mvp.Transpose();
        //map_data[0].g_Model = model.Transpose();
        //map_data[0].g_ModelInv = model.Inverse().Transpose();
        //map_data[0].g_Proj = camera->GetProjection().Transpose();
        //map_data[0].g_View = view.Transpose();
        //map_data[0].v_LightDiffuse = light->GetDiffuse();
        //map_data[0].v_LightPosition = float4(light->GetPosition(), 1.0f);
        //  map_data[0].lightSpec = float4(light->GetSpecular(), 1.0f);

        //map_data[0].v_CameraPosition = float4(camera->GetPosition(), 1.0);



        //map_data[0].v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
        //map_data[0].v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);

        //map_data.Unmap();
        printf("Unampping!\n");
        //return;


    }

    //return;



    for (auto mesh : actor->GetMeshes())
    {

        auto material = mesh->GetMaterial();

        auto albedo = material->GetDiffuse();
        auto norm = material->GetNormal();
        auto rough = material->GetRough();
        auto metal = material->GetMetal();

        //      return;
              //Bind Textures

       // m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(albedo->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
       // m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureNorm")->Set(norm->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
       // m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureRough")->Set(rough->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
       // m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureMetal")->Set(metal->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);

       // m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Shadow")->Set(light->GetShadowMap()->GetTexView());


        //

        //

        int b = 5;

        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { mesh->GetBuffer()->GetVertexBuffer() };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Engine::m_pImmediateContext->SetIndexBuffer(mesh->GetBuffer()->GetIndexBuffer(), 0, flags);

        //   return;


        Engine::m_pImmediateContext->SetPipelineState(m_PS_ActorDepth);

        Engine::m_pImmediateContext->CommitShaderResources(m_ActorDepth_SRB, flags);

        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;

        attrib.NumIndices = mesh->GetTris().size() * 3;

        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);

    }

    Engine::m_pImmediateContext->Flush();


}


void NitroRenderer::CreateTerrainPipeline() {


    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Terrain PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;


    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Terrain VS";
        ShaderCI.FilePath = "mat_terrain.vsh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        m_TerrainConstants = CreateUniform(sizeof(TerrainConstants), "Terrain");
    }



    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Terrain PS";
        ShaderCI.FilePath = "mat_terrain.psh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pPS);
    }


    //
 
    //

    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 4, VT_FLOAT32, False},
        LayoutElement{2, 0, 3, VT_FLOAT32, False},
        LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
        LayoutElement{5, 0, 3, VT_FLOAT32, False},
        LayoutElement(6,0,3,VT_FLOAT32,False),


    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis
    ShaderResourceVariableDesc Vars[] =
    {
        {SHADER_TYPE_PIXEL, "v_Texture", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        {SHADER_TYPE_PIXEL, "v_TextureNorm", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        { SHADER_TYPE_PIXEL, "v_TextureSpec", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
        { SHADER_TYPE_PIXEL, "v_TextureLayer", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
        {SHADER_TYPE_PIXEL, "v_Shadow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
    };

    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_WRAP, TEXTURE_ADDRESS_WRAP, TEXTURE_ADDRESS_WRAP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "v_Texture", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_TextureNorm", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_TextureSpec", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_TextureLayer", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_Shadow", SamLinearClampDesc}

    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

    Engine::m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PS_Terrain);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_PS_Terrain->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_TerrainConstants);


    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_PS_Terrain->CreateShaderResourceBinding(&m_Terrain_SRB, true);



}

void NitroRenderer::RenderTerrain(NodeTerrain* terrain)
{

    auto light = GetSceneGraph()->GetLights()[0];
    auto camera = GetSceneGraph()->GetCamera();

    auto m_Buffer =  terrain->GetMesh()->GetBuffer();

    int ii = 0;

    int index = 0;

    for (auto layer : terrain->GetLayers()) {


        auto lights = GetSceneGraph()->GetLights();
        auto light = lights[0];

        float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);

        float4x4 mvp = camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


        float4x4 view = camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

        float4x4 model = terrain->GetWorldMatrix();

        float4x4 id = float4x4::Identity().Inverse();

        //mvp = mvp*id;

        //mvp.Transpose();


        mvp = model * view * mvp;

        TerrainConstants dd;

        dd.v_MVP = mvp.Transpose();
        dd.v_Model = model.Transpose();
        dd.v_ModelInv = model.Inverse().Transpose();
        dd.v_Proj = camera->GetProjection().Transpose();
        dd.v_View = view.Transpose();
        dd.v_LightDiff = light->GetDiffuse();
        dd.v_LightPos = float4(light->GetPosition(), 1.0f);
        //  dd.vlightSpec = float4(light->GetSpecular(), 1.0f);

        dd.v_LightProp = float4(light->GetRange(), 0, 0, 0);
        dd.v_CameraPos = float4(camera->GetPosition(), 1.0);
        dd.v_Layers = int4(index, 0, 0, 0);

        index++;
        //      dd.v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
      //        dd.v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);




        {
        printf("Mapping!\n");
        MapHelper<TerrainConstants> map_data(Engine::m_pImmediateContext, m_TerrainConstants, MAP_TYPE::MAP_WRITE, MAP_FLAG_DISCARD);






        *map_data = dd;
    }


        //m_Material->SetDiffuse(layer->GetColor());
        //m_Material->SetNormals(layer->GetNormal());
        //m_Material->SetSpecular(layer->GetSpec());
       // m_Material->SetLayerMap(layer->GetLayerMap());
       // m_Material->SetLayerIndex(ii);
       // m_Material->Bind(sp);

        auto col = layer->GetColor();
        auto norm = layer->GetNormal();
        auto spec = layer->GetSpec();

        m_Terrain_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(col->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        m_Terrain_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureNorm")->Set(norm->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        m_Terrain_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureSpec")->Set(spec->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        m_Terrain_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureLayer")->Set(layer->GetLayerMap()->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);

        m_Terrain_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Shadow")->Set(light->GetShadowMap()->GetTexView());



        ii++;
        IBuffer* pBuffs[] = { m_Buffer->GetVertexBuffer() };
        Uint64 offsets = 0;
        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
        Engine::m_pImmediateContext->SetIndexBuffer(m_Buffer->GetTriangleBuffer(), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        Engine::m_pImmediateContext->SetPipelineState(m_PS_Terrain);

        Engine::m_pImmediateContext->CommitShaderResources(m_Terrain_SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;
        attrib.NumIndices = terrain->GetMesh()->GetTriangles().size() * 3;
        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);


    }

}

void NitroRenderer::CreateTerrainDepthPipeline() {



    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Terrain PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;


    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "TerrainDepth VS";
        ShaderCI.FilePath = "mat_terraindepth.vsh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        m_TerrainDepthConstants = CreateUniform(sizeof(TerrainConstants), "TerrainDepth");
    }



    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "TerrainDepth PS";
        ShaderCI.FilePath = "mat_terraindepth.psh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pPS);
    }


    //

    //

    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 4, VT_FLOAT32, False},
        LayoutElement{2, 0, 3, VT_FLOAT32, False},
        LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
        LayoutElement{5, 0, 3, VT_FLOAT32, False},
        LayoutElement(6,0,3,VT_FLOAT32,False),


    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis
    ShaderResourceVariableDesc Vars[] =
    {
        {SHADER_TYPE_PIXEL, "v_Texture", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        {SHADER_TYPE_PIXEL, "v_TextureNorm", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        { SHADER_TYPE_PIXEL, "v_TextureSpec", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
        { SHADER_TYPE_PIXEL, "v_TextureLayer", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
        {SHADER_TYPE_PIXEL, "v_Shadow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
    };

    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = 0;

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_WRAP, TEXTURE_ADDRESS_WRAP, TEXTURE_ADDRESS_WRAP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "v_Texture", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_TextureNorm", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_TextureSpec", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_TextureLayer", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_Shadow", SamLinearClampDesc}

    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = 0;

    Engine::m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PS_TerrainDepth);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_PS_TerrainDepth->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_TerrainDepthConstants);


    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_PS_TerrainDepth->CreateShaderResourceBinding(&m_TerrainDepth_SRB, true);



}

void NitroRenderer::RenderTerrainDepth(NodeTerrain* terrain) {

    auto camera = GetSceneGraph()->GetCamera();
    auto lights = GetSceneGraph()->GetLights();
    auto light = lights[0];

    float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);

    float4x4 mvp = camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


    float4x4 view = camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

    float4x4 model = terrain->GetWorldMatrix();

    float4x4 id = float4x4::Identity().Inverse();

    //mvp = mvp*id;

    //mvp.Transpose();


    mvp = model * view * mvp;

  

    TerrainConstants dd;

    dd.v_MVP = mvp.Transpose();
    dd.v_Model = model.Transpose();
    dd.v_ModelInv = model.Inverse().Transpose();
    dd.v_Proj = camera->GetProjection().Transpose();
    dd.v_View = view.Transpose();
    dd.v_LightDiff = light->GetDiffuse();
    dd.v_LightPos = float4(light->GetPosition(), 1.0f);
    //  dd.vlightSpec = float4(light->GetSpecular(), 1.0f);

    dd.v_LightProp = float4(light->GetRange(), 0, 0, 0);
    dd.v_CameraPos = float4(camera->GetPosition(), 1.0);
    dd.v_CameraExt = float4(camera->GetNearZ(), camera->GetFarZ(), 0, 0);


    {


        printf("Mapping!\n");
        MapHelper<TerrainConstants> map_data(Engine::m_pImmediateContext, m_TerrainDepthConstants, MAP_TYPE::MAP_WRITE, MAP_FLAG_DISCARD);






        *map_data = dd;

        //map_data[0].g_MVP = mvp.Transpose();
        //map_data[0].g_Model = model.Transpose();
        //map_data[0].g_ModelInv = model.Inverse().Transpose();
        //map_data[0].g_Proj = camera->GetProjection().Transpose();
        //map_data[0].g_View = view.Transpose();
        //map_data[0].v_LightDiffuse = light->GetDiffuse();
        //map_data[0].v_LightPosition = float4(light->GetPosition(), 1.0f);
        //  map_data[0].lightSpec = float4(light->GetSpecular(), 1.0f);

        //map_data[0].v_CameraPosition = float4(camera->GetPosition(), 1.0);



        //map_data[0].v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
        //map_data[0].v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);

        //map_data.Unmap();
        printf("Unampping!\n");
        //return;


    }

    //return;


    auto mesh = terrain->GetMesh();

     

        //      return;
              //Bind Textures

        //

        //

        int b = 5;

        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { mesh->GetBuffer()->GetVertexBuffer() };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Engine::m_pImmediateContext->SetIndexBuffer(mesh->GetBuffer()->GetTriangleBuffer(), 0, flags);

        //   return;


        Engine::m_pImmediateContext->SetPipelineState(m_PS_TerrainDepth);

        Engine::m_pImmediateContext->CommitShaderResources(m_TerrainDepth_SRB, flags);

        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;

        attrib.NumIndices = mesh->GetTriangles().size() * 3;

        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);

    


        Engine::m_pImmediateContext->Flush();
//
}

void NitroRenderer::CreateGizmoPipeline() {

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Gizmo PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = true;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;


    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Gizmo VS";
        ShaderCI.FilePath = "basic.vsh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        m_GizmoConstants = CreateUniform(sizeof(float4x4), "Gizmo");
    }



    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Gizmo PS";
        ShaderCI.FilePath = "basic.psh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pPS);
    }


    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 4, VT_FLOAT32, False},
        LayoutElement{2, 0, 3, VT_FLOAT32, False},
        LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
        LayoutElement{5, 0, 3, VT_FLOAT32, False},
        LayoutElement(6,0,4,VT_FLOAT32,False),
        LayoutElement(7,0,4,VT_FLOAT32,False)

    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis
    ShaderResourceVariableDesc Vars[] =
    {
        {SHADER_TYPE_PIXEL, "v_Texture", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        {SHADER_TYPE_PIXEL, "g_TextureNorm", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        { SHADER_TYPE_PIXEL, "g_TextureRough", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
        { SHADER_TYPE_PIXEL, "g_TextureMetal", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
        {SHADER_TYPE_PIXEL, "v_Shadow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
    };

    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = 1;

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "v_Texture", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureNorm", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureRough", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureMetal", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_Shadow", SamLinearClampDesc}

    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = 1;


    Engine::m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PS_Gizmo);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_PS_Gizmo->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_GizmoConstants);



    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_PS_Gizmo->CreateShaderResourceBinding(&m_Gizmo_SRB, true);


}

void NitroRenderer::RenderGizmo(NodeEntity* entity)
{

    auto camera = GetSceneGraph()->GetCamera();
    auto lights = GetSceneGraph()->GetLights();
    auto light = lights[0];

    float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);

    float4x4 mvp = camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


    float4x4 view = camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

    float4x4 model = entity->GetWorldMatrix();

    float4x4 id = float4x4::Identity().Inverse();

    //mvp = mvp*id;

    //mvp.Transpose();


    mvp = model * view * mvp;

    

    {


        printf("Mapping!\n");
        MapHelper<float4x4> map_data(Engine::m_pImmediateContext, m_GizmoConstants, MAP_TYPE::MAP_WRITE, MAP_FLAG_DISCARD);






        *map_data = mvp.Transpose();

        //map_data[0].g_MVP = mvp.Transpose();
        //map_data[0].g_Model = model.Transpose();
        //map_data[0].g_ModelInv = model.Inverse().Transpose();
        //map_data[0].g_Proj = camera->GetProjection().Transpose();
        //map_data[0].g_View = view.Transpose();
        //map_data[0].v_LightDiffuse = light->GetDiffuse();
        //map_data[0].v_LightPosition = float4(light->GetPosition(), 1.0f);
        //  map_data[0].lightSpec = float4(light->GetSpecular(), 1.0f);

        //map_data[0].v_CameraPosition = float4(camera->GetPosition(), 1.0);



        //map_data[0].v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
        //map_data[0].v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);

        //map_data.Unmap();
        printf("Unampping!\n");
        //return;


    }

    //return;



    for (auto mesh : entity->GetMeshes())
    {

        auto material = mesh->GetMaterial();

        auto albedo = material->GetDiffuse();


        //      return;
              //Bind Textures

        m_Gizmo_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(albedo->GetView(),SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);


        //

        //

        int b = 5;

        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { mesh->GetBuffer()->GetVertexBuffer() };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Engine::m_pImmediateContext->SetIndexBuffer(mesh->GetBuffer()->GetIndexBuffer(), 0, flags);

        //   return;


        Engine::m_pImmediateContext->SetPipelineState(m_PS_Gizmo);

        Engine::m_pImmediateContext->CommitShaderResources(m_Gizmo_SRB, flags);

        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;

        attrib.NumIndices = mesh->GetTris().size() * 3;

        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);

    }

    Engine::m_pImmediateContext->Flush();

}

void NitroRenderer::CreateLinesPipeline() {

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Gizmo PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_LINE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;


    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Lines VS";
        ShaderCI.FilePath = "mesh_lines.vsh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        m_LinesConstants = CreateUniform(sizeof(float4x4), "Lines");
    }



    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Lines PS";
        ShaderCI.FilePath = "mesh_lines.psh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pPS);
    }


    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 4, VT_FLOAT32, False},


    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis
    ShaderResourceVariableDesc Vars[] =
    {
        {SHADER_TYPE_PIXEL, "v_Texture", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        {SHADER_TYPE_PIXEL, "g_TextureNorm", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        { SHADER_TYPE_PIXEL, "g_TextureRough", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
        { SHADER_TYPE_PIXEL, "g_TextureMetal", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
        {SHADER_TYPE_PIXEL, "v_Shadow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
    };

    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = 0;

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "v_Texture", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureNorm", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureRough", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureMetal", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "v_Shadow", SamLinearClampDesc}

    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = 0;


    Engine::m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PS_Lines);


    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_PS_Lines->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_LinesConstants);



    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_PS_Lines->CreateShaderResourceBinding(&m_Lines_SRB, true);



}

void NitroRenderer::RenderLines(MeshLines* lines) {


    auto camera = GetSceneGraph()->GetCamera();
    auto lights = GetSceneGraph()->GetLights();
    auto light = lights[0];

    float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);

    float4x4 mvp = camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


    float4x4 view = camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

    float4x4 model = float4x4::Identity();

    float4x4 id = float4x4::Identity().Inverse();

    //mvp = mvp*id;

    //mvp.Transpose();


    mvp = model * view * mvp;



    {


        printf("Mapping!\n");
        MapHelper<float4x4> map_data(Engine::m_pImmediateContext, m_LinesConstants, MAP_TYPE::MAP_WRITE, MAP_FLAG_DISCARD);






        *map_data = mvp.Transpose();


    }

    //map_data.Unmap();

       // auto material = 


       // auto albedo = material->GetDiffuse();


        //      return;
              //Bind Textures

//        m_Gizmo_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(albedo->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);


        //

        //

        int b = 5;

        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { lines->GetBuffer()->GetVertexBuffer() };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Engine::m_pImmediateContext->SetIndexBuffer(lines->GetBuffer()->GetIndexBuffer(), 0, flags);

        //   return;


        Engine::m_pImmediateContext->SetPipelineState(m_PS_Lines);

        Engine::m_pImmediateContext->CommitShaderResources(m_Lines_SRB, flags);

        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;

        attrib.NumIndices = lines->GetLines().size() * 2;

        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);


}

void NitroRenderer::CreateNormalsPipeline() {
    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Normals PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;


    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Normals VS";
        ShaderCI.FilePath = "normals.vsh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        m_NormalsConstants = CreateUniform(sizeof(NormalsConstants), "Normals");
    }



    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Normals PS";
        ShaderCI.FilePath = "normals.psh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pPS);
    }


    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 4, VT_FLOAT32, False},
        LayoutElement{2, 0, 3, VT_FLOAT32, False},
        LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
        LayoutElement{5, 0, 3, VT_FLOAT32, False},
        LayoutElement(6,0,4,VT_FLOAT32,False),
        LayoutElement(7,0,4,VT_FLOAT32,False)

    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis
    ShaderResourceVariableDesc Vars[] =
    {
        //{SHADER_TYPE_PIXEL, "g_Texture", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        {SHADER_TYPE_PIXEL, "g_TextureNorm", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
      //  { SHADER_TYPE_PIXEL, "g_TextureRough", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
      //  { SHADER_TYPE_PIXEL, "g_TextureMetal", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
     //   {SHADER_TYPE_PIXEL, "v_Shadow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
    };

    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
       // {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc},
        {SHADER_TYPE_PIXEL, "g_TextureNorm", SamLinearClampDesc},
      //  {SHADER_TYPE_PIXEL, "g_TextureRough", SamLinearClampDesc},
      //  {SHADER_TYPE_PIXEL, "g_TextureMetal", SamLinearClampDesc},
      //  {SHADER_TYPE_PIXEL, "v_Shadow", SamLinearClampDesc}

    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

    Engine::m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PS_Normals);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_PS_Normals->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_NormalsConstants);


    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_PS_Normals->CreateShaderResourceBinding(&m_Normals_SRB, true);
}

void NitroRenderer::CreateExtrasPipeline() {



}

void NitroRenderer::RenderNormals(NodeEntity* entity) {

    auto camera = GetSceneGraph()->GetCamera();
    auto lights = GetSceneGraph()->GetLights();
    auto light = lights[0];

    float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);

    float4x4 mvp = camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


    float4x4 view = camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

    float4x4 model = entity->GetWorldMatrix();

    float4x4 id = float4x4::Identity().Inverse();

    //mvp = mvp*id;

    //mvp.Transpose();


    mvp = model * view * mvp;

    NormalsConstants dd;

    dd.g_MVP = mvp.Transpose();
    dd.g_Model = model.Transpose();
    dd.g_ModelInv = model.Inverse().Transpose();
    dd.g_Proj = camera->GetProjection().Transpose();
    dd.g_View = view.Transpose();
    dd.v_LightDiffuse = light->GetDiffuse();
    dd.v_LightPosition = float4(light->GetPosition(), 1.0f);
    //  dd.vlightSpec = float4(light->GetSpecular(), 1.0f);


    dd.v_CameraPosition = float4(camera->GetPosition(), 1.0);



    dd.v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
    dd.v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);



    {


        printf("Mapping!\n");
        MapHelper<NormalsConstants> map_data(Engine::m_pImmediateContext, m_NormalsConstants, MAP_TYPE::MAP_WRITE, MAP_FLAG_DISCARD);






        *map_data = dd;

        //map_data[0].g_MVP = mvp.Transpose();
        //map_data[0].g_Model = model.Transpose();
        //map_data[0].g_ModelInv = model.Inverse().Transpose();
        //map_data[0].g_Proj = camera->GetProjection().Transpose();
        //map_data[0].g_View = view.Transpose();
        //map_data[0].v_LightDiffuse = light->GetDiffuse();
        //map_data[0].v_LightPosition = float4(light->GetPosition(), 1.0f);
        //  map_data[0].lightSpec = float4(light->GetSpecular(), 1.0f);

        //map_data[0].v_CameraPosition = float4(camera->GetPosition(), 1.0);



        //map_data[0].v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
        //map_data[0].v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);

        //map_data.Unmap();
        printf("Unampping!\n");
        //return;


    }

    //return;



    for (auto mesh : entity->GetMeshes())
    {

        auto material = mesh->GetMaterial();

       // auto albedo = material->GetDiffuse();
        auto norm = material->GetNormal();
      //  auto rough = material->GetRough();
      //  auto metal = material->GetMetal();

        //      return;
              //Bind Textures

      //  m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(albedo->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
 //       m_Normals_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureNorm")->Set(norm->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
     //   m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureRough")->Set(rough->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
    //    m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureMetal")->Set(metal->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);

   //     m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Shadow")->Set(light->GetShadowMap()->GetTexView());

        //

        //

        int b = 5;

        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { mesh->GetBuffer()->GetVertexBuffer() };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Engine::m_pImmediateContext->SetIndexBuffer(mesh->GetBuffer()->GetIndexBuffer(), 0, flags);

        //   return;


        Engine::m_pImmediateContext->SetPipelineState(m_PS_Normals);

        Engine::m_pImmediateContext->CommitShaderResources(m_Normals_SRB, flags);

        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;

        attrib.NumIndices = mesh->GetTris().size() * 3;

        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);

    }

    Engine::m_pImmediateContext->Flush();

}

void NitroRenderer::CreatePositionsPipeline() {

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Positions PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;


    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Positions VS";
        ShaderCI.FilePath = "positions.vsh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        m_PositionsConstants = CreateUniform(sizeof(PositionsConstants), "Positions");
    }




    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Positions PS";
        ShaderCI.FilePath = "positions.psh";
        Engine::m_pDevice->CreateShader(ShaderCI, &pPS);
    }


    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 4, VT_FLOAT32, False},
        LayoutElement{2, 0, 3, VT_FLOAT32, False},
        LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
        LayoutElement{5, 0, 3, VT_FLOAT32, False},
        LayoutElement(6,0,4,VT_FLOAT32,False),
        LayoutElement(7,0,4,VT_FLOAT32,False)

    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis

    // clang-format on
//    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = 0;

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
  
    // clang-format on
    //PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = 0;

    Engine::m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PS_Positions);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_PS_Positions->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_PositionsConstants);


    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_PS_Positions->CreateShaderResourceBinding(&m_Positions_SRB, true);

}

void NitroRenderer::RenderPositions(NodeEntity* entity)
{

    auto camera = GetSceneGraph()->GetCamera();
    auto lights = GetSceneGraph()->GetLights();
    auto light = lights[0];

    float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);

    float4x4 mvp = camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


    float4x4 view = camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

    float4x4 model = entity->GetWorldMatrix();

    float4x4 id = float4x4::Identity().Inverse();

    //mvp = mvp*id;

    //mvp.Transpose();


    mvp = model * view * mvp;

    PositionsConstants dd;

    dd.g_MVP = mvp.Transpose();
    dd.g_Model = model.Transpose();
    dd.g_ModelInv = model.Inverse().Transpose();
    dd.g_Proj = camera->GetProjection().Transpose();
    dd.g_View = view.Transpose();
    dd.v_LightDiffuse = light->GetDiffuse();
    dd.v_LightPosition = float4(light->GetPosition(), 1.0f);
    //  dd.vlightSpec = float4(light->GetSpecular(), 1.0f);


    dd.v_CameraPosition = float4(camera->GetPosition(), 1.0);



    dd.v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
    dd.v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);



    {


        printf("Mapping!\n");
        MapHelper<PositionsConstants> map_data(Engine::m_pImmediateContext, m_PositionsConstants, MAP_TYPE::MAP_WRITE, MAP_FLAG_DISCARD);






        *map_data = dd;

        //map_data[0].g_MVP = mvp.Transpose();
        //map_data[0].g_Model = model.Transpose();
        //map_data[0].g_ModelInv = model.Inverse().Transpose();
        //map_data[0].g_Proj = camera->GetProjection().Transpose();
        //map_data[0].g_View = view.Transpose();
        //map_data[0].v_LightDiffuse = light->GetDiffuse();
        //map_data[0].v_LightPosition = float4(light->GetPosition(), 1.0f);
        //  map_data[0].lightSpec = float4(light->GetSpecular(), 1.0f);

        //map_data[0].v_CameraPosition = float4(camera->GetPosition(), 1.0);



        //map_data[0].v_ExtraProp = float4(light->GetRange(), 0, 0, 0);
        //map_data[0].v_ViewDir = float4(camera->TransformVector(float3(0, 0, -1.0)), 1.0);

        //map_data.Unmap();
        printf("Unampping!\n");
        //return;


    }

    //return;



    for (auto mesh : entity->GetMeshes())
    {

        auto material = mesh->GetMaterial();

        // auto albedo = material->GetDiffuse();
        auto norm = material->GetNormal();
        //  auto rough = material->GetRough();
        //  auto metal = material->GetMetal();

          //      return;
                //Bind Textures

        //  m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(albedo->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
      //  m_Normals_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureNorm")->Set(norm->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        //   m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureRough")->Set(rough->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
       //    m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureMetal")->Set(metal->GetView(), SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);

      //     m_EntityPBR_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Shadow")->Set(light->GetShadowMap()->GetTexView());

           //

           //

        int b = 5;

        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { mesh->GetBuffer()->GetVertexBuffer() };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Engine::m_pImmediateContext->SetIndexBuffer(mesh->GetBuffer()->GetIndexBuffer(), 0, flags);

        //   return;


        Engine::m_pImmediateContext->SetPipelineState(m_PS_Positions);

        Engine::m_pImmediateContext->CommitShaderResources(m_Positions_SRB, flags);

        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;

        attrib.NumIndices = mesh->GetTris().size() * 3;

        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);

    }

    Engine::m_pImmediateContext->Flush();

}