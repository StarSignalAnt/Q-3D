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
protected:

	GraphNode* m_Light = nullptr;
	RefCntAutoPtr<IShader> m_VS;
	RefCntAutoPtr<IShader> m_PS;
	RefCntAutoPtr<IPipelineState> m_Pipeline;
	RefCntAutoPtr<IPipelineState> m_PipelineAdd;
	RefCntAutoPtr<IBuffer> m_UniformBuffer;
	RefCntAutoPtr<IShaderResourceBinding> m_SRB;
	RefCntAutoPtr<IShaderResourceBinding> m_SRBAdd;
	glm::mat4 m_Bones[100];
	int m_IndexCount = 0;
	RefCntAutoPtr<IBuffer> m_Buffers[128];
	glm::mat4 m_RenderMatrices[256];
	Texture2D* m_Textures[256];
	glm::vec4 m_CameraExt;
	glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	//Tex2D




};

