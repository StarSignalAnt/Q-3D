#include "pch.h"
#include "Texture2D.h"
#include <thread>
#include "RenderTarget2D.h"
#include "VFile.h"
#include "Engine.h"
#include <map>

std::map<std::string, Texture2D*> m_cache;

std::vector<uint8_t> GetTextureData(Diligent::ITexture* pTexture)
{
    auto pTexDesc = pTexture->GetDesc();

    if (pTexDesc.Type != Diligent::RESOURCE_DIM_TEX_2D)
    {
        throw std::runtime_error("Only 2D textures are supported");
    }

    uint32_t width = pTexDesc.Width;
    uint32_t height = pTexDesc.Height;
    uint32_t rowPitch = width * 4; // Assuming RGBA8 format
    uint32_t bufferSize = rowPitch * height;

    std::vector<uint8_t> textureData(bufferSize);

    // Try to map the texture directly
    Diligent::MappedTextureSubresource MappedData;
    Engine::m_pImmediateContext->MapTextureSubresource(pTexture, 0, 0, Diligent::MAP_READ, Diligent::MAP_FLAG_DO_NOT_WAIT, nullptr, MappedData);
    {
        // Direct access successful
        for (uint32_t y = 0; y < height; ++y)
        {
            const uint8_t* srcRow = static_cast<const uint8_t*>(MappedData.pData) + y * MappedData.Stride;
            uint8_t* dstRow = textureData.data() + y * rowPitch;
            std::memcpy(dstRow, srcRow, rowPitch);
        }


        Engine::m_pImmediateContext->UnmapTextureSubresource(pTexture, 0, 0);
    }
    
    //return nullptr;


    {
        // Direct access failed, fall back to staging texture method
        Diligent::TextureDesc StagingTexDesc = pTexDesc;
        StagingTexDesc.Usage = Diligent::USAGE_STAGING;
        StagingTexDesc.CPUAccessFlags = Diligent::CPU_ACCESS_READ;
        StagingTexDesc.BindFlags = Diligent::BIND_NONE;

        Diligent::RefCntAutoPtr<Diligent::ITexture> pStagingTexture;
        Engine::m_pDevice->CreateTexture(StagingTexDesc, nullptr, &pStagingTexture);

       // Engine::m_pImmediateContext->CopyTexture(pTexture, 0, 0, nullptr, pStagingTexture, 0, 0, nullptr, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);


       // if (pContext->MapTextureSubresource(pStagingTexture, 0, 0, Diligent::MAP_READ, Diligent::MAP_FLAG_DO_NOT_WAIT, nullptr, MappedData) == Diligent::SUCCEEDED)
        {
            for (uint32_t y = 0; y < height; ++y)
            {
                const uint8_t* srcRow = static_cast<const uint8_t*>(MappedData.pData) + y * MappedData.Stride;
                uint8_t* dstRow = textureData.data() + y * rowPitch;
                std::memcpy(dstRow, srcRow, rowPitch);
            }

        //    pContext->UnmapTextureSubresource(pStagingTexture, 0, 0);
        }
     
    }

    return textureData;
}

void _load_texture(Texture2D* texture)
{

    auto tex = new Texture2D(texture->GetPath(), false);
    texture->Set(tex->GetTex(),tex->GetView());
    texture->Loaded();
//    delete tex;
}

Texture2D* Texture2D::WhiteTex = nullptr;

Texture2D::Texture2D(std::string path,bool threaded) {

    int b = 5;
    if (VFile::Exists(path.c_str()) == false) {
    //    m_pTexture = WhiteTex->GetTex();
 //       m_pTextureView = WhiteTex->GetView();
        return;
    }


    if (true) {
        TextureLoadInfo loadInfo;

        if (m_cache.count(path) > 0)
        {
            
            auto tex = m_cache[path];
            m_pTexture = tex->GetTex();
            m_pTextureView = tex->GetView();
            m_Path = tex->GetPath();
            return;
            
        }


        loadInfo.BindFlags = BIND_FLAGS::BIND_SHADER_RESOURCE;// | BIND_FLAGS::BIND_UNORDERED_ACCESS;
       // loadInfo.GenerateMips = true;
        //loadInfo.Usage = USAGE::USAGE_DYNAMIC;

        loadInfo.Format = TEX_FORMAT_RGBA16_UNORM;
        loadInfo.MipLevels = 12;
        loadInfo.MipFilter = TEXTURE_LOAD_MIP_FILTER_DEFAULT;
        loadInfo.GenerateMips = true;

        // 
        Engine::m_pImmediateContext->Flush();
        CreateTextureFromFile(path.c_str(), loadInfo, Engine::m_pDevice, &m_pTexture);
      
        
        
        Engine::m_pImmediateContext->Flush();

        m_pTextureView = m_pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
     
        Engine::m_pImmediateContext->Flush();

       // StateTransitionDesc barriers[] = {
       //    {m_pTexture, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
       // };



      //  Engine::m_pImmediateContext->TransitionResourceStates(_countof(barriers), barriers);
     //   Engine::m_pImmediateContext->Flush();

      //  Engine::m_pImmediateContext->GenerateMips(m_pTextureView);



        m_Width = m_pTexture->GetDesc().Width;
        m_Height = m_pTexture->GetDesc().Height;
        m_Path = path;
        m_Loading = false;
        m_cache[path] = this;

    }
    else {

        m_Path = path;
        m_Loading = true;
        load = std::thread(_load_texture, this);

    }
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
    Engine::m_pDevice->CreateTexture(TexDesc, &tdata, &pTexture);
    m_pTexture = pTexture;
    m_pTextureView = pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

}

Texture2D::Texture2D(RenderTarget2D* target) {

    m_pTexture = target->GetTexture();
    m_pTextureView = target->GetView();

}

Texture2D::~Texture2D() {
    return;
    m_pTexture.Release();
    m_pTextureView.Release();
    m_pTexture.Detach();
    m_pTextureView.Detach();


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
    subresourceData.Stride = updateBox.MaxX * 4*sizeof(float); // Assuming 4 bytes per pixel (e.g., RGBA8 format)


    data_m.lock();
    // Update the texture
    Engine::m_pImmediateContext->UpdateTexture(m_pTexture, 0, 0, updateBox, subresourceData, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pTextureView = m_pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
    data_m.unlock();
}
