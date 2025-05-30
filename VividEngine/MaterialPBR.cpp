#include "MaterialPBR.h"
#include "Vivid.h"
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Buffer.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
#include <MapHelper.hpp> // Add this line
#include "Texture2D.h"
#include "TextureCube.h"
#include "SceneGraph.h"
#include "LightComponent.h"
#include "GraphNode.h"
#include "RenderTargetCube.h"

using namespace Diligent;

struct PBRConstant {

    // === TRANSFORMATION MATRICES ===
    glm::mat4 g_ModelMatrix;        // World transform
    glm::mat4 g_ViewMatrix;         // Camera view
    glm::mat4 g_ProjectionMatrix;   // Projection
    glm::mat4 g_MVPMatrix;          // Combined Model-View-Projection
    glm::mat4 g_NormalMatrix;       // Inverse transpose of model matrix for normals

    // === CAMERA ===
    glm::vec4 g_CameraPosition;       // .xyz = world-space camera position, .w unused


    // === LIGHTING: Point Light ===
    glm::vec4 g_LightPosition;        // .xyz = position, .w unused
    glm::vec4 g_LightColor;           // .rgb = color, .a unused
    glm::vec4 g_LightIntensity;       // .x = intensity multiplier, .yzw unused
    glm::vec4 g_LightRange;           // .x = effective range, .yzw unused
    glm::vec4 g_ToneParams;
    glm::vec4 g_AmbientColor;
};

MaterialPBR::MaterialPBR() {

    m_ColorTexture = new Texture2D("Engine/Maps/White.png");
	m_MetallicTexture = new Texture2D("Engine/Maps/White.png");
	m_RoughnessTexture = new Texture2D("Engine/Maps/White.png");
	m_NormalTexture = new Texture2D("Engine/Maps/Normal.png");
	m_EnvironmentMap = new TextureCube("Engine/Maps/blackcm.tex");
	m_HeightTexture = new Texture2D("Engine/Maps/Black.png");
	//m_BRDF = new Texture2D("Engine/Maps/BRDF.png");


    SetVertexShader("Engine/Shader/PBR/pbr.vsh");
    SetPixelShader("Engine/Shader/PBR/pbr.psh");

    m_UniformBuffer = CreateUniform(sizeof(PBRConstant), "Basic Uniform Buffer - MVP");

    GraphicsPipelineDesc gp;


    RasterizerStateDesc r_desc;


    r_desc.CullMode = CULL_MODE_BACK;



    DepthStencilStateDesc ds_desc;
    ds_desc.DepthEnable = true;
    ds_desc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL;
    ds_desc.DepthWriteEnable = true;


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
    gp.RTVFormats[0] = Vivid::m_pSwapChain->GetDesc().ColorBufferFormat;
    gp.DSVFormat = Vivid::m_pSwapChain->GetDesc().DepthBufferFormat;
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

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureNormal";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureMetal";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureRough";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureEnvironment";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureShadow";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureHeight";
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

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureNormal";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;


    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureMetal";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;


    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureRough";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;


    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureEnvironment";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;


    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureShadow";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;



    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureHeight";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;



    samplers.push_back(v_sampler);

    PipelineResourceLayoutDesc rl_desc;

    rl_desc.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
    rl_desc.Variables = vars.data();
    rl_desc.ImmutableSamplers = samplers.data();
    rl_desc.NumVariables = 7;

    rl_desc.NumImmutableSamplers = 7;


    PipelineStateDesc pso_desc;

    pso_desc.Name = "Material PBR";
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

    Vivid::m_pDevice->CreateGraphicsPipelineState(gp_desc, &ps);

    m_Pipeline = ps;
    m_Pipeline->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_UniformBuffer);
	m_Pipeline->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Constants")->Set(m_UniformBuffer);  
    m_Pipeline->CreateShaderResourceBinding(&m_SRB, true);


    //Additive

//    GraphicsPipelineDesc gp;


