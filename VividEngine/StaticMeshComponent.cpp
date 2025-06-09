#include "StaticMeshComponent.h"
#include "Vivid.h"

void StaticMeshComponent::Finalize() {

	for (auto sub : m_SubMeshes) {


		float* data = new float[sub->m_Vertices.size() * 19];

		int i = 0;
		for (auto v : sub->m_Vertices) {

			data[i++] = v.position.x;
			data[i++] = v.position.y;
			data[i++] = v.position.z;
			data[i++] = v.color.x;
			data[i++] = v.color.y;
			data[i++] = v.color.z;
			data[i++] = v.color.w;
			data[i++] = v.uv.x;
			data[i++] = v.uv.y;
			data[i++] = v.uv.z;
			data[i++] = v.normal.x;
			data[i++] = v.normal.y;
			data[i++] = v.normal.z;
			data[i++] = v.binormal.x;
			data[i++] = v.binormal.y;
			data[i++] = v.binormal.z;
			data[i++] = v.tangent.x;
			data[i++] = v.tangent.y;
			data[i++] = v.tangent.z;

		}

		BufferDesc vbuf;
		vbuf.Name = "Mesh Buffer VB";
		vbuf.Usage = USAGE_IMMUTABLE;
		vbuf.BindFlags = BIND_VERTEX_BUFFER;
		vbuf.Size = (Uint64)(sizeof(Vertex3) * sub->m_Vertices.size());

		BufferData vdata;
		vdata.DataSize = vbuf.Size;
		vdata.pData = data;

		Vivid::m_pDevice->CreateBuffer(vbuf, &vdata, &sub->VertexBuffer);

		BufferDesc ibuf;

		ibuf.Name = "Mesh Buffer IB";
		ibuf.Usage = USAGE_IMMUTABLE;
		ibuf.BindFlags = BIND_INDEX_BUFFER;
		ibuf.Size = (Uint64)(sizeof(Uint32) *sub->m_Triangles.size() * 3);

		Uint32* indices = new Uint32[sub->m_Triangles.size() * 3];

		i = 0;
		for (auto t : sub->m_Triangles)
		{
			indices[i++] = t.v0;
			indices[i++] = t.v1;
			indices[i++] = t.v2;
		}


		BufferData idata;
		idata.DataSize = ibuf.Size;
		idata.pData = indices;

		Vivid::m_pDevice->CreateBuffer(ibuf, &idata, &sub->IndexBuffer);

	}

}
