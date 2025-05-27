#pragma once
#include <string>

//#include <Graphics/GraphicsEngine/interface/GraphicsEngine.h>
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Texture.h>
#include <Graphics/GraphicsEngine/interface/TextureView.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
//#include <Common/interface/FileStream.h>

#include <TextureUtilities.h>
#include "Engine.h"
#include <thread>
#include <mutex>

using namespace Diligent;

class RenderTarget2D;


class Texture2D
{
public:

	static Texture2D* WhiteTex;
	~Texture2D();
	Texture2D(std::string path,bool threaded = false);
	Texture2D(int w, int h, float* data, int bpp);
	Texture2D(RenderTarget2D* rt);
	Texture2D(ITextureView* view)
	{
		m_pTextureView = view;
	}
	RefCntAutoPtr<ITextureView> GetView() {
		if (m_Loading) {
			return WhiteTex->GetView();
		}
		return m_pTextureView;
	}
	RefCntAutoPtr<ITexture> GetTex() {
		if (m_Loading) {
			return WhiteTex->GetTex();
		}
		return m_pTexture;
	}
	void Update(float* data);
	void Set(RefCntAutoPtr<ITexture> tex,RefCntAutoPtr<ITextureView> view)
	{
		m_pTextureView = view;
		m_pTexture = tex;

	}
	std::string GetPath() {
		return m_Path;
	}
	void Loaded() {
		m_Loading = false;
	}
private:

	RefCntAutoPtr<ITexture> m_pTexture;
	RefCntAutoPtr<ITextureView> m_pTextureView;
	std::string m_Path;
	bool m_Loading = false;
	int m_Width;
	int m_Height;
	std::thread load;
	std::mutex data_m;
};

