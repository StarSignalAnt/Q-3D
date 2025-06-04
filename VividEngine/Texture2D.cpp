#include "Texture2D.h"
#include "RenderTarget2D.h"
Texture2D::Texture2D(std::string path)
{
    m_Path = path;
	TextureLoadInfo loadInfo;
    loadInfo.BindFlags = BIND_FLAGS::BIND_SHADER_RESOURCE;// | BIND_FLAGS::BIND_UNORDERED_ACCESS;
    // loadInfo.GenerateMips = true;
     //loadInfo.Usage = USAGE::USAGE_DYNAMIC;

    loadInfo.Format = TEX_FORMAT_RGBA16_UNORM;
    
    loadInfo.MipLevels = 24;
   // 
    loadInfo.MipFilter = TEXTURE_LOAD_MIP_FILTER_DEFAULT;
    loadInfo.MipFilter = TEXTURE_LOAD_MIP_FILTER_BOX_AVERAGE;


    loadInfo.GenerateMips = true;

    // 
    Vivid::m_pImmediateContext->Flush();
    CreateTextureFromFile(path.c_str(), loadInfo, Vivid::m_pDevice, &m_pTexture);



    Vivid::m_pImmediateContext->Flush();


    m_pTextureView = m_pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

    Vivid::m_pImmediateContext->Flush();

    // StateTransitionDesc barriers[] = {
    //    {m_pTexture, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
    // };



   //  Engine::m_pImmediateContext->TransitionResourceStates(_countof(barriers), barriers);
  //   Engine::m_pImmediateContext->Flush();

   //  Engine::m_pImmediateContext->GenerateMips(m_pTextureView);



    m_Width = m_pTexture->GetDesc().Width;
    m_Height = m_pTexture->GetDesc().Height;



}

Texture2D::Texture2D(RenderTarget2D* target) {

    m_pTexture = target->GetTexture();
    m_pTextureView = target->GetView();

}