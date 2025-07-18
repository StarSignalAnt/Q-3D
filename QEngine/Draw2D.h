#pragma once
#include "BasicMath.hpp"
#include <glm/glm.hpp>
#include "GraphNode.h"
#include <vector>

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

class Texture2D;
class TextureCube;

using namespace Diligent;
// Forward declarations
class MaterialBasic3D;



namespace Q3D::Engine::Texture {
    class Texture2D;
}

//class Texture2D;
class RenderMaterial;
class Material2D;

// Vertex structure that matches the memory layout expected by your 3D material.
struct Draw2D_Vertex
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 uv;
    glm::vec4 view;
};

class Draw2D
{
public:
    // --- CREATION & DESTRUCTION ---
    // Creates persistent GPU resources. Call once during initialization.
    Draw2D(GraphNode* camera);
    // Releases resources. Call once during shutdown.
    ~Draw2D();

    // --- PER-FRAME USAGE ---
    // Call once at the start of a frame before any drawing.
    void BeginFrame();
    // Call for every rectangle you want to draw.
    void Rect(Q3D::Engine::Texture::Texture2D* image, glm::vec2 position, glm::vec2 size, glm::vec4 color);
    // Call once at the end of all 2D drawing for a frame to render the last batch.
    void Flush();
    void SetAdditive(bool add);
    // --- STATE MANAGEMENT ---
    void SetMaterial(RenderMaterial* material);
    void SetOverride(RenderMaterial* mat);

private:
    // Internal flush method used by Rect() when a batch break occurs.
    void InternalFlush();

    // --- MEMBER VARIABLES ---
    static const size_t MAX_QUADS_PER_BATCH = 1000;

    // GPU resources (created once)
    RefCntAutoPtr<IBuffer> m_VertexBuffer;
    RefCntAutoPtr<IBuffer> m_IndexBuffer;

    // CPU-side data for the current batch
    std::vector<Draw2D_Vertex> m_BatchVertices;
    Q3D::Engine::Texture::Texture2D* m_BatchTexture = nullptr;
    RenderMaterial* m_ActiveMaterial = nullptr;

    // Class state
    RenderMaterial* m_Material;
    RenderMaterial* m_Override = nullptr;
    GraphNode* m_Camera;
    bool m_Add = false;
};