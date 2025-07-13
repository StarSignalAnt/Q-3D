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
#include<glm/glm.hpp>

namespace Q3D::Engine::Texture {
	class Texture2D;
}
//class Texture2D;


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
	RefCntAutoPtr<ITextureView> GetDepthShaderView() {
		return m_DepthShaderView;
	}
	Q3D::Engine::Texture::Texture2D* GetDepthTexture2D();
	Q3D::Engine::Texture::Texture2D* GetTexture2D();
	void ClearZ();
	void SetClearCol(glm::vec4 col) {
		ClearCol = col;
	}
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
	glm::vec4 ClearCol = glm::vec4(1, 1, 1, 1);
};

