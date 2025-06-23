#pragma once

#include "BasicMath.hpp"

using namespace Diligent;

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
	void SetLayerMap(Texture2D* tex) {
		m_LayerMap = tex;
	}
	void SetPixels(PixelMap* pixels);
	Texture2D* GetLayerMap() {
		return m_LayerMap;
	}
	PixelMap* GetPixels() {
		return m_LayerPixMap;
	}
	Texture2D* GetColor() {
		return m_Color;
	}
	void SetColor(Texture2D* color) {
		m_Color = color;
	}
	void SetNormal(Texture2D* norm) {
		m_Normal = norm;
	}
	void SetSpecular(Texture2D* spec) {
		m_Specular = spec;
	}
	Texture2D* GetNormal() {
		return m_Normal;
	}
	Texture2D* GetSpec() {
		return m_Specular;
	}
	void Create();
private:

	Texture2D* m_Color = nullptr;
	Texture2D* m_Normal = nullptr;
	Texture2D* m_Specular = nullptr;
	MaterialMeshPBR* m_Material = nullptr;
	Texture2D* m_LayerMap = nullptr;
	PixelMap* m_LayerPixMap = nullptr;


};

