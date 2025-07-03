#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // Functions like glm::translate, glm::rotate, glm::perspective
#include <glm/gtc/type_ptr.hpp> 
#include <vector>
#include "GraphNode.h"

#if D3D11_SUPPORTED
#    include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#endif
#if D3D12_SUPPORTED
#    include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#endif
#if GL_SUPPORTED
#    include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#endif
#if VULKAN_SUPPORTED
#    include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#endif
#if METAL_SUPPORTED
#    include "Graphics/GraphicsEngineMetal/interface/EngineFactoryMtl.h"
#endif

#ifdef GetObject
#    undef GetObject
#endif
#ifdef CreateWindow
#    undef CreateWindow
#endif

#include <optional>
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "BasicMath.hpp"

class Texture2D;
class TextureCube;

using namespace Diligent;
class RenderMaterial
{
public:

	void SetVertexShader(std::string path);
	void SetPixelShader(std::string path);
	RefCntAutoPtr<IBuffer> CreateUniform(int size, std::string path);
	void SetBuffer(RefCntAutoPtr<IBuffer> buffer, int index);
	void SetMatrix(glm::mat4 matrix, int index);	
	void SetTexture(Texture2D* texture, int index) {
		m_Textures[index] = texture;
	};
	void SetCameraExt(glm::vec4 ext) { m_CameraExt = ext; }
	virtual void Bind(bool add) = 0;
	virtual void Render() = 0;
	void SetCameraPosition(glm::vec3 position) { m_CameraPosition = position; }
	void SetIndexCount(int count) { m_IndexCount = count; }	
	void SetLight(GraphNode* light) { m_Light = light; };
	void SetBone(glm::mat4 bone, int index) {

			if (index < 100)
				m_Bones[index] = bone;
	}
	glm::mat4 GetBone(int index) const {
		if (index < 100)
			return m_Bones[index];
		return glm::mat4(1.0f);
	}
	virtual void Save(std::string path) {};
	virtual void Load(std::string path) {};
	std::string GetName() {
		return m_Name;
	}
	void SetName(std::string name) {
		m_Name = name;
	}
	std::string GetPath() {
		return m_Path;
	}
	void SetIndex(int val, int idx) {
		m_Indices[idx] = val;
	}
	void SetPSO(RefCntAutoPtr<IPipelineState> pso) {
		m_Pipeline = pso;
	}
	void SetPSOAdd(RefCntAutoPtr<IPipelineState> pso)
	{
		m_PipelineAdd = pso;
	}

	void SetSRB(RefCntAutoPtr<IShaderResourceBinding> srb) {
		m_SRB = srb;
	}
	void SetSRBAdd(RefCntAutoPtr<IShaderResourceBinding> srb) {
		m_SRBAdd = srb;
	}


	RefCntAutoPtr<IPipelineState> GetPSO() {
		return m_Pipeline;
	}
	RefCntAutoPtr<IPipelineState> GetPSOAdd() {
		return m_PipelineAdd;
	}
	RefCntAutoPtr<IShaderResourceBinding> GetSRB() {
		return m_SRB;
	}
	RefCntAutoPtr<IShaderResourceBinding> GetSRBAdd() {
		return m_SRBAdd;
	}
	RefCntAutoPtr<IBuffer> GetUniformBuf() {
		return m_UniformBuffer;
	}

	void SetUniformBuffer(RefCntAutoPtr<IBuffer> buf) {
		m_UniformBuffer = buf;
	}

protected:

	GraphNode* m_Light = nullptr;
	RefCntAutoPtr<IShader> m_VS;
	RefCntAutoPtr<IShader> m_PS;
	RefCntAutoPtr<IPipelineState> m_Pipeline;
	RefCntAutoPtr<IPipelineState> m_PipelineAdd;
	RefCntAutoPtr<IBuffer> m_UniformBuffer;
	RefCntAutoPtr<IShaderResourceBinding> m_SRB;
	RefCntAutoPtr<IShaderResourceBinding> m_SRBAdd;
	std::string m_Name = "";
	std::string m_Path = "";
	glm::mat4 m_Bones[100];
	int m_IndexCount = 0;
	int m_Indices[128];
	RefCntAutoPtr<IBuffer> m_Buffers[128];
	glm::mat4 m_RenderMatrices[256];
	Texture2D* m_Textures[256];
	glm::vec4 m_CameraExt;
	glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	//Tex2D




};

