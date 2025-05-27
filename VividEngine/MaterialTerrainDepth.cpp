#include "pch.h"
#include "MaterialTerrainDepth.h"
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
using namespace Diligent;


struct Constants {

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


MaterialTerrainDepth::MaterialTerrainDepth() {


    return;
    Create();

   // m_Diffuse = new Texture2D("edit/blank_layer.png");
   // m_Normal = new Texture2D("engine/normal.png");
   // m_Specular = new Texture2D("engine/white.png");
   // m_Metal = new Texture2D("engine/white.png");
   // m_Roughness = new Texture2D("engine/grey.png");
   // m_Ambient = new Texture2D("engine/white.png");
}

void MaterialTerrainDepth::Create() {

    //printf("Creating MeshLight.\n");
    SetVertexShader("mat_terraindepth.vsh");
    SetPixelShader("mat_terraindepth.psh");
    BasicUniform = CreateUniform(sizeof(Constants), "Constants Uniform Buffer - MVP");
    BasicUniform2 = CreateUniform(sizeof(Constants), "Constants Uniform Buffer - MVP");
    GraphicsPipelineDesc gp;




    RasterizerStateDesc r_desc;



    r_desc.CullMode = CULL_MODE_BACK;



    DepthStencilStateDesc ds_desc;
    ds_desc.DepthEnable = true;
    ds_desc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL;
    ds_desc.DepthWriteEnable = true;


    BlendStateDesc b_desc;

    b_desc.RenderTargets[0].BlendEnable = false;
    b_desc.RenderTargets[0].SrcBlend = BLEND_FACTOR::BLEND_FACTOR_SRC_ALPHA;
    b_desc.RenderTargets[0].DestBlend = BLEND_FACTOR::BLEND_FACTOR_INV_SRC_ALPHA;


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
                                        LayoutElement{6,0,3,VT_FLOAT32,False}

    };

    in_desc.LayoutElements = LayoutElems;
    in_desc.NumElements = 7;



    gp.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    gp.RasterizerDesc = r_desc;
    //  gp.RasterizerDesc.CullMode = CULL_MODE_NONE;
    gp.DepthStencilDesc = ds_desc;
    gp.SmplDesc.Count = 4;

    //gp.SmplDesc.Quality = 1.0f;

    //gp.NumRenderTargets = 0;

    gp.BlendDesc = b_desc;
    gp.RTVFormats[0] = gp.RTVFormats[0] = TEX_FORMAT_R32_FLOAT;// Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
    gp.DSVFormat = Engine::m_pSwapChain->GetDesc().DepthBufferFormat;
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

    ShaderResourceVariableDesc v_norm;

    v_norm.ShaderStages = SHADER_TYPE_PIXEL;
    v_norm.Name = "v_TextureNorm";
    v_norm.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_norm);

    ShaderResourceVariableDesc v_rough;

    v_rough.ShaderStages = SHADER_TYPE_PIXEL;
    v_rough.Name = "v_TextureSpec";
    v_rough.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_rough);

    ShaderResourceVariableDesc v_metal;

    v_metal.ShaderStages = SHADER_TYPE_PIXEL;
    v_metal.Name = "v_TextureLayer";
    v_metal.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_metal);

    ShaderResourceVariableDesc v_amb;

    v_amb.ShaderStages = SHADER_TYPE_PIXEL;
    v_amb.Name = "v_Shadow";
    v_amb.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_amb);


    ImmutableSamplerDesc v_sampler;

    SamplerDesc v_rsampler;
    v_rsampler.MinFilter = FILTER_TYPE::FILTER_TYPE_LINEAR;
    v_rsampler.MagFilter = FILTER_TYPE::FILTER_TYPE_LINEAR;
    v_rsampler.MipFilter = FILTER_TYPE::FILTER_TYPE_LINEAR;
    v_rsampler.AddressU = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
    v_rsampler.AddressV = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
    v_rsampler.AddressW = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
    // v_rsampler.MaxAnisotropy = 1.0f;



    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_Texture";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;


    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureNorm";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;

    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureSpec";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;

    samplers.push_back(v_sampler);


    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureLayer";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;

    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_Shadow";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;

    samplers.push_back(v_sampler);



    PipelineResourceLayoutDesc rl_desc;

    rl_desc.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
    rl_desc.Variables = vars.data();
    rl_desc.ImmutableSamplers = samplers.data();
    rl_desc.NumVariables = 5;

    rl_desc.NumImmutableSamplers = 5;


    PipelineStateDesc pso_desc;

    pso_desc.Name = "Material Terrain PBR";
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

    Engine::m_pDevice->CreateGraphicsPipelineState(gp_desc, &ps);

    m_Pipeline = ps;
    m_Pipeline->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(BasicUniform);
    m_Pipeline->CreateShaderResourceBinding(&m_SRB, true);




    b_desc.RenderTargets[0].BlendEnable = true;
    b_desc.RenderTargets[0].SrcBlend = BLEND_FACTOR::BLEND_FACTOR_ONE;
    b_desc.RenderTargets[0].DestBlend = BLEND_FACTOR::BLEND_FACTOR_ONE;
    gp.BlendDesc = b_desc;
    gp_desc.GraphicsPipeline = gp;


    RefCntAutoPtr<IPipelineState> ps2;


    Engine::m_pDevice->CreateGraphicsPipelineState(gp_desc, &ps2);

    m_SecondPassPipeline = ps2;
    m_SecondPassPipeline->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(BasicUniform2);
    m_SecondPassPipeline->CreateShaderResourceBinding(&m_SecondPassSRB, true);


}

