#pragma once
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
class TextureDepth
{
public:
	TextureDepth(RenderTarget2D* rt);

	RefCntAutoPtr<ITextureView> GetView() {

		return m_pTextureView;
	}
	RefCntAutoPtr<ITexture> GetTex() {
		
		return m_pTexture;
	}

private:

	RefCntAutoPtr<ITexture> m_pTexture;
	RefCntAutoPtr<ITextureView> m_pTextureView;
};

