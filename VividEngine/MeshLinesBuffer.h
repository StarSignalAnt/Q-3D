#pragma once
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "BasicMath.hpp"

using namespace Diligent;
class MeshLines;

class MeshLinesBuffer
{
public:

	void SetMeshLines(MeshLines* mesh);
	RefCntAutoPtr<IBuffer> GetVertexBuffer() {
		return VertexBuffer;
	}
	RefCntAutoPtr<IBuffer> GetIndexBuffer() {
		return IndexBuffer;
	}

private:
	RefCntAutoPtr<IBuffer> VertexBuffer;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	MeshLines* m_Mesh;

};

