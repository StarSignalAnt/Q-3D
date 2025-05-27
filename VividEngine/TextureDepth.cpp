#include "TextureDepth.h"
#include "RenderTarget2D.h"

TextureDepth::TextureDepth(RenderTarget2D* target) {


    m_pTexture = target->GetDepthTexture();
    m_pTextureView = target->GetDepthView();

}