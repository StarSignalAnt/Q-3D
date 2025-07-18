#include "pch.h"
#include "RenderTarget2D.h"
#include "QEngine.h"
#include "Texture2D.h"

RenderTarget2D::RenderTarget2D(int width, int height,bool isdepth) {

	m_Width = width;
	m_Height = height;

	TextureDesc desc;
	desc.Name = "RenderTarget2D Texture";
	desc.Type = RESOURCE_DIM_TEX_2D;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	if (isdepth) {
		desc.Format = TEX_FORMAT_R32_FLOAT;
	}
	else {
		desc.Format = Q3D::Engine::QEngine::GetSwapChain()->GetDesc().ColorBufferFormat;
	}
	desc.BindFlags = BIND_FLAGS::BIND_SHADER_RESOURCE | BIND_FLAGS::BIND_RENDER_TARGET;
	desc.ClearValue.Format = desc.Format;
	desc.ClearValue.Color[0] = 0;
	desc.ClearValue.Color[1] = 0;
	desc.ClearValue.Color[2] = 0;
	desc.ClearValue.Color[3] = 1;
	desc.SampleCount = 1;

	Q3D::Engine::QEngine::GetDevice()->CreateTexture(desc, nullptr, &m_Texture);
	m_RTView = m_Texture->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
	m_ShaderView = m_Texture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);


	TextureDesc depth = desc;
	depth.Name = "RenderTarget2D Depth Texture";
	depth.Format = TEX_FORMAT_D32_FLOAT;
	depth.BindFlags = BIND_FLAGS::BIND_DEPTH_STENCIL | BIND_FLAGS::BIND_SHADER_RESOURCE;
	depth.ClearValue.Format = TEX_FORMAT_D32_FLOAT;
	depth.ClearValue.DepthStencil.Depth = 1.0;
	depth.ClearValue.DepthStencil.Stencil = 0;
	Q3D::Engine::QEngine::GetDevice()->CreateTexture(depth, nullptr, &m_DepthTexture);


	TextureViewDesc depthViewDesc;
	depthViewDesc.ViewType = TEXTURE_VIEW_DEPTH_STENCIL;
	depthViewDesc.Format = TEX_FORMAT_D32_FLOAT;
	m_DepthTexture->CreateView(depthViewDesc, &m_DepthView);


	TextureViewDesc depthShaderViewDesc;
	depthShaderViewDesc.ViewType = TEXTURE_VIEW_SHADER_RESOURCE;
	depthShaderViewDesc.Format = TEX_FORMAT_R32_FLOAT;
	m_DepthTexture->CreateView(depthShaderViewDesc, &m_DepthShaderView);






}

Q3D::Engine::Texture::Texture2D* RenderTarget2D::GetDepthTexture2D() {
	return new Q3D::Engine::Texture::Texture2D(GetDepthTexture(), GetDepthShaderView());
}

void RenderTarget2D::ClearZ() {

	Q3D::Engine::QEngine::GetContext()->ClearDepthStencil(m_DepthView, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

}

void RenderTarget2D::Bind() {

	//ITextureView* tv = m_DepthTexView;
	float ClearColor[4] = { ClearCol.x,ClearCol.y,ClearCol.z,1.0f };

	//if (!m_Depth) {
	//	ClearColor[0] = 0;
	//	ClearColor[1] = 0;
	//	ClearColor[2] = 0;
//	}


	Q3D::Engine::QEngine::SetBoundRT2D(this);

	Q3D::Engine::QEngine::GetContext()->SetRenderTargets(1, &m_RTView, m_DepthView, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	Q3D::Engine::QEngine::GetContext()->ClearRenderTarget(m_RTView, ClearColor, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	Q3D::Engine::QEngine::GetContext()->ClearDepthStencil(m_DepthView, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);



}

void RenderTarget2D::Release() {

	auto pRTV = Q3D::Engine::QEngine::GetSwapChain()->GetCurrentBackBufferRTV();

	Q3D::Engine::QEngine::GetContext()->SetRenderTargets(1, &pRTV, Q3D::Engine::QEngine::GetSwapChain()->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	Q3D::Engine::QEngine::SetBoundRT2D(nullptr);


}

Q3D::Engine::Texture::Texture2D* RenderTarget2D::GetTexture2D() {
	return new Q3D::Engine::Texture::Texture2D(GetTexture(), GetView());
}