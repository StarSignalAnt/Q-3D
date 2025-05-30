#pragma once
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Texture.h>
#include <Graphics/GraphicsEngine/interface/TextureView.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
//#include <Common/interface/FileStream.h>

#include <TextureUtilities.h>
#include "Vivid.h"

using namespace Diligent;

class RenderTargetCube
{
public:

	RenderTargetCube(int width, int height, bool depth);
	void Bind(int face);
	void Release(int face);
	int GetWidth() {
		return m_Width;
	}
	int GetHeight() {
		return m_Height;
	}
	RefCntAutoPtr<ITextureView> GetTexView() {
		return m_TexView;
	}
private:

	int m_Width = 0;
	int m_Height = 0;
	RefCntAutoPtr<ITextureView> m_FaceView[6];
	RefCntAutoPtr<ITexture> m_Texture;
	RefCntAutoPtr<ITexture> m_DepthTex;
	RefCntAutoPtr<ITextureView> m_DepthTexView;
	RefCntAutoPtr<ITextureView> m_TexView;
	bool m_Depth = false;

};

