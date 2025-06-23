#pragma once
#include "BasicMath.hpp"
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Texture.h>
#include <Graphics/GraphicsEngine/interface/TextureView.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
//#include <Common/interface/FileStream.h>

#include <TextureUtilities.h>
using namespace Diligent;


class RenderTarget2D
{
public:

	RenderTarget2D(int width, int height,bool depth);
	void Bind();
	void Release();
	int GetWidth() {
		return m_Width;
	}
	int GetHeight() {
		return m_Height;
	}
	RefCntAutoPtr<ITexture> GetTexture() {
		return m_Texture;
	}
	RefCntAutoPtr<ITextureView> GetView() {
		return m_ShaderView;
	}
	RefCntAutoPtr<ITexture> GetDepthTexture() {
		return m_DepthTexture;
	}
	RefCntAutoPtr<ITextureView> GetDepthView() {
		return m_DepthShaderView;
	}
	void ClearZ();
private:
	int m_Width = 0;
	int m_Height = 0;
	float4 m_ClearValue;
	RefCntAutoPtr<ITexture> m_DepthTexture;
	RefCntAutoPtr<ITexture> m_Texture;
	RefCntAutoPtr<ITextureView> m_RTView;
	RefCntAutoPtr<ITextureView> m_ShaderView;
	RefCntAutoPtr<ITextureView> m_DepthView;
	RefCntAutoPtr<ITextureView> m_DepthShaderView;
};

