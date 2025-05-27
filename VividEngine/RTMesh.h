#pragma once

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
#include <thread>
#include <mutex>

using namespace Diligent;
class Mesh3D;
class Node;


class RTMesh
{
public:

	RTMesh(Mesh3D* mesh);
	void UpdateBLAS();
	std::string GetName() {
		return m_Name;
	}
	RefCntAutoPtr<IBottomLevelAS> GetBLAS() {
		return m_BLAS;
	}
	Node* GetNode() {
		return m_Node;
	}
	Mesh3D* GetMesh() {
		return m_Mesh;
	}
	bool GetDynamic() {
		return m_Dynamic;
	}
	bool HasPrimitive() {
		return m_HasInstance;
	}
	void SetPrimitive(TLASBuildInstanceData inst) {
		m_Instance = inst;
		m_HasInstance = true;
	}
	TLASBuildInstanceData& GetPrimitive() {
		return m_Instance;
	}
private:

	std::string m_Name;
	RefCntAutoPtr<IBottomLevelAS> m_BLAS;
	RefCntAutoPtr<IBuffer> VertexBuffer;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	Node* m_Node;
	Mesh3D* m_Mesh;
	bool m_Dynamic = false;
	float* m_VertData = nullptr;
	RefCntAutoPtr<IBuffer> pScratchBuffer;
	TLASBuildInstanceData m_Instance;
	bool m_HasInstance = false;
};

