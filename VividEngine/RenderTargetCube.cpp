
#include "RenderTargetCube.h"

RenderTargetCube::RenderTargetCube(int width, int height, bool depth) {

	m_Width = width;
	m_Height = height;

	TextureDesc tex;

	m_Depth = depth;

	tex.Width = width;
	tex.Height = height;
	tex.Type = RESOURCE_DIM_TEX_CUBE;
	tex.MipLevels = 1;
	tex.ArraySize = 6;
	tex.Usage = USAGE_DEFAULT;
	if (depth) {
		tex.Format = TEX_FORMAT_R32_FLOAT;
	}
	else {
		tex.Format = Vivid::m_pSwapChain->GetCurrentBackBufferRTV()->GetDesc().Format;
	}
	tex.BindFlags = BIND_FLAGS::BIND_SHADER_RESOURCE | BIND_FLAGS::BIND_RENDER_TARGET;
	tex.ClearValue.Format = tex.Format;
	tex.ClearValue.Color[0] = 0;
	tex.ClearValue.Color[1] = 0;
	tex.ClearValue.Color[2] = 0;
	tex.ClearValue.Color[3] = 0;
	Vivid::m_pDevice->CreateTexture(tex, nullptr, &m_Texture);
	m_TexView = m_Texture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
	for (int i = 0; i < 6; i++) {
		TextureViewDesc desc;
		desc.Name = "Cube Face";
		desc.TextureDim = RESOURCE_DIM_TEX_2D_ARRAY;
		desc.ViewType = TEXTURE_VIEW_RENDER_TARGET;
		desc.MostDetailedMip = 0;
		desc.FirstArraySlice = i;
		desc.NumArraySlices = 1;
		m_Texture->CreateView(desc, &m_FaceView[i]);
	}

	TextureDesc dep;

	dep.Name = "RenderCube Depth";
	dep.Format = TEXTURE_FORMAT::TEX_FORMAT_D32_FLOAT;
	dep.BindFlags = BIND_FLAGS::BIND_DEPTH_STENCIL;
	dep.ClearValue.Format = dep.Format;
	dep.ClearValue.DepthStencil.Depth = 1;
	dep.ClearValue.DepthStencil.Stencil = 0;
	dep.Type = RESOURCE_DIM_TEX_2D;
	dep.Width = width;
	dep.Height = height;
	Vivid::m_pDevice->CreateTexture(dep, nullptr, &m_DepthTex);
	m_DepthTexView = m_DepthTex->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL);



}

void RenderTargetCube::Bind(int face) {

	//ITextureView* views = new ITextureView[1];
	//views[0] = m_DepthTexView;

	ITextureView* tv = m_DepthTexView;
	float ClearColor[4] = { 1,1,1,1.0 };
	if (!m_Depth) {
		ClearColor[0] = 0;
		ClearColor[1] = 0;
		ClearColor[2] = 0;
	}
	Vivid::SetBoundRTC(this);

	Vivid::m_pImmediateContext->SetRenderTargets(1, &m_FaceView[face], m_DepthTexView, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	Vivid::m_pImmediateContext->ClearRenderTarget(m_FaceView[face], ClearColor, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	Vivid::m_pImmediateContext->ClearDepthStencil(m_DepthTexView, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);



}

void RenderTargetCube::Release(int face) {

	auto pRTV = Vivid::m_pSwapChain->GetCurrentBackBufferRTV();

	Vivid::m_pImmediateContext->SetRenderTargets(1, &pRTV, Vivid::m_pSwapChain->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	Vivid::SetBoundRTC(nullptr);


}