void MaterialTerrainDepth::Bind(bool sp) {

    if (sp) {
        m_SecondPassSRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(m_Diffuse->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SecondPassSRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureNorm")->Set(m_Normal->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        //m_SecondPassSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureRough")->Set(m_Roughness->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        //m_SecondPassSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureMetal")->Set(m_Metal->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        //m_SecondPassSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureAmbient")->Set(m_Ambient->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SecondPassSRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Shadow")->Set(Engine::m_Light->GetShadowMap()->GetTexView(), SET_SHADER_RESOURCE_FLAG_NONE);
        if (m_EnvironmentTex != nullptr) {
            //    m_SecondPassSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Environment")->Set(m_EnvironmentTex->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        }
        else
            if (m_Environment != nullptr) {
                //      m_SecondPassSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Environment")->Set(m_Environment->GetTexView(), SET_SHADER_RESOURCE_FLAG_NONE);
            }
    }
    else {
        //m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(m_Diffuse->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
       // m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureNorm")->Set(m_Normal->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
     //   m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureLayer")->Set(m_LayerMap->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        // m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureRough")->Set(m_Roughness->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        // m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureMetal")->Set(m_Metal->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
         //m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureAmbient")->Set(m_Ambient->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
    //     m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Shadow")->Set(Engine::m_Light->GetShadowMaep()->GetTexView(), SET_SHADER_RESOURCE_FLAG_NONE);
        if (m_EnvironmentTex != nullptr) {
            //   m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Environment")->Set(m_EnvironmentTex->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        }
        else
            if (m_Environment != nullptr) {
                //     m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Environment")->Set(m_Environment->GetTexView(), SET_SHADER_RESOURCE_FLAG_NONE);
            }
    }
    //  m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureSpec")->Set(m_Specular->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
      //Engine::m_pImmediateContext->MapBuffer(BasicUniform, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);



    if (sp) {

        MapHelper<Constants> map_data(Engine::m_pImmediateContext, BasicUniform2, MAP_WRITE, MAP_FLAG_DISCARD);
        float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);


        auto light = Engine::m_Light;

        float4x4 mvp = Engine::m_Camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


        float4x4 view = Engine::m_Camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

        float4x4 model = Engine::m_Node->GetWorldMatrix();

        float4x4 id = float4x4::Identity().Inverse();

        //mvp = mvp*id;

        //mvp.Transpose();


        mvp = model * view * mvp;

        map_data[0].v_MVP = mvp.Transpose();
        map_data[0].v_Model = model.Transpose();
        map_data[0].v_ModelInv = model.Inverse().Transpose();
        map_data[0].v_Proj = Engine::m_Camera->GetProjection().Transpose();
        map_data[0].v_View = view.Transpose();
        map_data[0].v_LightDiff = float4(light->GetDiffuse(), 0);
        map_data[0].v_LightPos = float4(light->GetPosition(), 1.0f);
        //  map_data[0].lightSpec = float4(light->GetSpecular(), 1.0f);

        map_data[0].v_CameraPos = float4(Engine::m_Camera->GetPosition(), 1.0);



        int env = 0;
        if (m_Environment != nullptr || m_EnvironmentTex != nullptr) {
            env = 1.0;
        }

        //   map_data[0].v_ExtraProp = float4(light->GetRange(), env,0,0);
       //    map_data[0].v_ViewDir = float4(Engine::m_Camera->TransformVector(float3(0, 0, -1.0)), 1.0);

    }

    else {
        MapHelper<Constants> map_data(Engine::m_pImmediateContext, BasicUniform, MAP_WRITE, MAP_FLAG_DISCARD);
        float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);


        auto light = Engine::m_Light;

        float4x4 mvp = Engine::m_Camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


        float4x4 view = Engine::m_Camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

        float4x4 model = Engine::m_Node->GetWorldMatrix();

        float4x4 id = float4x4::Identity().Inverse();

        //mvp = mvp*id;

        //mvp.Transpose();


        mvp = model * view * mvp;


        map_data[0].v_MVP = mvp.Transpose();
        map_data[0].v_Model = model.Transpose();
        map_data[0].v_ModelInv = model.Inverse().Transpose();
        map_data[0].v_Proj = Engine::m_Camera->GetProjection().Transpose();
        map_data[0].v_View = view.Transpose();
        map_data[0].v_LightDiff = float4(light->GetDiffuse(), 0);
        map_data[0].v_LightPos = float4(light->GetPosition(), 1.0f);
        map_data[0].v_CameraExt = float4(Engine::m_Camera->GetNearZ(), Engine::m_Camera->GetFarZ(), 0, 0);
        //  map_data[0].lightSpec = float4(light->GetSpecular(), 1.0f);

        map_data[0].v_CameraPos = float4(Engine::m_Camera->GetPosition(), 1.0);



        int env = 0;
        if (m_Environment != nullptr || m_EnvironmentTex != nullptr) {
            env = 1.0;
        }



    }


    //map_data.Unmap();

    if (sp) {
        Engine::m_pImmediateContext->SetPipelineState(m_SecondPassPipeline);
    }
    else {
        Engine::m_pImmediateContext->SetPipelineState(m_Pipeline);
    }
}

