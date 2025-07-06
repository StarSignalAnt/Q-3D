#pragma once
#include "QEngine.h" // For access to the device and context

// Diligent Engine Headers
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Texture.h>
#include <Graphics/GraphicsEngine/interface/TextureView.h>
#include <Common/interface/RefCntAutoPtr.hpp>

using namespace Diligent;


class Texture3D
{
public:
    Texture3D(int width, int height, int depth);

    // Destructor
    ~Texture3D() = default;

    // Uploads a 2D slice of data to a specific depth layer of the 3D texture.
    void UploadLayer(const void* pData, int depthSlice);

    int GetWidth() { return m_Width; }
    int GetHeight() { return m_Height; }
    int GetDepth() { return m_Depth; }

    // Getters for using the texture in shaders
    RefCntAutoPtr<ITextureView> GetView() { return m_pTextureView; }
    RefCntAutoPtr<ITexture> GetTex() { return m_pTexture; }
    RefCntAutoPtr<ITextureView> GetUAV();
     
private:
    int m_Width;
    int m_Height;
    int m_Depth;
    RefCntAutoPtr<ITextureView> m_pUAV;
    RefCntAutoPtr<ITexture> m_pTexture;
    RefCntAutoPtr<ITextureView> m_pTextureView;
};

