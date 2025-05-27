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
#include "NodeCamera.h"
#include "NodeLight.h"

using namespace Diligent;

class RenderTargetCube;
class RenderTarget2D;
class ScriptHost;
class MaterialBase;
class SceneGraph;
class RendererBase;
class SolarisRenderer;

class Engine
{
public:

	static RefCntAutoPtr<IRenderDevice>  m_pDevice;
	static RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
	static RefCntAutoPtr<ISwapChain>     m_pSwapChain;
	static RefCntAutoPtr<IShaderSourceInputStreamFactory> m_pShaderFactory;
	static NodeCamera* m_Camera;
	static Node* m_Node;
	static int GetFrameWidth();
	static int GetFrameHeight();
	static void SetFrameWidth(int w);
	static void SetFrameHeight(int h);
	
	static std::vector<NodeLight*> m_Lights;
	static NodeLight* m_Light;
	static void ClearZ();
	static void SetBoundRTC(RenderTargetCube* target);
	static void SetBoundRT2D(RenderTarget2D* target);

	static RenderTargetCube* m_BoundRTC;
	static RenderTarget2D* m_BoundRT2D;
	static std::vector<MaterialBase*> m_ActiveMaterials;
	static MaterialBase* FindActiveMaterial(std::string path);
	static ScriptHost* m_ScriptHost;
	static SceneGraph* m_ActiveGraph;
	static bool m_MeterialsUpdated;
	static std::string m_ContentPath;

	static std::optional<std::string> FindResource(std::string name);
	static std::optional<std::string> searchStringInPath(const std::string& path, const std::string& searchString);
	static SolarisRenderer* m_Solaris;
	static RendererBase* m_ActiveRenderer;
private:

	static int m_FrameWidth;
	static int m_FrameHeight;

};

