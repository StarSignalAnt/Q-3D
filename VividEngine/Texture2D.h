#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>

#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Texture.h>
#include <Graphics/GraphicsEngine/interface/TextureView.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>

#include <TextureUtilities.h>
#include "Vivid.h"
#include <thread>
#include <mutex>

using namespace Diligent;

class RenderTarget2D; // Forward declaration

struct CachedTexture {
	RefCntAutoPtr<ITexture> texture;
	RefCntAutoPtr<ITextureView> textureView;
	int width;
	int height;
};

struct BinaryCacheHeader {
	int width;
	int height;
	int channels; // Always 4 for RGBA
	int format; // Store the original texture format
	int mipLevels;
	size_t dataSize;
};

class Texture2D
{
public:

	Texture2D(RenderTarget2D* target);
	Texture2D(std::string path, bool srgb = false);
	Texture2D(int w, int h, float* data, int bpp);
	Texture2D(int w, int h,char* data, int bpp);
	void Update(float* data);

	RefCntAutoPtr<ITextureView> GetView() {
		return m_pTextureView;
	}

	RefCntAutoPtr<ITexture> GetTex() {
		return m_pTexture;
	}

	std::string GetPath() {
		return m_Path;
	}

	// Static methods for cache management
	static void ClearMemoryCache();
	static void ClearBinaryCache(const std::string& path);

private:
	int m_Width;
	int m_Height;
	RefCntAutoPtr<ITexture> m_pTexture;
	RefCntAutoPtr<ITextureView> m_pTextureView;
	std::string m_Path;

	// Static cache storage
	static std::unordered_map<std::string, std::shared_ptr<CachedTexture>> s_textureCache;
	static std::mutex s_cacheMutex;

	// Helper methods
	std::string GetBinaryCachePath(const std::string& texturePath);
	bool LoadFromBinaryCache(const std::string& path);
	void SaveToBinaryCache(const std::string& path);
	std::vector<uint8_t> ExtractTextureData();
};