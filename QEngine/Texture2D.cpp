#include "Texture2D.h"
#include "RenderTarget2D.h"
#include <filesystem>
#include <iostream>
#include <cstdlib> // For std::getenv
#include "PixelMap.h"
// Static member definitions
std::unordered_map<std::string, std::shared_ptr<CachedTexture>> Texture2D::s_textureCache;
std::mutex Texture2D::s_cacheMutex;

// Helper function to get the application's cache directory path
// This avoids a dependency on Qt for path resolution.
std::string GetAppCacheDirectory()
{
    std::filesystem::path cachePath;
#ifdef _WIN32
    const char* localAppData = std::getenv("LOCALAPPDATA");
    if (localAppData)
    {
        cachePath = localAppData;
    }
#else // For Linux, macOS, and other UNIX-like systems
    const char* home = std::getenv("HOME");
    if (home)
    {
        cachePath = std::filesystem::path(home) / ".cache";
    }
#endif

    // Append a directory for our specific application to avoid clutter.
    // "Vivid" is used here, similar to how Qt would use the application name.
    if (!cachePath.empty())
    {
        cachePath /= "Vivid";
    }

    return cachePath.string();
}


Texture2D::Texture2D(std::string path, bool srgb)
{
    m_Path = path;

    // STEP 1: Load the image using PixelMap, forcing it into 8-bit (UINT8) mode.
    // This ensures we get the raw, unmodified pixel data from the file.
    PixelMap* pmap = new PixelMap(path, PixelMapDataType::UINT8);

    auto col = pmap->GetIntColor(5, 5);

    if (pmap->GetData() == nullptr)
    {
        std::cerr << "Error loading texture file via PixelMap: " << path << std::endl;
        delete pmap;
        return;
    }

    m_Width = pmap->GetWidth();
    m_Height = pmap->GetHeight();

    TextureDesc TexDesc;

    // STEP 2: Choose the correct 8-bit texture format based on the srgb flag.
    // The GPU hardware will handle the interpretation of this data perfectly.
    if (srgb)
    {
        // For color textures, use the 8-bit SRGB format.
        TexDesc.Format = TEX_FORMAT_RGBA8_UNORM_SRGB;
        TexDesc.Name = "sRGB Color Texture";
    }
    else
    {
        // For data textures (like normal maps), use the 8-bit linear format.
        // This is the key to fixing the 0.501 vs 0.5 issue.
        TexDesc.Format = TEX_FORMAT_RGBA8_UNORM;
        TexDesc.Name = "Linear Data Texture";
    }

    TexDesc.Type = RESOURCE_DIM_TEX_2D;
    TexDesc.Width = m_Width;
    TexDesc.Height = m_Height;
    TexDesc.MipLevels = 1; // No mipmaps to eliminate it as a variable.
    TexDesc.BindFlags = BIND_SHADER_RESOURCE;
    TexDesc.Usage = USAGE_IMMUTABLE;

    // STEP 3: Provide the raw 8-bit data to the GPU.
    TextureSubResData subresourceData;
    subresourceData.pData = pmap->GetData();
    // The stride is the width * number of bytes per pixel (RGBA = 4 bytes).
    subresourceData.Stride = m_Width * pmap->GetBPP();

    TextureData initialData;
    initialData.pSubResources = &subresourceData;
    initialData.NumSubresources = 1;

    // Create the texture on the GPU.
    QEngine::m_pDevice->CreateTexture(TexDesc, &initialData, &m_pTexture);

    // The PixelMap object is no longer needed after its data is sent to the GPU.
    delete pmap;

    if (m_pTexture)
    {
        m_pTextureView = m_pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
    }
    else
    {
        std::cerr << "Failed to create Diligent texture for: " << path << std::endl;
    }
    return;
    /*
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(s_cacheMutex);

    // Check memory cache first
    auto cacheIt = s_textureCache.find(path);
    if (cacheIt != s_textureCache.end()) {
        // Found in memory cache, use cached data
        auto cachedTex = cacheIt->second;
        m_pTexture = cachedTex->texture;
        m_pTextureView = cachedTex->textureView;
        m_Width = cachedTex->width;
        m_Height = cachedTex->height;
        return;
    }

    // Not in memory cache, try binary cache
    if (LoadFromBinaryCache(path)) {
        // Successfully loaded from binary cache
        // Add to memory cache for future use
        auto cachedTex = std::make_shared<CachedTexture>();
        cachedTex->texture = m_pTexture;
        cachedTex->textureView = m_pTextureView;
        cachedTex->width = m_Width;
        cachedTex->height = m_Height;
        s_textureCache[path] = cachedTex;
        return;
    }
    */

    // Not in any cache, load from original file
    TextureLoadInfo loadInfo;
    loadInfo.BindFlags = BIND_FLAGS::BIND_SHADER_RESOURCE;

    if (srgb) {
        loadInfo.Format = TEX_FORMAT_RGBA8_UNORM_SRGB;
        //loadInfo.MipLevels = 8;
        loadInfo.GenerateMips = true; // Generate mips ONLY for color textures
        loadInfo.MipLevels = 16;
    }
    else {
        //loadInfo.GenerateMips = false; 

        loadInfo.Format = TEX_FORMAT_RGBA8_UNORM;
    }
    loadInfo.IsSRGB = srgb;
    

    //loadInfo.MipFilter = TEXTURE_LOAD_MIP_FILTER_BOX_AVERAGE;
    loadInfo.GenerateMips = true;
    loadInfo.MipLevels = 16;

    QEngine::m_pImmediateContext->Flush();
    CreateTextureFromFile(path.c_str(), loadInfo, QEngine::m_pDevice, &m_pTexture);
    QEngine::m_pImmediateContext->Flush();

    m_pTextureView = m_pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
    QEngine::m_pImmediateContext->Flush();

    m_Width = m_pTexture->GetDesc().Width;
    m_Height = m_pTexture->GetDesc().Height;

    // Save to binary cache for future loads
    SaveToBinaryCache(path);

    // Add to memory cache
    auto cachedTex = std::make_shared<CachedTexture>();
    cachedTex->texture = m_pTexture;
    cachedTex->textureView = m_pTextureView;
    cachedTex->width = m_Width;
    cachedTex->height = m_Height;
    s_textureCache[path] = cachedTex;
}

