#pragma once

#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "BasicMath.hpp"

using namespace Diligent;

class Mesh3D;

class MeshBuffer
{
public:

	MeshBuffer();
	void SetMesh(Mesh3D* mesh);
	void SetMeshFast(float* v, Uint32* i,int verts,int tris);
	void Build();
	RefCntAutoPtr<IBuffer> GetVertexBuffer() {
		return VertexBuffer;
	}
	RefCntAutoPtr<IBuffer> GetIndexBuffer() {
		return IndexBuffer;
	}

private:
	RefCntAutoPtr<IBuffer> VertexBuffer;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	Mesh3D* m_Mesh;
};

