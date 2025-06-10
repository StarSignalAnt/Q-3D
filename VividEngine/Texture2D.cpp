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


Texture2D::Texture2D(int w, int h, float* data, int bpp)
{
    m_Width = w;
    m_Height = h;

    TextureDesc TexDesc;
    TexDesc.Name = "Float Tex2D"; // Name of the texture
    TexDesc.Type = RESOURCE_DIM_TEX_2D; // Cube map type
    TexDesc.Width = w;
    TexDesc.Height = h;
    TexDesc.Format = TEX_FORMAT_RGBA32_FLOAT; ;// Engine::m_pSwapChain->GetCurrentBackBufferRTV()->GetDesc().Format;  //DXGI_FORMAT_R32G32B32A32_FLOAT; // Assuming RGBA EXR format
    TexDesc.BindFlags = BIND_SHADER_RESOURCE;
    TexDesc.Usage = USAGE_DEFAULT;

    TexDesc.MipLevels = 1;
    std::vector<TextureSubResData> res;

    TextureSubResData adata;
    adata.pData = data;
    adata.Stride = w * sizeof(float) * 4;

    TextureData tdata;
    tdata.NumSubresources = 1;
    tdata.pSubResources = &adata;


    RefCntAutoPtr<ITexture> pTexture;
    Vivid::m_pDevice->CreateTexture(TexDesc, &tdata, &pTexture);
    m_pTexture = pTexture;
    m_pTextureView = pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

}


void Texture2D::Update(float* data) {


    Diligent::Box updateBox;
    updateBox.MinX = 0;
    updateBox.MaxX = m_Width;
    updateBox.MinY = 0;
    updateBox.MaxY = m_Height;
    updateBox.MinZ = 0;
    updateBox.MaxZ = 1;

    // Define the data to update the texture
    Diligent::TextureSubResData subresourceData;
    subresourceData.pData = data;
    subresourceData.Stride = updateBox.MaxX * 4 * sizeof(float); // Assuming 4 bytes per pixel (e.g., RGBA8 format)


//    data_m.lock();
    // Update the texture
    Vivid::m_pImmediateContext->UpdateTexture(m_pTexture, 0, 0, updateBox, subresourceData, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pTextureView = m_pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
  //  data_m.unlock();
}