//    RasterizerStateDesc r_desc;


    r_desc.CullMode = CULL_MODE_BACK;



  //  DepthStencilStateDesc ds_desc;
    ds_desc.DepthEnable = true;
    ds_desc.DepthFunc = COMPARISON_FUNC_EQUAL;
    ds_desc.DepthWriteEnable = false;


    //BlendStateDesc b_desc;


    b_desc.RenderTargets[0].BlendEnable = true;
    b_desc.RenderTargets[0].SrcBlend = BLEND_FACTOR::BLEND_FACTOR_ONE;
    b_desc.RenderTargets[0].DestBlend = BLEND_FACTOR::BLEND_FACTOR_ONE;


 

    elements.clear();
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
    gp.RTVFormats[0] = Vivid::m_pSwapChain->GetDesc().ColorBufferFormat;
    gp.DSVFormat = Vivid::m_pSwapChain->GetDesc().DepthBufferFormat;
    gp.InputLayout = in_desc;
    //gp.NumViewports = 1;


    gp.NumRenderTargets = 1;


    vars.clear();
    samplers.clear();

    //std::vector<ShaderResourceVariableDesc> vars;
    //std::vector<ImmutableSamplerDesc> samplers;

    //ShaderResourceVariableDesc v_tex;

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_Texture";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureNormal";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureMetal";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureRough";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureEnvironment";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureShadow";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

    v_tex.ShaderStages = SHADER_TYPE_PIXEL;
    v_tex.Name = "v_TextureHeight";
    v_tex.Type = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
    vars.push_back(v_tex);

 //   ImmutableSamplerDesc v_sampler;

  //  SamplerDesc v_rsampler;
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

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureNormal";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;


    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureMetal";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;


    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureRough";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;


    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureEnvironment";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;


    samplers.push_back(v_sampler);


    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureShadow";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;



    samplers.push_back(v_sampler);

    v_sampler.Desc = v_rsampler;
    v_sampler.SamplerOrTextureName = "v_TextureHeight";
    v_sampler.ShaderStages = SHADER_TYPE_PIXEL;



    samplers.push_back(v_sampler);

//    PipelineResourceLayoutDesc rl_desc;

    rl_desc.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
    rl_desc.Variables = vars.data();
    rl_desc.ImmutableSamplers = samplers.data();
    rl_desc.NumVariables = 7;

    rl_desc.NumImmutableSamplers = 7;


    //PipelineStateDesc pso_desc;

    pso_desc.Name = "Material PBR";
    pso_desc.ResourceLayout = rl_desc;

    //    pso_desc.PipelineType = PIPELINE_TYPE_GRAPHICS;


 //   GraphicsPipelineStateCreateInfo gp_desc;
    gp_desc.pVS = m_VS;
    gp_desc.pPS = m_PS;
    gp_desc.GraphicsPipeline = gp;
    gp_desc.PSODesc = pso_desc;
    // gp_desc.ResourceSignaturesCount = 0;



     //CreateUniform()

    RefCntAutoPtr<IPipelineState> ps2;


    Vivid::m_pDevice->CreateGraphicsPipelineState(gp_desc, &ps2);

    m_PipelineAdd = ps2;
    m_PipelineAdd->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_UniformBuffer);
    m_PipelineAdd->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Constants")->Set(m_UniformBuffer);
    m_PipelineAdd->CreateShaderResourceBinding(&m_SRBAdd, true);



    int b = 5;

}

