#include "pch.h"
#include "MaterialBase.h"
#include "Engine.h"
#include <vector>
#include "Texture2D.h"

#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Buffer.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
#include <MapHelper.hpp> // Add this line
#include "VFile.h"
#include "MaterialMeshPBR.h"
#include "MaterialMeshLight.h"
#include "TextureCube.h"

using namespace Diligent;
MaterialBase::MaterialBase() {


    m_Normal = new Texture2D("engine/normal.png");
    m_Diffuse = new Texture2D("engine/white.png");
    m_Roughness = new Texture2D("engine/white.png");
    m_Metal = new Texture2D("engine/white.png");
    m_Specular = m_Diffuse;
   
    return;
   // Create();
   // m_Diffuse = new Texture2D("test/test1.png");

    m_DiffuseColor = float4(1, 1, 1, 1);
    m_SpecularColor = float4(1, 1, 1, 1);
}

void MaterialBase::Create() {

    return;
    SetVertexShader("basic.vsh");
    SetPixelShader("basic.psh");

    BasicUniform = CreateUniform(sizeof(float4x4),"Basic Uniform Buffer - MVP");

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
                                        LayoutElement(6,0,4,VT_FLOAT32,False),
                                        LayoutElement(7,0,4,VT_FLOAT32,False)
    };

    in_desc.LayoutElements = LayoutElems;
    in_desc.NumElements = 8;
   

    
    gp.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
   gp.RasterizerDesc = r_desc;
  //  gp.RasterizerDesc.CullMode = CULL_MODE_NONE;
    gp.DepthStencilDesc = ds_desc;
    gp.SmplDesc.Count = 4;

    //gp.SmplDesc.Quality = 1.0f;

    //gp.NumRenderTargets = 0;

    gp.BlendDesc = b_desc;
    gp.RTVFormats[0] = Engine::m_pSwapChain->GetDesc().ColorBufferFormat;
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

    pso_desc.Name = "Material Basic";
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
    m_Pipeline->CreateShaderResourceBinding(&m_SRB,true);

    

    //Engine::m_pDevice->CreateBuffer();
}

RefCntAutoPtr<IBuffer> MaterialBase::CreateUniform(int size, std::string path) {

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

void MaterialBase::SetVertexShader(std::string path) {

    const char* filePath = path.c_str();

    // Allocate memory for the Diligent::Char* and copy the string
    size_t length = path.length();
    Char* diligentPath = new Char[length + 1]; // +1 for null terminator
    std::strcpy(diligentPath, filePath);

    ShaderDesc desc;

    desc.Name = diligentPath;
    desc.ShaderType = SHADER_TYPE_VERTEX;
     desc.UseCombinedTextureSamplers = true;
          

	ShaderCreateInfo info;
    info.FilePath = diligentPath;
    info.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    info.Desc = desc;
    info.EntryPoint = "main";
    info.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

	Engine::m_pDevice->CreateShader(info, &m_VS);


}

void MaterialBase::SetPixelShader(std::string path) {

    const char* filePath = path.c_str();

    // Allocate memory for the Diligent::Char* and copy the string
    size_t length = path.length();
    Char* diligentPath = new Char[length + 1]; // +1 for null terminator
    std::strcpy(diligentPath, filePath);

    ShaderDesc desc;

    desc.Name = diligentPath;
    desc.ShaderType = SHADER_TYPE_PIXEL;
    desc.UseCombinedTextureSamplers = true;
    

    ShaderCreateInfo info;
    info.FilePath = diligentPath;
    info.pShaderSourceStreamFactory = Engine::m_pShaderFactory;
    info.Desc = desc;
    info.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    info.EntryPoint = "main";

    Engine::m_pDevice->CreateShader(info, &m_PS);

}

void MaterialBase::Bind(bool sp) {

    m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(m_Diffuse->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
    //Engine::m_pImmediateContext->MapBuffer(BasicUniform, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
    
    MapHelper<float4x4> map_data(Engine::m_pImmediateContext, BasicUniform, MAP_WRITE, MAP_FLAG_DISCARD);
    float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);


   
    
    float4x4 mvp = Engine::m_Camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);
   

    float4x4 view = Engine::m_Camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

    float4x4 model = Engine::m_Node->GetWorldMatrix();

    float4x4 id = float4x4::Identity().Inverse();

    //mvp = mvp*id;

    //mvp.Transpose();

  
    mvp = model*view * mvp;


    map_data[0] = mvp.Transpose();

  //map_data.Unmap();

    Engine::m_pImmediateContext->SetPipelineState(m_Pipeline);

}

void MaterialBase::SetDiffuse(Texture2D* texture) {

    m_Diffuse = texture;
    Updated();
}

void MaterialBase::SetSpecular(Texture2D* texture) {

    m_Specular = texture;
    Updated();
}

void MaterialBase::SetNormals(Texture2D* texture) {

    m_Normal = texture;
    Updated();

}

void MaterialBase::SetEnvironment(RenderTargetCube* env) {

    m_Environment = env;
    Updated();
}

void MaterialBase::SaveMaterial(std::string path) {


        VFile* out = new VFile(path.c_str(), FileMode::Write);

      
        out->WriteString(m_Diffuse->GetPath().c_str());
        out->WriteString(m_Normal->GetPath().c_str());
        out->WriteString(m_Roughness->GetPath().c_str());
        out->WriteString(m_Metal->GetPath().c_str());
        if (m_EnvironmentTex != nullptr) {
            out->WriteInt(1);

            out->WriteString(m_EnvironmentTex->GetPath().c_str());
        }
        else {
            out->WriteInt(0);
        }
        out->WriteVec4(m_DiffuseColor);
        out->WriteVec4(m_SpecularColor);

        out->Close();
        m_FullPath = path;
  
}

MaterialBase* MaterialBase::LoadMaterial(std::string path) {


    VFile* in = new VFile(path.c_str(), FileMode::Read);


    auto res2 = (MaterialMeshPBR*)new MaterialBase;

        res2->SetDiffuse(new Texture2D(in->ReadString(),true));
        res2->SetNormals(new Texture2D(in->ReadString(),true));
        res2->SetRough(new Texture2D(in->ReadString(),true));
        res2->SetMetal(new Texture2D(in->ReadString(),true));
        if (in->ReadInt() == 1)
        {
            res2->SetEnvironmentTex(new TextureCube(in->ReadString()));
        }


        res2->SetDiffuseColor(in->ReadVec4());
        res2->SetSpecularColor(in->ReadVec4());

    

    in->Close();

    
    res2->SetPath(path);

    return res2;

}

void MaterialBase::Updated() {

    //void Updated() {
        Engine::m_MeterialsUpdated = true;
    
}

void MaterialBase::CopyMaps(MaterialBase* other) {

    m_Diffuse = other->GetDiffuse();
    m_Emissive = other->GetEmissive();
    m_Normal = other->GetNormal();
    m_Metal = other->GetMetal();
    m_Roughness = other->GetRough();
    m_Specular = other->GetSpecular();

}