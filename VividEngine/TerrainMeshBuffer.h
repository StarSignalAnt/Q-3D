#pragma once
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "BasicMath.hpp"

using namespace Diligent;
class TerrainMesh;

class TerrainMeshBuffer
{
public:

	TerrainMeshBuffer(TerrainMesh* mesh);
	RefCntAutoPtr<IBuffer> GetVertexBuffer() {
		return VertexBuffer;
	}
	
	RefCntAutoPtr<IBuffer> GetTriangleBuffer() {
		return IndexBuffer;
	}
	void Update();

private:
	RefCntAutoPtr<IBuffer> VertexBuffer;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	TerrainMesh* m_Mesh;
	float* m_Data = nullptr;
};

