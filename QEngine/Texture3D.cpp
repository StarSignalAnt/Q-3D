#include "Texture3D.h"
#include "Texture3D.h"
#include <iostream>

Texture3D::Texture3D(int width, int height, int depth) :
    m_Width(width),
    m_Height(height),
    m_Depth(depth)
{
    TextureDesc TexDesc;
    TexDesc.Name = "Volumetric Cloud Texture 3D";
    TexDesc.Type = RESOURCE_DIM_TEX_3D; // Specify that this is a 3D texture
    TexDesc.Width = m_Width;
    TexDesc.Height = m_Height;
    TexDesc.Depth = m_Depth;
    TexDesc.MipLevels = 1; // Start with no mipmaps for simplicity

    // R8_UNORM is a good choice for single-channel density data (like noise)
    // It's memory-efficient, storing one 8-bit value (0-255) per texel.
    TexDesc.Format = TEX_FORMAT_R8_UNORM;

    // The texture needs to be bindable as a shader resource to be read in the shader.
    TexDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;

    // USAGE_DEFAULT allows the GPU to update the texture, which is what we need.
    TexDesc.Usage = USAGE_DEFAULT;

    // Create the texture on the GPU. We pass nullptr for the initial data
    // because we will be uploading it layer by layer.
    Q3D::Engine::QEngine::GetDevice()->CreateTexture(TexDesc, nullptr, &m_pTexture);

    if (m_pTexture)
    {
        // Get the default shader resource view for the entire 3D texture.
        m_pTextureView = m_pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
    }
    else
    {
        std::cerr << "Error: Failed to create 3D texture." << std::endl;
    }
}

void Texture3D::UploadLayer(const void* pData, int depthSlice)
{
    if (!m_pTexture || pData == nullptr)
    {
        std::cerr << "Error: Cannot upload data to an invalid 3D texture or with null data." << std::endl;
        return;
    }

    if (depthSlice < 0 || depthSlice >= m_Depth)
    {
        std::cerr << "Error: depthSlice " << depthSlice << " is out of bounds." << std::endl;
        return;
    }

    // Define the 3D box (region) to update. In this case, it's a single 2D slice.
    Box updateBox;
    updateBox.MinX = 0;
    updateBox.MaxX = m_Width;
    updateBox.MinY = 0;
    updateBox.MaxY = m_Height;
    updateBox.MinZ = depthSlice;      // The start of the slice to update
    updateBox.MaxZ = depthSlice + 1;  // The end of the slice

    // Describe the source data layout
    TextureSubResData subresourceData;
    subresourceData.pData = pData;
    // Stride is the size of one row in bytes. For R8_UNORM, it's width * 1 byte.
    subresourceData.Stride = m_Width * sizeof(uint8_t);
    // DepthStride is the size of one full 2D slice. For R8_UNORM, it's width * height * 1 byte.
    subresourceData.DepthStride = m_Width * m_Height * sizeof(uint8_t);

    // Execute the update on the GPU
    Q3D::Engine::QEngine::GetContext()->UpdateTexture(
        m_pTexture,                             // Destination texture
        0,                                      // Mip level to update
        0,                                      // Array slice (not used for Tex3D)
        updateBox,                              // The 3D region to update
        subresourceData,                        // The source data
        RESOURCE_STATE_TRANSITION_MODE_NONE,    // Assume texture is already in a good state
        RESOURCE_STATE_TRANSITION_MODE_TRANSITION // Transition texture to be readable by shader
    );
}

RefCntAutoPtr<ITextureView> Texture3D::GetUAV() {
    if (m_pUAV == nullptr) {
        TextureViewDesc ViewDesc;
        ViewDesc.ViewType = TEXTURE_VIEW_UNORDERED_ACCESS;
        ViewDesc.Format = m_pTexture->GetDesc().Format;
        m_pTexture->CreateView(ViewDesc, &m_pUAV);
    }
    return m_pUAV;
}