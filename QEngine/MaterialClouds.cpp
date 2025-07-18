#include "MaterialClouds.h"
#include "QEngine.h"
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Buffer.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
#include <MapHelper.hpp> // Add this line
#include "Texture2D.h"
#include "Texture3D.h"

using namespace Diligent;


struct CloudsConstant {

    glm::mat4 m_MVP;
    glm::mat4 m_Model;
    glm::mat4 m_View;
    glm::mat4 m_Proj;
    glm::vec4 m_CameraPos;
    glm::vec4 m_SunDir;
    glm::vec4 m_VolStart;
    glm::vec4 m_VolSize;
    //glm::vec4 g_ZenithColor;
    //glm::vec4 g_HorizonColor;


};


MaterialClouds::MaterialClouds() {

    SetVertexShader("Engine/Shader/MaterialClouds/clouds.vsh");
    SetPixelShader("Engine/Shader/MaterialClouds/clouds.psh");

    m_UniformBuffer = CreateUniform(sizeof(CloudsConstant), "Clouds - MVP");

    GraphicsPipelineDesc gp;


    RasterizerStateDesc r_desc;


    r_desc.CullMode = CULL_MODE_NONE;



    DepthStencilStateDesc ds_desc;
    ds_desc.DepthEnable = true;
    ds_desc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL;
    ds_desc.DepthWriteEnable = false;


    BlendStateDesc b_desc;

    b_desc.RenderTargets[0].BlendEnable = true;
    b_desc.RenderTargets[0].SrcBlend = BLEND_FACTOR::BLEND_FACTOR_SRC_ALPHA;
    b_desc.RenderTargets[0].DestBlend = BLEND_FACTOR::BLEND_FACTOR_INV_SRC_ALPHA;


    LayoutElement pos;
    LayoutElement color;
    LayoutElement tex;
    LayoutElement norm;
    LayoutElement binorm;
    LayoutElement tangent;

    std::vector<LayoutElement> elements;

    pos.InputIndex = 0;
    pos.NumComponents = 3;
    pos.ValueType = VALUE_TYPE::VT_FLOAT32;
    pos.IsNormalized = false;

    color.InputIndex = 1;
    color.NumComponents = 4;
    color.ValueType = VALUE_TYPE::VT_FLOAT32;
    color.IsNormalized = false;

    tex.InputIndex = 2;
    tex.NumComponents = 3;
    tex.ValueType = VALUE_TYPE::VT_FLOAT32;
    tex.IsNormalized = false;

    norm.InputIndex = 3;
    norm.NumComponents = 3;
    norm.ValueType = VALUE_TYPE::VT_FLOAT32;
    norm.IsNormalized = false;

    binorm.InputIndex = 4;
    binorm.NumComponents = 3;
    binorm.ValueType = VALUE_TYPE::VT_FLOAT32;
    binorm.IsNormalized = false;

    tangent.InputIndex = 5;
    tangent.NumComponents = 3;
    tangent.ValueType = VALUE_TYPE::VT_FLOAT32;
    tangent.IsNormalized = false;


    elements.push_back(pos);
    elements.push_back(color);
    elements.push_back(tex);
    elements.push_back(norm);
    elements.push_back(binorm);
    elements.push_back(tangent);

    InputLayoutDesc in_desc;

    LayoutElement LayoutElems[] =
    {
        // Attribute 0 - vertex position
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        // Attribute 1 - vertex color
        LayoutElement{1, 0, 4, VT_FLOAT32, False},
                LayoutElement{2, 0, 3, VT_FLOAT32, False},
                        LayoutElement{3, 0, 3, VT_FLOAT32, False},
                                LayoutElement{4, 0, 3, VT_FLOAT32, False},
                                        LayoutElement{5, 0, 3, VT_FLOAT32, False},

    };

    in_desc.LayoutElements = LayoutElems;
    in_desc.NumElements = 6;



    gp.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    gp.RasterizerDesc = r_desc;
    //  gp.RasterizerDesc.CullMode = CULL_MODE_NONE;
    gp.DepthStencilDesc = ds_desc;
    gp.SmplDesc.Count = 1;


    //gp.SmplDesc.Quality = 1.0f;

    //gp.NumRenderTargets = 0;

    gp.BlendDesc = b_desc;
    gp.RTVFormats[0] = Q3D::Engine::QEngine::GetSwapChain()->GetDesc().ColorBufferFormat;
    gp.DSVFormat = Q3D::Engine::QEngine::GetSwapChain()->GetDesc().DepthBufferFormat;
    gp.InputLayout = in_desc;
    //gp.NumViewports = 1;


    gp.NumRenderTargets = 1;


    std::vector<ShaderResourceVariableDesc> vars;
    std::vector<ImmutableSamplerDesc> samplers;

    ShaderResourceVariableDesc v_tex;

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_Texture";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    ImmutableSamplerDesc v_sampler;

    SamplerDesc v_rsampler;
    v_rsampler.MinFilter = FILTER_TYPE::FILTER_TYPE_LINEAR;
    v_rsampler.MagFilter = FILTER_TYPE::FILTER_TYPE_LINEAR;
    v_rsampler.MipFilter = FILTER_TYPE::FILTER_TYPE_LINEAR;
    v_rsampler.AddressU = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
    v_rsampler.AddressV = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
    v_rsampler.AddressW = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_CLAMP;
    // v_rsampler.MaxAnisotropy = 1.0f;



    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_Texture";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;


    samplers.push_back(v_sampler);

    PipelineResourceLayoutDesc rl_desc;

    rl_desc.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
    rl_desc.Variables = vars.data();
    rl_desc.ImmutableSamplers = samplers.data();
    rl_desc.NumVariables = 1;

    rl_desc.NumImmutableSamplers = 1;


    PipelineStateDesc pso_desc;

    pso_desc.Name = "Material Clouds3D";
    pso_desc.ResourceLayout = rl_desc;

    //    pso_desc.PipelineType = PIPELINE_TYPE_GRAPHICS;


    GraphicsPipelineStateCreateInfo gp_desc;
    gp_desc.pVS = m_VS;
    gp_desc.pPS = m_PS;
    gp_desc.GraphicsPipeline = gp;
    gp_desc.PSODesc = pso_desc;

    // gp_desc.ResourceSignaturesCount = 0;



     //CreateUniform()

    RefCntAutoPtr<IPipelineState> ps;

    Q3D::Engine::QEngine::GetDevice()->CreateGraphicsPipelineState(gp_desc, &ps);


    m_Pipeline = ps;
    m_Pipeline->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_UniformBuffer);
    m_Pipeline->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Constants")->Set(m_UniformBuffer);
    m_Pipeline->CreateShaderResourceBinding(&m_SRB, true);


    int b = 5;

}

