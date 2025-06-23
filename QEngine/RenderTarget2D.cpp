#include "pch.h"
#include "RenderTarget2D.h"
#include "QEngine.h"

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
		desc.Format = QEngine::m_pSwapChain->GetDesc().ColorBufferFormat;
	}
	desc.BindFlags = BIND_FLAGS::BIND_SHADER_RESOURCE | BIND_FLAGS::BIND_RENDER_TARGET;
	desc.ClearValue.Format = desc.Format;
	desc.ClearValue.Color[0] = 0;
	desc.ClearValue.Color[1] = 0;
	desc.ClearValue.Color[2] = 0;
	desc.ClearValue.Color[3] = 1;
	desc.SampleCount = 1;

	QEngine::m_pDevice->CreateTexture(desc, nullptr, &m_Texture);
	m_RTView = m_Texture->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
	m_ShaderView = m_Texture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

	TextureDesc depth = desc;
	depth.Name = "RenderTarget2D Depth Texture";
	depth.Format = TEX_FORMAT_D32_FLOAT;
	depth.BindFlags = BIND_FLAGS::BIND_DEPTH_STENCIL | BIND_FLAGS::BIND_SHADER_RESOURCE;
	depth.ClearValue.Format = TEX_FORMAT_D32_FLOAT;
	depth.ClearValue.DepthStencil.Depth = 1.0;
	depth.ClearValue.DepthStencil.Stencil = 0;
	QEngine::m_pDevice->CreateTexture(depth, nullptr, &m_DepthTexture);

	TextureViewDesc depthViewDesc;
	depthViewDesc.ViewType = TEXTURE_VIEW_DEPTH_STENCIL;
	depthViewDesc.Format = TEX_FORMAT_D32_FLOAT;
	m_DepthTexture->CreateView(depthViewDesc, &m_DepthView);


	TextureViewDesc depthShaderViewDesc;
	depthShaderViewDesc.ViewType = TEXTURE_VIEW_SHADER_RESOURCE;
	depthShaderViewDesc.Format = TEX_FORMAT_R32_FLOAT;
	m_DepthTexture->CreateView(depthShaderViewDesc, &m_DepthShaderView);






}

void RenderTarget2D::ClearZ() {

	QEngine::m_pImmediateContext->ClearDepthStencil(m_DepthView, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

}

void RenderTarget2D::Bind() {

	//ITextureView* tv = m_DepthTexView;
	float ClearColor[4] = { 1,1,1,1.0 };
	//if (!m_Depth) {
	//	ClearColor[0] = 0;
	//	ClearColor[1] = 0;
	//	ClearColor[2] = 0;
//	}


	QEngine::SetBoundRT2D(this);

	QEngine::m_pImmediateContext->SetRenderTargets(1, &m_RTView, m_DepthView, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	QEngine::m_pImmediateContext->ClearRenderTarget(m_RTView, ClearColor, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	QEngine::m_pImmediateContext->ClearDepthStencil(m_DepthView, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);



}

void RenderTarget2D::Release() {

	auto pRTV = QEngine::m_pSwapChain->GetCurrentBackBufferRTV();

	QEngine::m_pImmediateContext->SetRenderTargets(1, &pRTV, QEngine::m_pSwapChain->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	QEngine::SetBoundRT2D(nullptr);

}