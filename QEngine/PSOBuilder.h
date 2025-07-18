#pragma once
#include "QEngine.h"
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
#include "VFile.h"
#include "RenderTarget2D.h"


// Forward declaration
namespace Diligent
{
    struct InputLayoutDesc;
}

// Forward declaration
namespace Diligent
{
    struct InputLayoutDesc;
}

enum class VertexLayoutType
{
    Normal3D,Normal2D
    // You can add other layouts like SkinnedMesh, PostProcess, etc. here
};
enum class LayoutResourceType
{
    PBRMaterial,Material2D,MaterialYUV
};


class PSOBuilder
{
public:
    PSOBuilder();

    // --- Core Pipeline Components ---
    PSOBuilder& SetName(const char* Name);
    PSOBuilder& WithShaders(
        Diligent::IRenderDevice* pDevice,
        Diligent::IShaderSourceInputStreamFactory* pShaderSourceFactory,
        const char* VSPath,
        const char* PSPath
    );
    PSOBuilder& WithLayout(VertexLayoutType LayoutType);
    PSOBuilder& WithInputLayout(const Diligent::InputLayoutDesc& Layout);
    PSOBuilder& WithResourceLayout(LayoutResourceType Type);
    PSOBuilder& WithCustomResourceLayout(const Diligent::PipelineResourceLayoutDesc& Desc);

    // --- State Configuration ---
    PSOBuilder& WithPrimitiveTopology(Diligent::PRIMITIVE_TOPOLOGY Topology);
    PSOBuilder& WithRasterizerState(const Diligent::RasterizerStateDesc& Desc);
    PSOBuilder& WithDepthStencilState(const Diligent::DepthStencilStateDesc& Desc);
    PSOBuilder& WithBlendState(const Diligent::BlendStateDesc& Desc);

    // --- Render Target Configuration ---
    PSOBuilder& WithNumRenderTargets(Diligent::Uint8 NumRTVs);
    PSOBuilder& WithRTVFormat(Diligent::Uint8 RTVIndex, Diligent::TEXTURE_FORMAT Format);
    PSOBuilder& WithDSVFormat(Diligent::TEXTURE_FORMAT Format);

    // --- Preset Configurations ---
    PSOBuilder& DefaultsFor3D();
    PSOBuilder& DefaultsForTransparent();
    PSOBuilder& DefaultsForUI();

    // --- Finalization ---
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> Build(Diligent::IRenderDevice* pDevice);

private:
    Diligent::GraphicsPipelineStateCreateInfo m_PSOCreateInfo;

    // ✨ FIX: Add members to hold onto the shaders and keep them alive
    Diligent::RefCntAutoPtr<Diligent::IShader> m_pVS;
    Diligent::RefCntAutoPtr<Diligent::IShader> m_pPS;
};
