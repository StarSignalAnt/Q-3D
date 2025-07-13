#include "pch.h"
#include "TextureCube.h"
#include "Graphics/GraphicsEngine/interface/Texture.h" // Include Diligent Engine's Texture header
#include <vector>
#include <algorithm>
#include "QEngine.h"
#include "VFile.h"
struct SubresourceData
{
    Uint32 Stride;
    const void* pData;
};

struct Vec3 {
    float r, g, b, a;
};



TextureCube::TextureCube(std::string path) {


    VFile* file = new VFile(path.c_str(), FileMode::Read);

    int w = file->ReadInt();
    int h = file->ReadInt();

    m_Path = path;

    m_Width = w;
    m_Height = h;

    std::vector<FaceData*> faces;

    for (int i = 0; i < 6; i++) {

        FaceData* data = new FaceData;
        data->Data = new float[w * h * 4];
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {

                int loc = (y * w * 3) + (x * 3);
                unsigned char r = (unsigned char)file->ReadByte();
                unsigned char g = (unsigned char)file->ReadByte();
                unsigned char b = (unsigned char)file->ReadByte();

                float fr = (float)((float)(r) / 255.0f);
                float fg = (float)((float)(g) / 255.0f);
                float fb = (float)((float)(b) / 255.0f);

                int fd = (y * w * 4) + (x * 4);
                data->Data[fd++] = fr;
                data->Data[fd++] = fg;
                data->Data[fd++] = fb;
                data->Data[fd++] = 1.0f;


            }
        }

        faces.push_back(data);
        m_Faces.push_back(data);

    }

    file->Close();

    TextureDesc TexDesc;
    TexDesc.Name = "CubeMapTexture"; // Name of the texture
    TexDesc.Type = RESOURCE_DIM_TEX_CUBE; // Cube map type
    TexDesc.Width = w;
    TexDesc.Height = h;
    TexDesc.Format = TEX_FORMAT_RGBA32_FLOAT; ;// Engine::m_pSwapChain->GetCurrentBackBufferRTV()->GetDesc().Format;  //DXGI_FORMAT_R32G32B32A32_FLOAT; // Assuming RGBA EXR format
    TexDesc.BindFlags = BIND_SHADER_RESOURCE;
    TexDesc.Usage = USAGE_IMMUTABLE;
    TexDesc.ArraySize = 6;
    TexDesc.MipLevels = 1;
    std::vector<TextureSubResData> res;

    for (auto sd : faces) {

        TextureSubResData ndata;
        ndata.pData = sd->Data;
        ndata.Stride = (w * sizeof(float) * 4.0f);

        res.push_back(ndata);


    }

    TextureData data;
    data.NumSubresources = 6;
    data.pSubResources = res.data();


    RefCntAutoPtr<ITexture> pTexture;
    Q3D::Engine::QEngine::GetDevice()->CreateTexture(TexDesc, &data, &pTexture);
    m_Texture = pTexture;
    m_TexView = pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);



}