Texture2D::Texture2D(RenderTarget2D* target) {
    m_pTexture = target->GetTexture();
    m_pTextureView = target->GetView();
    m_Width = m_pTexture->GetDesc().Width;
    m_Height = m_pTexture->GetDesc().Height;
}

Texture2D::Texture2D(int w, int h, float* data, int bpp)
{
    m_Width = w;
    m_Height = h;

    TextureDesc TexDesc;
    TexDesc.Name = "Float Tex2D";
    TexDesc.Type = RESOURCE_DIM_TEX_2D;
    TexDesc.Width = w;
    TexDesc.Height = h;
    TexDesc.Format = TEX_FORMAT_RGBA32_FLOAT;
    TexDesc.BindFlags = BIND_SHADER_RESOURCE;
    TexDesc.Usage = USAGE_DEFAULT;
    TexDesc.MipLevels = 1;

    TextureSubResData adata;
    adata.pData = data;
    adata.Stride = w * sizeof(float) * 4;

    TextureData tdata;
    tdata.NumSubresources = 1;
    tdata.pSubResources = &adata;

    RefCntAutoPtr<ITexture> pTexture;
    QEngine::m_pDevice->CreateTexture(TexDesc, &tdata, &pTexture);
    m_pTexture = pTexture;
    m_pTextureView = pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
}


