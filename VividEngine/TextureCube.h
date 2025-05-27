#pragma once
#include <vector>
#include <string>
//#include <Graphics/GraphicsEngine/interface/GraphicsEngine.h>
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Texture.h>
#include <Graphics/GraphicsEngine/interface/TextureView.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
//#include <Common/interface/FileStream.h>

#include <TextureUtilities.h>
#include "Engine.h"

struct FaceData {

	float* Data;

};


class TextureCube
{
public:

	TextureCube(std::string path);
	RefCntAutoPtr<ITextureView> GetView() {
		return m_TexView;
	}
	FaceData* GetFace(int index) {
		return m_Faces[index];
	}
	int GetWidth() {
		return m_Width;
	}
	int GetHeight() {
		return m_Height;
	}
	std::string GetPath() {
		return m_Path;
	}
private:

	std::vector<FaceData*> m_Faces;
	RefCntAutoPtr<ITexture> m_Texture;
	RefCntAutoPtr<ITextureView> m_TexView;
	int m_Width = 0;
	int m_Height = 0;
	std::string m_Path = "";
};

