#pragma once
#include <string>

#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "BasicMath.hpp"

using namespace Diligent;

class Texture2D;
class RenderTargetCube;
class TextureCube;

class MaterialBase
{
public:

	MaterialBase();
	virtual void Create();
	void SetVertexShader(std::string path);
	void SetPixelShader(std::string path);
	RefCntAutoPtr<IBuffer> CreateUniform(int size, std::string name);
	virtual void Bind(bool sp);
	RefCntAutoPtr<IShaderResourceBinding> GetSRB() {
		return m_SRB;
	}
	RefCntAutoPtr<IShaderResourceBinding> GetSecondPassSRB() {
		return m_SecondPassSRB;
	}
	void SetAux(Texture2D* aux) {
		m_Aux = aux;
	}
	void SetDiffuse(Texture2D* texture);
	void SetSpecular(Texture2D* texture);
	void SetNormals(Texture2D* texture);
	void SetRough(Texture2D* texture) {
		m_Roughness = texture;
		Updated();
	}
	void SetMetal(Texture2D* metal) {
		m_Metal = metal;
		Updated();
	}
	void SetAmbient(Texture2D* ambient) {
		m_Ambient = ambient;
	}
	void SetEnvironment(RenderTargetCube* env);
	void SetEnvironmentTex(TextureCube* map) {
		m_EnvironmentTex = map;
		Updated();
	}
	TextureCube* GetEnvironmentTex() {
		return m_EnvironmentTex;
	}
	Texture2D* GetDiffuse() {
		return m_Diffuse;
	}
	Texture2D* GetSpecular() {
		return m_Specular;
	}
	Texture2D* GetNormal() {
		return m_Normal;
	}
	Texture2D* GetRough() {
		return m_Roughness;
	}
	Texture2D* GetMetal() {
		return m_Metal;
	}
	Texture2D* GetAmbient() {
		return m_Ambient;
	}
	
	virtual void SaveMaterial(std::string path);
	static MaterialBase* LoadMaterial(std::string path);
	std::string GetPath() {
		return m_FullPath;
	}
	void SetPath(std::string path) {
		m_FullPath = path;
	}
	float4 GetDiffuseColor() {
		return m_DiffuseColor;
	}
	float4 GetSpecularColor() {
		return m_SpecularColor;
	}
	void SetDiffuseColor(float4 diff) {
		m_DiffuseColor = diff;
	}
	void SetSpecularColor(float4 spec) {
		m_SpecularColor = spec;
	}
	void SetEmissive(Texture2D* emissive) {
		m_Emissive = emissive;
	}
	Texture2D* GetEmissive() {
		return m_Emissive;
	}
	void Updated();
	void CopyMaps(MaterialBase* other);
protected:

	//Textures
	Texture2D* m_Diffuse = nullptr;
	Texture2D* m_Normal = nullptr;
	Texture2D* m_Specular = nullptr;
	Texture2D* m_Emissive = nullptr;
	RenderTargetCube* m_Environment = nullptr;
	TextureCube* m_EnvironmentTex = nullptr;
	Texture2D* m_Aux = nullptr;
	//PBR
	Texture2D* m_Roughness;
	Texture2D* m_Metal;
	Texture2D* m_Ambient;

	float4 m_DiffuseColor;
	float4 m_SpecularColor;

	bool m_CastShadows = true;
	bool m_ReceiveShadows = true;

	//Basic
	RefCntAutoPtr<IBuffer> BasicUniform;
	RefCntAutoPtr<IBuffer> BasicUniform2;
	//
	RefCntAutoPtr<IShader> m_VS;
	RefCntAutoPtr<IShader> m_PS;
	RefCntAutoPtr<IPipelineState> m_Pipeline;
	RefCntAutoPtr<IPipelineState> m_SecondPassPipeline;
	RefCntAutoPtr<IBuffer> m_UniformBuffer;
	RefCntAutoPtr<IShaderResourceBinding> m_SRB;
	RefCntAutoPtr<IShaderResourceBinding> m_SecondPassSRB;
	std::string m_FullPath;
	bool m_Updated = false;

};