Texture2D::Texture2D(int w, int h, char* data, int bpp)
{
    m_Width = w;
    m_Height = h;

    TextureDesc TexDesc;
    TexDesc.Name = "Float Tex2D";
    TexDesc.Type = RESOURCE_DIM_TEX_2D;
    TexDesc.Width = w;
    TexDesc.Height = h;
    TexDesc.Format = TEXTURE_FORMAT::TEX_FORMAT_RGBA32_FLOAT;
    TexDesc.BindFlags = BIND_SHADER_RESOURCE;
    TexDesc.Usage = USAGE_DEFAULT;
    TexDesc.MipLevels = 1;

    TextureSubResData adata;
    adata.pData = data;
    adata.Stride = w * sizeof(float) * 4;

    TextureData tdata;
    tdata.NumSubresources = 1;
    tdata.pSubResources = &adata;

    RefCntAutoPtr<ITexture> pTexture;
    QEngine::m_pDevice->CreateTexture(TexDesc, &tdata, &pTexture);
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

    Diligent::TextureSubResData subresourceData;
    subresourceData.pData = data;
    subresourceData.Stride = updateBox.MaxX * 4 * sizeof(float);

    QEngine::m_pImmediateContext->UpdateTexture(m_pTexture, 0, 0, updateBox, subresourceData,
        Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pTextureView = m_pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
}

std::string Texture2D::GetBinaryCachePath(const std::string& texturePath) {
    // Mimics the behavior of PropertyTexture::getCachePath to use a central cache.
    std::filesystem::path originalPath(texturePath);
    std::string cacheFileName = originalPath.stem().string() + ".texcache";

    std::string appCacheDir = GetAppCacheDirectory();
    if (appCacheDir.empty())
    {
        // Fallback to old behavior: save next to the original file.
        return (originalPath.parent_path() / cacheFileName).string();
    }

    // Use the same "/textures" subfolder as PropertyTexture.
    std::filesystem::path cacheDir = std::filesystem::path(appCacheDir) / "textures";

    // Create the directory if it does not exist.
    if (!std::filesystem::exists(cacheDir))
    {
        std::filesystem::create_directories(cacheDir);
    }

    return (cacheDir / cacheFileName).string();
}

bool Texture2D::LoadFromBinaryCache(const std::string& path) {
    std::string cachePath = GetBinaryCachePath(path);

    // Check if cache file exists
    if (!std::filesystem::exists(cachePath)) {
        return false;
    }

    // Check if original file is newer than cache, if the original exists
    if (std::filesystem::exists(path)) {
        auto originalTime = std::filesystem::last_write_time(path);
        auto cacheTime = std::filesystem::last_write_time(cachePath);
        if (originalTime > cacheTime) {
            return false; // Original is newer, so the cache is stale.
        }
    }

    try {
        std::ifstream file(cachePath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // Read header
        BinaryCacheHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (file.fail() || header.channels != 4) {
            file.close();
            return false;
        }

        // Read texture data
        std::vector<uint8_t> textureData(header.dataSize);
        file.read(reinterpret_cast<char*>(textureData.data()), header.dataSize);
        file.close();

        if (file.fail()) {
            return false;
        }

        // Create texture from cached data using the original format and settings
        TextureDesc TexDesc;
        TexDesc.Name = "Cached Texture";
        TexDesc.Type = RESOURCE_DIM_TEX_2D;
        TexDesc.Width = header.width;
        TexDesc.Height = header.height;
        TexDesc.Format = static_cast<TEXTURE_FORMAT>(header.format); // Use original format
        TexDesc.BindFlags = BIND_SHADER_RESOURCE;
        TexDesc.Usage = USAGE_DEFAULT;
        TexDesc.MipLevels = header.mipLevels;

        // Calculate bytes per pixel based on the stored format
        int bytesPerPixel;
        switch (static_cast<TEXTURE_FORMAT>(header.format)) {
        case TEX_FORMAT_RGBA16_UNORM:
            bytesPerPixel = 8;
            break;
        case TEX_FORMAT_RGBA32_FLOAT:
            bytesPerPixel = 16;
            break;
        default: // Covers RGBA8_UNORM, RGBA8_UNORM_SRGB etc.
            bytesPerPixel = 4;
            break;
        }


        TextureSubResData adata;
        adata.pData = textureData.data();
        adata.Stride = header.width * bytesPerPixel;

        TextureData tdata;
        tdata.NumSubresources = 1;
        tdata.pSubResources = &adata;

        RefCntAutoPtr<ITexture> pTexture;
        QEngine::m_pDevice->CreateTexture(TexDesc, &tdata, &pTexture);

        if (!pTexture) {
            return false;
        }

        m_pTexture = pTexture;
        m_pTextureView = pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
        m_Width = header.width;
        m_Height = header.height;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading from binary cache: " << e.what() << std::endl;
        return false;
    }
}

void Texture2D::SaveToBinaryCache(const std::string& path) {
    try {
        std::string cachePath = GetBinaryCachePath(path);

        // Extract texture data
        std::vector<uint8_t> textureData = ExtractTextureData();
        if (textureData.empty()) {
            return; // Failed to extract data
        }

        // Write cache file
        std::ofstream file(cachePath, std::ios::binary);
        if (!file.is_open()) {
            return;
        }

        // Write header with original texture properties
        BinaryCacheHeader header;
        header.width = m_Width;
        header.height = m_Height;
        header.channels = 4;
        header.format = static_cast<int>(m_pTexture->GetDesc().Format);
        header.mipLevels = m_pTexture->GetDesc().MipLevels;
        header.dataSize = textureData.size();

        file.write(reinterpret_cast<const char*>(&header), sizeof(header));
        file.write(reinterpret_cast<const char*>(textureData.data()), textureData.size());
        file.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving to binary cache: " << e.what() << std::endl;
    }
}

std::vector<uint8_t> Texture2D::ExtractTextureData() {
    std::vector<uint8_t> data;

    try {
        // Get original texture description
        const TextureDesc& originalDesc = m_pTexture->GetDesc();

        // Create a staging texture with exact same format as original
        TextureDesc stagingDesc = originalDesc;
        stagingDesc.Usage = USAGE_STAGING;
        stagingDesc.BindFlags = BIND_NONE;
        stagingDesc.CPUAccessFlags = CPU_ACCESS_READ;
        stagingDesc.MipLevels = 1; // Only get the base mip level

        RefCntAutoPtr<ITexture> stagingTexture;
        QEngine::m_pDevice->CreateTexture(stagingDesc, nullptr, &stagingTexture);

        if (!stagingTexture) {
            std::cerr << "Failed to create staging texture" << std::endl;
            return data;
        }

        // Copy texture to staging - copy only the base mip level
        CopyTextureAttribs copyAttribs;
        copyAttribs.pSrcTexture = m_pTexture;
        copyAttribs.pDstTexture = stagingTexture;
        copyAttribs.SrcMipLevel = 0;
        copyAttribs.DstMipLevel = 0;
        copyAttribs.SrcSlice = 0;
        copyAttribs.DstSlice = 0;

        QEngine::m_pImmediateContext->CopyTexture(copyAttribs);
        QEngine::m_pImmediateContext->Flush();

        // Map staging texture and read data
        MappedTextureSubresource mappedData;
        QEngine::m_pImmediateContext->MapTextureSubresource(stagingTexture, 0, 0, MAP_READ, MAP_FLAG_NONE, nullptr, mappedData);

        if (mappedData.pData) {
            // Calculate bytes per pixel based on format
            int bytesPerPixel;
            switch (originalDesc.Format) {
            case TEX_FORMAT_RGBA16_UNORM:
                bytesPerPixel = 8; // 2 bytes per channel * 4 channels
                break;
            case TEX_FORMAT_RGBA8_UNORM:
            case TEX_FORMAT_RGBA8_UNORM_SRGB:
                bytesPerPixel = 4; // 1 byte per channel * 4 channels
                break;
            case TEX_FORMAT_RGBA32_FLOAT:
                bytesPerPixel = 16; // 4 bytes per channel * 4 channels
                break;
            default:
                bytesPerPixel = 4; // Default fallback
                break;
            }

            size_t dataSize = m_Width * m_Height * bytesPerPixel;
            data.resize(dataSize);

            // Copy row by row to handle potential row padding
            const uint8_t* srcData = static_cast<const uint8_t*>(mappedData.pData);
            uint8_t* dstData = data.data();

            size_t rowSize = m_Width * bytesPerPixel;

            for (int y = 0; y < m_Height; ++y) {
                memcpy(dstData + y * rowSize, srcData + y * mappedData.Stride, rowSize);
            }

            QEngine::m_pImmediateContext->UnmapTextureSubresource(stagingTexture, 0, 0);
        }
        else {
            std::cerr << "Failed to map staging texture" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error extracting texture data: " << e.what() << std::endl;
    }

    return data;
}

// Static methods for cache management
void Texture2D::ClearMemoryCache() {
    std::lock_guard<std::mutex> lock(s_cacheMutex);
    s_textureCache.clear();
}