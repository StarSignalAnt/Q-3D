#include "MaterialTerrainDepth.h"
#include "QEngine.h"
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Buffer.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
#include <MapHelper.hpp> // Add this line
#include "Texture2D.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "RenderTargetCube.h"

struct TerrainConstants {

    glm::mat4 v_MVP;
    glm::mat4 v_Proj;
    glm::mat4 v_Model;
    glm::mat4 v_View;
    glm::mat4 v_ModelInv;
    glm::vec4 v_CameraPos;
    glm::vec4 v_LightPos;
    glm::vec4 v_LightProp;
    glm::vec4 v_LightDiff;
    glm::vec4 v_CameraExt;
    int4 v_Layers;

};
struct LightingConstants {

    glm::vec4 v_CameraPos;
    glm::vec4 v_LightPos;
    glm::vec4 v_LightDiff;
    glm::vec4 v_LightRange;
    int4 v_Layers;

};

using namespace Diligent;

MaterialTerrainDepth::MaterialTerrainDepth() {

    SetVertexShader("Engine/Shader/TerrainDepth/mat_terraindepth.vsh");
    SetPixelShader("Engine/Shader/TerrainDepth/mat_terraindepth.psh");


    m_UniformBuffer = CreateUniform(sizeof(TerrainConstants), "Basic Uniform Buffer - MVP");
    //m_LightingBuffer = CreateUniform(sizeof(LightingConstants), "Basic lighting uniform buffer");
    GraphicsPipelineStateCreateInfo PSOCreateInfo;


    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "TerrainDepth PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Q3D::Engine::QEngine::GetSwapChain()->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = Q3D::Engine::QEngine::GetSwapChain()->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on
    BlendStateDesc b_desc;
    b_desc.RenderTargets[0].BlendEnable = true;
    b_desc.RenderTargets[0].SrcBlend = BLEND_FACTOR::BLEND_FACTOR_SRC_ALPHA;
    b_desc.RenderTargets[0].DestBlend = BLEND_FACTOR::BLEND_FACTOR_INV_SRC_ALPHA;
    PSOCreateInfo.GraphicsPipeline.BlendDesc = b_desc;

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;


    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    //m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);





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

    PSOCreateInfo.pVS = m_VS;
    PSOCreateInfo.pPS = m_PS;

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

    Q3D::Engine::QEngine::GetDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &m_Pipeline);


    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly through the pipeline state object.
    m_Pipeline->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_UniformBuffer);
    m_Pipeline->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Constants")->Set(m_UniformBuffer);
 //   m_Pipeline->GetStaticVariableByName(SHADER_TYPE_PIXEL, "LightingConstants")->Set(m_LightingBuffer);



    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
    m_Pipeline->CreateShaderResourceBinding(&m_SRB, true);


}

void MaterialTerrainDepth::Bind(bool add) {


    int index = 0;
    //mvp = mvp*id;
    glm::mat4 proj = m_RenderMatrices[2];
    glm::mat4 view = m_RenderMatrices[0];
    glm::mat4 model = m_RenderMatrices[1];


    glm::mat4 mvp = proj * view * model;
    //mvp.Transpose();




    auto col = m_Textures[0];
    auto norm = m_Textures[1];
    auto spec = m_Textures[2];

   // m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(col->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
   // m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureNorm")->Set(norm->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
   // m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureSpec")->Set(spec->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
   // m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureLayer")->Set(m_Textures[3]->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);

  //  m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Shadow")->Set(m_Light->GetComponent<LightComponent>()->GetShadowMap()->GetTexView());
    //Engine::m_pImmediateContext->MapBuffer(BasicUniform, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
    {
        MapHelper<TerrainConstants> dd(Q3D::Engine::QEngine::GetContext(), m_UniformBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
        float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);

        dd[0].v_MVP = glm::transpose(mvp);
        dd[0].v_Model = glm::transpose(model);
        dd[0].v_ModelInv = glm::transpose(glm::inverse(model));
        dd[0].v_Proj = glm::transpose(proj);
        dd[0].v_View = glm::transpose(view);
        dd[0].v_LightDiff = glm::vec4(m_Light->GetComponent<LightComponent>()->GetColor(), 1.0);
        dd[0].v_LightPos = glm::vec4(m_Light->GetPosition(), 1.0f);
        //  dd.vlightSpec = float4(light->GetSpecular(), 1.0f);

        dd[0].v_LightProp = glm::vec4(m_Light->GetComponent<LightComponent>()->GetRange(), 0, 0, 0);
        //dd[0].v_CameraPos = glm::vec4(camera->GetPosition(), 1.0);
        dd[0].v_Layers = int4(m_Indices[0], 0, 0, 0);
		dd[0].v_CameraPos = glm::vec4(m_CameraPosition, 1.0f);
		dd[0].v_CameraExt = glm::vec4(m_CameraExt.x,m_CameraExt.y, 0.0f,0);

        //MapHelper<TerrainConstants> map_data(Engine::m_pImmediateContext, m_TerrainConstants, MAP_TYPE::MAP_WRITE, MAP_FLAG_DISCARD);


        //float4x4 mvp = Engine::m_Camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


        //float4x4 view = Engine::m_Camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

       // float4x4 model = Engine::m_Node->GetWorldMatrix();

        //float4x4 id = float4x4::Identity().Inverse();

        //mvp = mvp*id;

        //mvp.Transpose();


        //mvp = model * view * mvp;

        glm::mat4 proj = m_RenderMatrices[2];
        glm::mat4 view = m_RenderMatrices[0];
        glm::mat4 model = m_RenderMatrices[1];

        glm::mat4 mvp = proj * view * model;

        // map_data[0] = glm::transpose(mvp);
    }
    {
  
    }
    //map_data.Unmap();


    Uint64 offsets = 0;

    IBuffer* pBuffs[] = { m_Buffers[0] };

    RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

    //     return;
    Q3D::Engine::QEngine::GetContext()->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
    Q3D::Engine::QEngine::GetContext()->SetIndexBuffer(m_Buffers[1], 0, flags);


    //   return;


    Q3D::Engine::QEngine::GetContext()->SetPipelineState(m_Pipeline);

    Q3D::Engine::QEngine::GetContext()->CommitShaderResources(m_SRB, flags);
    //Vivid::m_pImmediateContext->SetPipelineState(m_Pipeline);


}

void MaterialTerrainDepth::Render() {



    DrawIndexedAttribs attrib;
    attrib.IndexType = VALUE_TYPE::VT_UINT32;

    attrib.NumIndices = m_IndexCount;

    attrib.Flags = DRAW_FLAG_VERIFY_ALL;
    Q3D::Engine::QEngine::GetContext()->DrawIndexed(attrib);

}