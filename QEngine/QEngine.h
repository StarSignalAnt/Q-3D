#pragma once
#define NOMINMAX

#ifdef QENGINE_BUILD_EXE
// This is the App.exe, which exports the symbols
#define QENGINE_API __declspec(dllexport)

#elif defined(QENGINE_BUILD_DLL)
// This is the CGame.dll, which imports the symbols
#define QENGINE_API __declspec(dllimport)

#else
// This is the Engine.lib, which does nothing special
#define QENGINE_API

#endif

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
#include <filesystem>
class Draw2D;
class RenderTargetCube;
class RenderTarget2D;
class MaterialPBR;
class Physics;
class NodeRegistry;
class GameVideo;
class GameContent;
class DllImporter;

using namespace Diligent;
namespace Q3D::Engine {
using VividCallbackFunc = void(*)(std::string value);



	class QEngine
	{
	public:

		QENGINE_API static void RegisterCComponent(Component* component);



		QENGINE_API static void InitCDLL();
		static float m_DeltaTime;

		QENGINE_API static RefCntAutoPtr<IRenderDevice> GetDevice() {
			return m_pDevice;
		}

		QENGINE_API static RefCntAutoPtr<IDeviceContext> GetContext() {
			return m_pImmediateContext;
		}


		QENGINE_API static RefCntAutoPtr<ISwapChain> GetSwapChain() {
			return m_pSwapChain;
		}

		QENGINE_API static RefCntAutoPtr<IShaderSourceInputStreamFactory> GetShaderFactory() {

			return m_pShaderFactory;

		}

		QENGINE_API static std::vector<MaterialPBR*> GetActiveMaterials() {
			return m_ActiveMaterials;
		}

		QENGINE_API static std::vector<Component*> GetCComponents() {
			return m_CComponents;
		}

		QENGINE_API static std::vector<SharpClassInfo> GetSharpComponentClasses() {
			return m_ComponentClasses;
		}

		QENGINE_API static MonoLib* GetMonoLib() {
			return m_MonoLib;
		}

		QENGINE_API static void InitEngine();
		QENGINE_API static void SetActiveMaterials(std::vector<MaterialPBR*> mats)
		{
			m_ActiveMaterials = mats;
		}

		QENGINE_API static GameContent* GetContent() {
			return m_Content;
		}

		QENGINE_API static int GetFrameWidth();
		QENGINE_API static int GetFrameHeight();
		QENGINE_API static void SetFrameWidth(int w);
		QENGINE_API static void SetFrameHeight(int h);
		QENGINE_API static void ClearZ();
		QENGINE_API static void SetBoundRTC(RenderTargetCube* target);
		QENGINE_API static void SetBoundRT2D(RenderTarget2D* target);

		QENGINE_API static void InitPython();
		QENGINE_API static void InitMono();

		QENGINE_API static void DebugLog(const std::string& message);

		QENGINE_API static NodeRegistry& GetNodeRegistry() {
			return NodeRegistry::GetInstance();
		}
		QENGINE_API static void RegisterNodeTypes();
		QENGINE_API static void StartVideo(std::string path);
		QENGINE_API static void RenderVideo();
		QENGINE_API static void SetVideo(GameVideo* video);


		QENGINE_API static void SetContentPath(std::string path);
		QENGINE_API static std::string GetContentPath();
		QENGINE_API static void CheckDLL();

		QENGINE_API static void SetScissor(int x, int y, int w, int h) {
			ScX = x;
			ScY = y;
			ScW = w;
			ScH = h;
		}

		QENGINE_API static bool IsCComponent(Component* comp);

		QENGINE_API static Physics* GetPhysics() {
			return m_Physics;
		}
		QENGINE_API static void SetContent(GameContent* content) {
			m_Content = content;
		}
		QENGINE_API static void SetDevice(RefCntAutoPtr<IRenderDevice> dev)
		{
			m_pDevice = dev;
		}
		QENGINE_API static void SetContext(RefCntAutoPtr<IDeviceContext> context) {

			m_pImmediateContext = context;
		}
		QENGINE_API static void SetSwapChain(RefCntAutoPtr<ISwapChain> chain) {
			m_pSwapChain = chain;
		}
		QENGINE_API static void SetShaderFactory(RefCntAutoPtr<IShaderSourceInputStreamFactory> factor) {
			m_pShaderFactory = factor;
		}
		QENGINE_API static int GetScX() {
			return ScX;
		}
		QENGINE_API static int GetScY() {
			return ScY;
		}
		QENGINE_API static int GetScW() {
			return ScW;
		}
		QENGINE_API static int GetScH() {
			return ScH;
		}

	private:
		static GameVideo* m_CurrentVideo;
		static Physics* m_Physics;
		static VividCallbackFunc DebugLogCB;
		static std::vector<MaterialPBR*> m_ActiveMaterials;
		static MonoLib* m_MonoLib;
		static std::vector<SharpClassInfo> m_ComponentClasses;
		static Draw2D* m_Draw;
		static int ScX;
		static int ScY;
		static int ScW;
		static int ScH;
		static std::filesystem::file_time_type s_last_dll_write_time;
		static GameContent* m_Content;
		static std::vector<Component*> m_CComponents;
		static DllImporter* m_CDLL;
		static int m_FrameWidth;
		static int m_FrameHeight;
		static RefCntAutoPtr<IRenderDevice>  m_pDevice;
		static RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
		static RefCntAutoPtr<ISwapChain>     m_pSwapChain;
		static RefCntAutoPtr<IShaderSourceInputStreamFactory> m_pShaderFactory;
		static RenderTargetCube* m_BoundRTC;
		static RenderTarget2D* m_BoundRT2D;
		static std::string m_ContentPath;
	};
}
