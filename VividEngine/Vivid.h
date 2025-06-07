#pragma once

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

class RenderTargetCube;
class RenderTarget2D;
class Physics;

using namespace Diligent;

class Vivid
{
public:

	static RefCntAutoPtr<IRenderDevice>  m_pDevice;
	static RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
	static RefCntAutoPtr<ISwapChain>     m_pSwapChain;
	static RefCntAutoPtr<IShaderSourceInputStreamFactory> m_pShaderFactory;
	static void InitEngine();

	static int GetFrameWidth();
	static int GetFrameHeight();
	static void SetFrameWidth(int w);
	static void SetFrameHeight(int h);
	static void ClearZ();
	static void SetBoundRTC(RenderTargetCube* target);
	static void SetBoundRT2D(RenderTarget2D* target);
	static RenderTargetCube* m_BoundRTC;
	static RenderTarget2D* m_BoundRT2D;
	static void InitPython();
	static Physics* m_Physics;
private:

	static int m_FrameWidth;
	static int m_FrameHeight;

};

