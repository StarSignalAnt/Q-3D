#include "PSOBuilder.h"
#include "PSOBuilder.h"


// Constructor and other methods remain the same...
PSOBuilder::PSOBuilder()
{
    DefaultsFor3D();
}

PSOBuilder& PSOBuilder::WithShaders(
    Diligent::IRenderDevice* pDevice,
    Diligent::IShaderSourceInputStreamFactory* pShaderSourceFactory,
    const char* VSPath,
    const char* PSPath)
{
    Diligent::ShaderCreateInfo ShaderCI;
    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
    ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
   // ShaderCI.UseCombinedTextureSamplers = true;

    // Create Vertex Shader from file
    ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
    ShaderCI.Desc.UseCombinedTextureSamplers = true;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = VSPath;
    ShaderCI.FilePath = VSPath;
    // ✨ FIX: Store the created shader in the member variable m_pVS
    pDevice->CreateShader(ShaderCI, &m_pVS);
    if (!m_pVS) {
        // Handle error: Vertex shader failed to compile
        return *this;
    }

    // Create Pixel Shader from file
    ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = PSPath;
    ShaderCI.FilePath = PSPath;
    // ✨ FIX: Store the created shader in the member variable m_pPS
    pDevice->CreateShader(ShaderCI, &m_pPS);
    if (!m_pPS) {
        // Handle error: Pixel shader failed to compile
        return *this;
    }

    // Now assign the valid, non-dangling raw pointers
    m_PSOCreateInfo.pVS = m_pVS;
    m_PSOCreateInfo.pPS = m_pPS;

    return *this;
}


PSOBuilder& PSOBuilder::SetName(const char* Name)
{
    m_PSOCreateInfo.PSODesc.Name = Name;
    return *this;
}

