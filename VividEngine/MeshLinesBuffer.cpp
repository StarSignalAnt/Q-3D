#include "pch.h"
#include "MeshLinesBuffer.h"
#include "MeshLines.h"
#include "Engine.h"

void MeshLinesBuffer::SetMeshLines(MeshLines* lines) {

	m_Mesh = lines;

	float* data = new float[m_Mesh->GetVertices().size() * 7];

	int i = 0;
	for (auto v : m_Mesh->GetVertices()) {

		data[i++] = v.position.x;
		data[i++] = v.position.y;
		data[i++] = v.position.z;
		data[i++] = v.color.x;
		data[i++] = v.color.y;
		data[i++] = v.color.z;
		data[i++] = v.color.w;
	

	}

	BufferDesc vbuf;
	vbuf.Name = "Mesh Buffer VB";
	vbuf.Usage = USAGE_IMMUTABLE;
	vbuf.BindFlags = BIND_VERTEX_BUFFER;
	vbuf.Size = (Uint64)(sizeof(LineVertex) * m_Mesh->GetVertices().size());

	BufferData vdata;
	vdata.DataSize = vbuf.Size;
	vdata.pData = data;

	Engine::m_pDevice->CreateBuffer(vbuf, &vdata, &VertexBuffer);

	BufferDesc ibuf;

	ibuf.Name = "Mesh Buffer IB";
	ibuf.Usage = USAGE_IMMUTABLE;
	ibuf.BindFlags = BIND_INDEX_BUFFER;
	ibuf.Size = (Uint64)(sizeof(Uint32) * m_Mesh->GetLines().size() * 2);

	Uint32* indices = new Uint32[m_Mesh->GetLines().size() * 3];

	i = 0;
	for (auto t : m_Mesh->GetLines())
	{
		indices[i++] = t.V0;
		indices[i++] = t.V1;

	}


	BufferData idata;
	idata.DataSize = ibuf.Size;
	idata.pData = indices;

	Engine::m_pDevice->CreateBuffer(ibuf, &idata, &IndexBuffer);


}