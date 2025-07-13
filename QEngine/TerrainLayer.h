#pragma once

#include "BasicMath.hpp"

using namespace Diligent;

namespace Q3D::Engine::Texture {
	class Texture2D;
}

class Texture2D;
class TextureCube;
class MaterialMeshPBR;
class PixelMap;

class TerrainLayer
{
public:

	TerrainLayer();
	void PlotBrush(float x, float y, int w, int h, float strength);
	void FillBrush(float4 value);
	void SetLayerMap(Q3D::Engine::Texture::Texture2D* tex) {
		m_LayerMap = tex;
	}
	void SetPixels(PixelMap* pixels);
	Q3D::Engine::Texture::Texture2D* GetLayerMap() {
		return m_LayerMap;
	}
	PixelMap* GetPixels() {
		return m_LayerPixMap;
	}
	Q3D::Engine::Texture::Texture2D* GetColor() {
		return m_Color;
	}
	void SetColor(Q3D::Engine::Texture::Texture2D* color) {
		m_Color = color;
	}
	void SetNormal(Q3D::Engine::Texture::Texture2D* norm) {
		m_Normal = norm;
	}
	void SetSpecular(Q3D::Engine::Texture::Texture2D* spec) {
		m_Specular = spec;
	}
	Q3D::Engine::Texture::Texture2D* GetNormal() {
		return m_Normal;
	}
	Q3D::Engine::Texture::Texture2D* GetSpec() {
		return m_Specular;
	}
	void Create();
private:

	Q3D::Engine::Texture::Texture2D* m_Color = nullptr;
	Q3D::Engine::Texture::Texture2D* m_Normal = nullptr;
	Q3D::Engine::Texture::Texture2D* m_Specular = nullptr;
	MaterialMeshPBR* m_Material = nullptr;
	Q3D::Engine::Texture::Texture2D* m_LayerMap = nullptr;
	PixelMap* m_LayerPixMap = nullptr;


};