PSOBuilder& PSOBuilder::WithLayout(VertexLayoutType LayoutType)
{
    // The InputLayoutDesc holds a pointer to the layout elements, so they must have static storage duration.
    static Diligent::LayoutElement Normal3DLayoutElems[6];

    Diligent::InputLayoutDesc Layout;

    switch (LayoutType)
    {
    case VertexLayoutType::Normal2D:
    {
        static Diligent::LayoutElement Normal2DLayoutElems[4];

        Normal2DLayoutElems[0] = { 0, 0, 3, Diligent::VT_FLOAT32, false }; // Attribute 0: Vertex Position
        Normal2DLayoutElems[1] = { 1, 0, 4, Diligent::VT_FLOAT32, false }; // Attribute 1: Vertex Color
        Normal2DLayoutElems[2] = { 2, 0, 3, Diligent::VT_FLOAT32, false }; // Attribute 2: Normal
        Normal2DLayoutElems[3] = { 3, 0, 4, Diligent::VT_FLOAT32, false }; // Attribute 3: Tangent
        Layout.LayoutElements = Normal2DLayoutElems;
        Layout.NumElements = 4;

    }
        break;
    case VertexLayoutType::Normal3D:
    {
        // As per your definition for a general 3D mesh vertex
        Normal3DLayoutElems[0] = { 0, 0, 3, Diligent::VT_FLOAT32, false }; // Attribute 0: Vertex Position
        Normal3DLayoutElems[1] = { 1, 0, 4, Diligent::VT_FLOAT32, false }; // Attribute 1: Vertex Color
        Normal3DLayoutElems[2] = { 2, 0, 3, Diligent::VT_FLOAT32, false }; // Attribute 2: Normal
        Normal3DLayoutElems[3] = { 3, 0, 3, Diligent::VT_FLOAT32, false }; // Attribute 3: Tangent
        Normal3DLayoutElems[4] = { 4, 0, 3, Diligent::VT_FLOAT32, false }; // Attribute 4: Binormal
        Normal3DLayoutElems[5] = { 5, 0, 3, Diligent::VT_FLOAT32, false }; // Attribute 5: TexCoord (UV)

        Layout.LayoutElements = Normal3DLayoutElems;
        Layout.NumElements = 6;
        break;
    }

    // Add cases for other layouts (e.g., SkinnedMesh) here in the future
    }

    m_PSOCreateInfo.GraphicsPipeline.InputLayout = Layout;
    return *this;
}
PSOBuilder& PSOBuilder::WithResourceLayout(LayoutResourceType Type)
{
  

 

    Diligent::PipelineResourceLayoutDesc ResourceLayout;

    switch (Type)
    {
    case LayoutResourceType::Material2D:
    {
        static Diligent::ImmutableSamplerDesc PBRSamplers[1];
        static Diligent::ShaderResourceVariableDesc PBRVars[] = {
          {Diligent::SHADER_TYPE_PIXEL, "v_Texture", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
        };

        Diligent::SamplerDesc SamplerLinearWrapDesc{
            Diligent::FILTER_TYPE_LINEAR, Diligent::FILTER_TYPE_LINEAR, Diligent::FILTER_TYPE_LINEAR,
            Diligent::TEXTURE_ADDRESS_WRAP, Diligent::TEXTURE_ADDRESS_WRAP, Diligent::TEXTURE_ADDRESS_CLAMP
        };

        for (int i = 0; i < 1; ++i) {
            PBRSamplers[i] = { PBRVars[i].ShaderStages, PBRVars[i].Name, SamplerLinearWrapDesc };
        }

        ResourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
        ResourceLayout.Variables = PBRVars;
        ResourceLayout.NumVariables = _countof(PBRVars);
        ResourceLayout.ImmutableSamplers = PBRSamplers;
        ResourceLayout.NumImmutableSamplers = _countof(PBRSamplers);

    }
        break;
    case LayoutResourceType::PBRMaterial:
    {
        static Diligent::ImmutableSamplerDesc PBRSamplers[8];
        // These arrays must be static as the PSO description only stores a pointer to them.
        static Diligent::ShaderResourceVariableDesc PBRVars[] = {
            {Diligent::SHADER_TYPE_PIXEL, "v_Texture", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
            {Diligent::SHADER_TYPE_PIXEL, "v_TextureNormal", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
            {Diligent::SHADER_TYPE_PIXEL, "v_TextureMetal", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
            {Diligent::SHADER_TYPE_PIXEL, "v_TextureRough", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
            {Diligent::SHADER_TYPE_PIXEL, "v_TextureEnvironment", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
            {Diligent::SHADER_TYPE_PIXEL, "v_TextureShadow", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
            {Diligent::SHADER_TYPE_PIXEL, "v_TextureDirShadow", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
            {Diligent::SHADER_TYPE_PIXEL, "v_TextureHeight", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
        };

        Diligent::SamplerDesc SamplerLinearWrapDesc{
            Diligent::FILTER_TYPE_LINEAR, Diligent::FILTER_TYPE_LINEAR, Diligent::FILTER_TYPE_LINEAR,
            Diligent::TEXTURE_ADDRESS_WRAP, Diligent::TEXTURE_ADDRESS_WRAP, Diligent::TEXTURE_ADDRESS_CLAMP
        };

        for (int i = 0; i < 8; ++i) {
            PBRSamplers[i] = { PBRVars[i].ShaderStages, PBRVars[i].Name, SamplerLinearWrapDesc };
        }

        ResourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
        ResourceLayout.Variables = PBRVars;
        ResourceLayout.NumVariables = _countof(PBRVars);
        ResourceLayout.ImmutableSamplers = PBRSamplers;
        ResourceLayout.NumImmutableSamplers = _countof(PBRSamplers);
        break;
    }
    // Add other resource layout types here in the future
    }

    m_PSOCreateInfo.PSODesc.ResourceLayout = ResourceLayout;
    return *this;
}


PSOBuilder& PSOBuilder::WithInputLayout(const Diligent::InputLayoutDesc& Layout)
{
    m_PSOCreateInfo.GraphicsPipeline.InputLayout = Layout;
    return *this;
}

PSOBuilder& PSOBuilder::WithPrimitiveTopology(Diligent::PRIMITIVE_TOPOLOGY Topology)
{
    m_PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = Topology;
    return *this;
}

PSOBuilder& PSOBuilder::WithRasterizerState(const Diligent::RasterizerStateDesc& Desc)
{
    m_PSOCreateInfo.GraphicsPipeline.RasterizerDesc = Desc;
    return *this;
}

PSOBuilder& PSOBuilder::WithDepthStencilState(const Diligent::DepthStencilStateDesc& Desc)
{
    m_PSOCreateInfo.GraphicsPipeline.DepthStencilDesc = Desc;
    return *this;
}

PSOBuilder& PSOBuilder::WithBlendState(const Diligent::BlendStateDesc& Desc)
{
    m_PSOCreateInfo.GraphicsPipeline.BlendDesc = Desc;
    return *this;
}

PSOBuilder& PSOBuilder::WithNumRenderTargets(Diligent::Uint8 NumRTVs)
{
    m_PSOCreateInfo.GraphicsPipeline.NumRenderTargets = NumRTVs;
    return *this;
}

PSOBuilder& PSOBuilder::WithRTVFormat(Diligent::Uint8 RTVIndex, Diligent::TEXTURE_FORMAT Format)
{
    m_PSOCreateInfo.GraphicsPipeline.RTVFormats[RTVIndex] = Format;
    return *this;
}

PSOBuilder& PSOBuilder::WithDSVFormat(Diligent::TEXTURE_FORMAT Format)
{
    m_PSOCreateInfo.GraphicsPipeline.DSVFormat = Format;
    return *this;
}

PSOBuilder& PSOBuilder::DefaultsFor3D()
{
    m_PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_BACK;
    m_PSOCreateInfo.GraphicsPipeline.RasterizerDesc.FillMode = Diligent::FILL_MODE_SOLID;
    m_PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
    m_PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    return *this;
}

PSOBuilder& PSOBuilder::DefaultsForTransparent()
{
    Diligent::BlendStateDesc BSDesc;
    BSDesc.RenderTargets[0].BlendEnable = true;
    BSDesc.RenderTargets[0].SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
    BSDesc.RenderTargets[0].DestBlend = Diligent::BLEND_FACTOR_INV_SRC_ALPHA;
    WithBlendState(BSDesc);

    // Transparent objects should be tested against the depth buffer but not write to it.
    m_PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = false;

    return *this;
}

PSOBuilder& PSOBuilder::DefaultsForUI()
{
    // UI is typically 2D, so no depth testing or culling is needed.
    m_PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = false;
    m_PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;

    // Enable alpha blending for UI elements
    Diligent::BlendStateDesc BSDesc;
    BSDesc.RenderTargets[0].BlendEnable = true;
    BSDesc.RenderTargets[0].SrcBlend = Diligent::BLEND_FACTOR_SRC_ALPHA;
    BSDesc.RenderTargets[0].DestBlend = Diligent::BLEND_FACTOR_INV_SRC_ALPHA;
    WithBlendState(BSDesc);

    return *this;
}


Diligent::RefCntAutoPtr<Diligent::IPipelineState> PSOBuilder::Build(Diligent::IRenderDevice* pDevice)
{
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pPSO;
    pDevice->CreateGraphicsPipelineState(m_PSOCreateInfo, &pPSO);
    return pPSO;
}
