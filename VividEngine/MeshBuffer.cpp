#include "pch.h"
#include "MeshBuffer.h"
#include "Mesh3D.h"
#include "Engine.h"

MeshBuffer::MeshBuffer() {

	m_Mesh = nullptr;

}

void MeshBuffer::SetMesh(Mesh3D* mesh) {

	m_Mesh = mesh;

}

void MeshBuffer::SetMeshFast(float* v, Uint32* i,int verts,int tris) {

	BufferDesc vbuf;
	vbuf.Name = "Mesh Buffer VB";
	vbuf.Usage = USAGE_IMMUTABLE;
	vbuf.BindFlags = BIND_VERTEX_BUFFER;
	vbuf.Size = (Uint64)(sizeof(Vertex) * verts);

	BufferData vdata;
	vdata.DataSize = vbuf.Size;
	vdata.pData = v;

	Engine::m_pDevice->CreateBuffer(vbuf, &vdata, &VertexBuffer);

	BufferDesc ibuf;

	ibuf.Name = "Mesh Buffer IB";
	ibuf.Usage = USAGE_IMMUTABLE;
	ibuf.BindFlags = BIND_INDEX_BUFFER;
	ibuf.Size = (Uint64)(sizeof(Uint32) * tris * 3);


	BufferData idata;
	idata.DataSize = ibuf.Size;
	idata.pData = i;

	Engine::m_pDevice->CreateBuffer(ibuf, &idata, &IndexBuffer);

}

void MeshBuffer::Build() {



	float* data = new float[m_Mesh->GetVertices().size()*27];

	int i = 0;
	for (auto v : m_Mesh->GetVertices()) {

		data[i++] = v.position.x;
		data[i++] = v.position.y;
		data[i++] = v.position.z;
		data[i++] = v.color.x;
		data[i++] = v.color.y;
		data[i++] = v.color.z;
		data[i++] = v.color.w;
		data[i++] = v.texture.x;
		data[i++] = v.texture.y;
		data[i++] = v.texture.z;
		data[i++] = v.normal.x;
		data[i++] = v.normal.y;
		data[i++] = v.normal.z;
		data[i++] = v.binormal.x;
		data[i++] = v.binormal.y;
		data[i++] = v.binormal.z;
		data[i++] = v.tangent.x;
		data[i++] = v.tangent.y;
		data[i++] = v.tangent.z;
		data[i++] = v.bone_ids.x;
		data[i++] = v.bone_ids.y;
		data[i++] = v.bone_ids.z;
		data[i++] = v.bone_ids.w;
		data[i++] = v.bone_weights.x;
		data[i++] = v.bone_weights.y;
		data[i++] = v.bone_weights.z;
		data[i++] = v.bone_weights.w;

	}

	BufferDesc vbuf;
	vbuf.Name = "Mesh Buffer VB";
	vbuf.Usage = USAGE_IMMUTABLE;
	vbuf.BindFlags = BIND_VERTEX_BUFFER;
	vbuf.Size = (Uint64)(sizeof(Vertex) * m_Mesh->GetVertices().size());

	BufferData vdata;
	vdata.DataSize = vbuf.Size;
	vdata.pData = data;

	Engine::m_pDevice->CreateBuffer(vbuf, &vdata, &VertexBuffer);

	BufferDesc ibuf;

	ibuf.Name = "Mesh Buffer IB";
	ibuf.Usage = USAGE_IMMUTABLE;
	ibuf.BindFlags = BIND_INDEX_BUFFER;
	ibuf.Size = (Uint64)(sizeof(Uint32) * m_Mesh->GetTris().size() * 3);

	Uint32* indices = new Uint32[m_Mesh->GetTris().size() * 3];

	i = 0;
	for (auto t : m_Mesh->GetTris())
	{
		indices[i++] = t.v0;
		indices[i++] = t.v1;
		indices[i++] = t.v2;
	}


	BufferData idata;
	idata.DataSize = ibuf.Size;
	idata.pData = indices;

	Engine::m_pDevice->CreateBuffer(ibuf, &idata, &IndexBuffer);

}