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
#include "MonoLib.h"
#include <functional>
#include "NodeRegistry.h"
class RenderTargetCube;
class RenderTarget2D;
class MaterialPBR;
class Physics;
class NodeRegistry;



using namespace Diligent;

using VividCallbackFunc = void(*)(std::string value);

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
	static void InitMono();
	static Physics* m_Physics;
	static VividCallbackFunc DebugLogCB;
	static std::vector<MaterialPBR*> m_ActiveMaterials;
	static MonoLib* m_MonoLib;
	static std::vector<SharpClassInfo> m_ComponentClasses;
	static void DebugLog(const std::string& message)
	{
		if (DebugLogCB) {
			DebugLogCB(message);
		}
	}
	static NodeRegistry& GetNodeRegistry() {
		return NodeRegistry::GetInstance();
	}
	static void RegisterNodeTypes();
private:


	
	static int m_FrameWidth;
	static int m_FrameHeight;

};