void MaterialPBR::Bind(bool add) {

    //auto l1 = SceneGraph::m_CurrentGraph->GetLights()[0];

	//auto lc = l1->GetComponent<LightComponent>();

    if (add) {

        auto l1 = m_Light;
        auto lc = l1->GetComponent<LightComponent>();

        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(m_ColorTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureNormal")->Set(m_NormalTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureMetal")->Set(m_MetallicTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureRough")->Set(m_RoughnessTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureEnvironment")->Set(m_EnvironmentMap->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
		m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureShadow")->Set(m_Light->GetComponent<LightComponent>()->GetShadowMap()->GetTexView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureHeight")->Set(m_HeightTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);

        //Engine::m_pImmediateContext->MapBuffer(BasicUniform, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
        {
            MapHelper<PBRConstant> map_data(Vivid::m_pImmediateContext, m_UniformBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
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

            glm::mat4 mvp = proj * model * view;

            //    map_data[0].g_MVPMatrix = glm::transpose(mvp);
            //	map_data[0].g_ModelMatrix = glm::transpose(model);
            map_data[0].g_ProjectionMatrix = glm::transpose(proj);
            map_data[0].g_ViewMatrix = glm::transpose(view);
            //    map_data[0].g_NormalMatrix = glm::transpose(glm::inverse(model)); // Inverse transpose for normals
            map_data[0].g_CameraPosition = glm::vec4(m_CameraPosition, 1.0f);

            map_data[0].g_LightPosition = glm::vec4(l1->GetPosition(), 1.0f); // Light position
            map_data[0].g_LightColor = glm::vec4(lc->GetColor(), 1.0f); // White light color
            map_data[0].g_LightIntensity = glm::vec4(lc->GetIntensity(), 1.0f, 1.0f, 1.0f); // Was 10000!

            // Also try removing transposes (test one at a time):
            map_data[0].g_MVPMatrix = glm::transpose(mvp); // Remove transpose
            map_data[0].g_ModelMatrix = glm::transpose(model); // Remove transpose 
            map_data[0].g_NormalMatrix = glm::transpose(glm::inverse(model)); // Remove transpose
            map_data[0].g_LightRange = glm::vec4(lc->GetRange(), 100.f, 100.f, 1.f); // Light range
            map_data[0].g_ToneParams = glm::vec4(1.0f, 2.2f, 0.05f, 1.0f); // Tone mapping parameters (example values)
            map_data[0].g_AmbientColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Ambient color
        }



        //map_data.Unmap();


        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { m_Buffers[0] };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Vivid::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Vivid::m_pImmediateContext->SetIndexBuffer(m_Buffers[1], 0, flags);


        //   return;


        Vivid::m_pImmediateContext->SetPipelineState(m_PipelineAdd);

        Vivid::m_pImmediateContext->CommitShaderResources(m_SRBAdd, flags);
    }
    else {
        auto l1 = m_Light;
        auto lc = l1->GetComponent<LightComponent>();

        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(m_ColorTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureNormal")->Set(m_NormalTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureMetal")->Set(m_MetallicTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureRough")->Set(m_RoughnessTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureEnvironment")->Set(m_EnvironmentMap->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureShadow")->Set(m_Light->GetComponent<LightComponent>()->GetShadowMap()->GetTexView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureHeight")->Set(m_HeightTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);


        //Engine::m_pImmediateContext->MapBuffer(BasicUniform, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
        {
            MapHelper<PBRConstant> map_data(Vivid::m_pImmediateContext, m_UniformBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
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

            glm::mat4 mvp = proj * model * view;

            //    map_data[0].g_MVPMatrix = glm::transpose(mvp);
            //	map_data[0].g_ModelMatrix = glm::transpose(model);
            map_data[0].g_ProjectionMatrix = glm::transpose(proj);
            map_data[0].g_ViewMatrix = glm::transpose(view);
            //    map_data[0].g_NormalMatrix = glm::transpose(glm::inverse(model)); // Inverse transpose for normals
            map_data[0].g_CameraPosition = glm::vec4(m_CameraPosition, 1.0f);

            map_data[0].g_LightPosition = glm::vec4(l1->GetPosition(), 1.0f); // Light position
            map_data[0].g_LightColor = glm::vec4(lc->GetColor(), 1.0f); // White light color
            map_data[0].g_LightIntensity = glm::vec4(lc->GetIntensity(), 1.0f, 1.0f, 1.0f); // Was 10000!

            // Also try removing transposes (test one at a time):
            map_data[0].g_MVPMatrix = glm::transpose(mvp); // Remove transpose
            map_data[0].g_ModelMatrix = glm::transpose(model); // Remove transpose 
            map_data[0].g_NormalMatrix = glm::transpose(glm::inverse(model)); // Remove transpose
            map_data[0].g_LightRange = glm::vec4(lc->GetRange(), 100.f, 100.f, 1.f); // Light range
            map_data[0].g_ToneParams = glm::vec4(1.0f, 2.2f, 0.05f, 1.0f); // Tone mapping parameters (example values)
            map_data[0].g_AmbientColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Ambient color
        }



        //map_data.Unmap();


        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { m_Buffers[0] };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Vivid::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Vivid::m_pImmediateContext->SetIndexBuffer(m_Buffers[1], 0, flags);


        //   return;


        Vivid::m_pImmediateContext->SetPipelineState(m_Pipeline);

        Vivid::m_pImmediateContext->CommitShaderResources(m_SRB, flags);
        //Vivid::m_pImmediateContext->SetPipelineState(m_Pipeline);
    }

}

void MaterialPBR::Render() {

    DrawIndexedAttribs attrib;
    attrib.IndexType = VALUE_TYPE::VT_UINT32;

    attrib.NumIndices = m_IndexCount;

    attrib.Flags = DRAW_FLAG_VERIFY_ALL;
    Vivid::m_pImmediateContext->DrawIndexed(attrib);

}