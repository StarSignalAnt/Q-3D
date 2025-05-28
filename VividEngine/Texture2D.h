#pragma once
#include <string>

#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Texture.h>
#include <Graphics/GraphicsEngine/interface/TextureView.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
//#include <Common/interface/FileStream.h>

#include <TextureUtilities.h>
#include "Vivid.h"
#include <thread>
#include <mutex>

using namespace Diligent;

class Texture2D
{
public:

	Texture2D(std::string path);
	RefCntAutoPtr<ITextureView> GetView() {
	
		return m_pTextureView;
	}
	RefCntAutoPtr<ITexture> GetTex() {
	
		return m_pTexture;
	}
private:

	int m_Width;
	int m_Height;
	RefCntAutoPtr<ITexture> m_pTexture;
	RefCntAutoPtr<ITextureView> m_pTextureView;

};

