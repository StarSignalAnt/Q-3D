#pragma once
#include "RendererBase.h"
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
#include "MapHelper.hpp"
#include "GraphicsTypesX.hpp"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"
#include "ShaderMacroHelper.hpp"

#include "AdvancedMath.hpp"

#include "structures.fxh"

class RTMesh;
class Draw2D;
class NodeTerrain;

class SolarisRenderer :
    public RendererBase
{
public:

    SolarisRenderer();
    void CreateStructures();
    void CreateTopLevel();
    void CreateSBT();
    void PreRender() override;
    void Render(bool no_render) override;
    void UpdatePrimitives();
    void UpdateLights();
    void CreatePipeline();
    void UpdateDynamics();
    void UpdateTextures();
    void UpdateTerrain();
    Texture2D* GetRenderedFrame();

private:

    bool m_RayTracedShadows = true;

    RefCntAutoPtr<IShaderResourceBinding> m_RayTracingSRB;
    //data
    std::vector<RTMesh*> m_Meshes;
    std::vector<RTMesh*> m_DynMeshes;
    RefCntAutoPtr<ITopLevelAS> m_TLAS;
    RefCntAutoPtr<IBuffer>             m_ScratchBuffer;
    RefCntAutoPtr<IBuffer>             m_InstanceBuffer;
    RefCntAutoPtr<IShaderBindingTable> m_SBT;
    RefCntAutoPtr<IBuffer> m_RenderAttribsCB;
    Constants m_Constants = {};
    RefCntAutoPtr<IBuffer> m_ConstantsCB;
    static constexpr int NumTextures = 4;
    static constexpr int NumCubes = 4;

    RefCntAutoPtr<IBuffer> m_CubeAttribsCB;
    RefCntAutoPtr<IBuffer> m_BoxAttribsCB;


    RefCntAutoPtr<IPipelineState>         m_pRayTracingPSO;
    RefCntAutoPtr<IShaderResourceBinding> m_pRayTracingSRB;

    RefCntAutoPtr<IPipelineState>         m_pImageBlitPSO;
    RefCntAutoPtr<IShaderResourceBinding> m_pImageBlitSRB;

    RefCntAutoPtr<IBottomLevelAS>      m_pCubeBLAS;
    RefCntAutoPtr<IBottomLevelAS>      m_pProceduralBLAS;
    RefCntAutoPtr<ITopLevelAS>         m_pTLAS;

    RefCntAutoPtr<IShaderBindingTable> m_pSBT;

    Uint32          m_MaxRecursionDepth = 8;
    const double    m_MaxAnimationTimeDelta = 1.0 / 60.0;
    float           m_AnimationTime = 0.0f;
    
    bool            m_EnableCubes[NumCubes] = { true, true, true, true };
    bool            m_Animate = true;
    float           m_DispersionFactor = 0.1f;
    TEXTURE_FORMAT          m_ColorBufferFormat = TEX_FORMAT_RGBA32_FLOAT;
    RefCntAutoPtr<ITexture> m_pColorRT;

    Draw2D* m_Draw;
    RefCntAutoPtr<ITextureView> m_GlobalTex;
    RefCntAutoPtr<IBuffer> vertsBuf;
    RefCntAutoPtr<IBuffer> trisBuf;
    RefCntAutoPtr<IBuffer> primBuf;
    RefCntAutoPtr<IBuffer> offsetsBuf;
    RefCntAutoPtr<IBuffer> lightsBuf;
    RefCntAutoPtr<IBufferView> verticesBufferView;
    RefCntAutoPtr<IBufferView> trisBufferView;
    RefCntAutoPtr<IBufferView> primitivesBufferView;
    RefCntAutoPtr<IBufferView> offsetsBufferView;
    RefCntAutoPtr<IBufferView> lightsBufferView;
    RefCntAutoPtr<IBuffer> dynVertsBuf;
    RefCntAutoPtr<IBufferView> dynVertsBufView;
    RefCntAutoPtr<IBuffer> dynTrisBuf;
    RefCntAutoPtr<IBufferView> dynTrisBufView;
    RefCntAutoPtr<IBuffer> dynOffsetsBuf;
    RefCntAutoPtr<IBufferView> dynOffsetsBufView;
    RefCntAutoPtr<IBuffer> terrainVertsBuf;
    RefCntAutoPtr<IBuffer> terrainTrisBuf;
    RefCntAutoPtr<IBufferView> terrainVertsBufView;
    RefCntAutoPtr<IBufferView> terrainTrisBufView;
    Texture2D* m_RenderedFrame = nullptr;
    int prevCount = 0;
    NodeTerrain* m_ActiveTerrain = nullptr;
};

