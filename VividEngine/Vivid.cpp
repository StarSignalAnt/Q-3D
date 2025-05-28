
#include "Vivid.h"

RefCntAutoPtr<IRenderDevice>  Vivid::m_pDevice;
RefCntAutoPtr<IDeviceContext> Vivid::m_pImmediateContext;
RefCntAutoPtr<ISwapChain>     Vivid::m_pSwapChain;
RefCntAutoPtr<IShaderSourceInputStreamFactory> Vivid::m_pShaderFactory;


int Vivid::GetFrameWidth() {


	//if (m_BoundRTC != nullptr) {
//		return m_BoundRTC->GetWidth();
//	}
//	if (m_BoundRT2D != nullptr) {
//		return m_BoundRT2D->GetWidth();
//	}
	return m_FrameWidth;

}

int Vivid::GetFrameHeight() {

//	if (m_BoundRTC != nullptr) {
	//	return m_BoundRTC->GetHeight();
//	}
//	if (m_BoundRT2D != nullptr) {
//		return m_BoundRT2D->GetHeight();
//	}
	return m_FrameHeight;


}

void Vivid::SetFrameWidth(int w) {

	m_FrameWidth = w;

}

void Vivid::SetFrameHeight(int h) {

	m_FrameHeight = h;

}

int Vivid::m_FrameWidth = 0;
int Vivid::m_FrameHeight = 0;


void Vivid::ClearZ() {

	m_pImmediateContext->ClearDepthStencil(m_pSwapChain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);


}