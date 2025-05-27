#include "pch.h"
#include "TextureCube.h"
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <ImfHeader.h>
#include <ImfChannelList.h>
#include <ImfStringAttribute.h>
#include "Graphics/GraphicsEngine/interface/Texture.h" // Include Diligent Engine's Texture header
#include <vector>
#include <vector>
#include <algorithm>
#include "Engine.h"
#include "VFile.h"
struct SubresourceData
{
    Uint32 Stride;
    const void* pData;
};

struct Vec3 {
    float r, g, b, a;
};

void resizeImage(const Imf::Array2D<Imf::Rgba>& src, int srcWidth, int srcHeight, Imf::Array2D<Imf::Rgba>& dst, int dstWidth, int dstHeight) {
    for (int y = 0; y < dstHeight; ++y) {
        for (int x = 0; x < dstWidth; ++x) {
            int srcX = static_cast<int>((float)x / dstWidth * srcWidth);
            int srcY = static_cast<int>((float)y / dstHeight * srcHeight);
            dst[y][x] = src[srcY][srcX];
        }
    }
}


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

                int loc = (y*w*3)+(x*3);
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
        ndata.Stride = (w*sizeof(float)*4.0f);

        res.push_back(ndata);


    }

    TextureData data;
    data.NumSubresources = 6;
    data.pSubResources = res.data();


    RefCntAutoPtr<ITexture> pTexture;
    Engine::m_pDevice->CreateTexture(TexDesc, &data, &pTexture);
    m_Texture = pTexture;
    m_TexView = pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);


    /*
    using namespace Imf;
    using namespace Imath;

    try
    {
        // Open the EXR file
        RgbaInputFile file(path.c_str());
        Box2i dw = file.header().dataWindow();
        int width = dw.max.x - dw.min.x + 1;
        int height = dw.max.y - dw.min.y + 1;

        // Read the RGB channels into separate arrays
        Array2D<Rgba> pixels(width, height);
        file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
        file.readPixels(dw.min.y, dw.max.y);

        int targetSize = std::max(width, height);
        bool use_res = false;
        // Resize the image if necessary
        Array2D<Rgba> resizedPixels(targetSize, targetSize);
        if (width != targetSize || height != targetSize) {
            resizeImage(pixels, width, height, resizedPixels, targetSize, targetSize);
            use_res = true;
        }
        else {
            use_res = false;
            //resizedPixels = pixels;
        }



        // Determine the number of mip levels (for cube maps typically 1)
        int mipLevels = 1;

        // Create a texture descriptor
        TextureDesc TexDesc;
        TexDesc.Name = "CubeMapTexture"; // Name of the texture
        TexDesc.Type = RESOURCE_DIM_TEX_CUBE; // Cube map type
        TexDesc.Width = targetSize;
        TexDesc.Height = targetSize;
        TexDesc.Format = TEX_FORMAT_RGBA32_FLOAT; ;// Engine::m_pSwapChain->GetCurrentBackBufferRTV()->GetDesc().Format;  //DXGI_FORMAT_R32G32B32A32_FLOAT; // Assuming RGBA EXR format
        TexDesc.BindFlags = BIND_SHADER_RESOURCE;
        TexDesc.Usage = USAGE_IMMUTABLE;
        TexDesc.ArraySize = 6;
        TexDesc.MipLevels = 1;
        
        std::vector<SubresourceData> InitData(6);
        // Allocate memory for each cube map face
        if (use_res) {
            
            for (int face = 0; face < 6; ++face)
            {
                InitData[face].Stride = TexDesc.Width * sizeof(float) * 4;
                InitData[face].pData = new char[InitData[face].Stride * TexDesc.Height];
                for (int y = 0; y < TexDesc.Height; ++y)
                {
                    const Rgba* src = &resizedPixels[y][0];
                    float* dest = (float*)(InitData[face].pData) + y * TexDesc.Width * 4;
                    for (int x = 0; x < TexDesc.Width; ++x, ++src, dest += 4)
                    {
                        dest[0] = static_cast<float>(src->r);
                        dest[1] = static_cast<float>(src->g);
                        dest[2] = static_cast<float>(src->b);
                        dest[3] = static_cast<float>(src->a);
                    }
                }
            }
        }
        else {
       
            for (int face = 0; face < 6; ++face)
            {
                InitData[face].Stride = TexDesc.Width * sizeof(float) * 4;
                InitData[face].pData = new char[InitData[face].Stride * TexDesc.Height];
                for (int y = 0; y < TexDesc.Height; ++y)
                {
                    const Rgba* src = &pixels[y][0];
                    float* dest = (float*)(InitData[face].pData) + y * TexDesc.Width * 4;
                    for (int x = 0; x < TexDesc.Width; ++x, ++src, dest += 4)
                    {
                        dest[0] = static_cast<float>(src->r);
                        dest[1] = static_cast<float>(src->g);
                        dest[2] = static_cast<float>(src->b);
                        dest[3] = static_cast<float>(src->a);
                    }
                }
            }
        }

        std::vector<TextureSubResData> res;

        for (auto sd : InitData) {

            TextureSubResData ndata;
            ndata.pData = sd.pData;
            ndata.Stride = sd.Stride;
            
            res.push_back(ndata);


        }

      TextureData data;
      data.NumSubresources = 6;
      data.pSubResources = res.data();
     
     
      // Create the texture

      RefCntAutoPtr<ITexture> pTexture;
      Engine::m_pDevice->CreateTexture(TexDesc, &data, &pTexture);
      m_Texture = pTexture;
      m_TexView = pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

      // Release allocated memory
      for (auto& initData : InitData)
      {
          delete[] initData.pData;
      }

   
      // Return the created texture
      //*ppTexture = pTexture.Detach();
      //return true;
    }
    catch (const std::exception& e)
    {
        // Handle any exceptions (e.g., file not found, EXR file format errors)
        // Print error message or log it
        // Example: std::cerr << "Error loading EXR file: " << e.what() << std::endl;
        return;
    }
    */
}