void MaterialClouds::Bind(bool add) {

    //  m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(m_Textures[0]->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
      //Engine::m_pImmediateContext->MapBuffer(BasicUniform, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);

    m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(m_Map->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);


    {
        MapHelper<CloudsConstant> map_data(Q3D::Engine::QEngine::GetContext(), m_UniformBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
        float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);


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

        map_data[0].m_MVP = glm::transpose(mvp);
        map_data[0].m_Model = model;
        map_data[0].m_View = view;
        map_data[0].m_Proj = proj;
        map_data[0].m_CameraPos = glm::vec4(m_CameraPosition, 1.0);
        map_data[0].m_SunDir = glm::vec4(m_SunDir, 1);
       // std::cout << "SY:" << std::to_string(m_SunDir.y) << std::endl;
        map_data[0].m_VolStart = glm::vec4(m_VolStart,0);
        map_data[0].m_VolSize = glm::vec4(m_VolSize, 0);
        //map_data[0].g_SunDir = glm::vec4(m_SunDir,0);




        //   map_data[0].g_ZenithColor = glm::vec4(m_ZenithColor, 0);
       //    map_data[0].g_HorizonColor = glm::vec4(m_HorizonColor, 0);
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

void MaterialClouds::Render() {


    DrawIndexedAttribs attrib;
    attrib.IndexType = VALUE_TYPE::VT_UINT32;

    attrib.NumIndices = m_IndexCount;

    attrib.Flags = DRAW_FLAG_VERIFY_ALL;
    Q3D::Engine::QEngine::GetContext()->DrawIndexed(attrib);


}