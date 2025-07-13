#include "pch.h"
#include "TerrainMeshBuffer.h"
#include "TerrainMesh.h"
//#include "Engine.h"
//#include "MeshBuffer.h"
//#include "Mesh3D.h"
#include "QEngine.h"

TerrainMeshBuffer::TerrainMeshBuffer(TerrainMesh* mesh) {


	


	BufferDesc vbuf;
	vbuf.Name = "Mesh Buffer VB";
	vbuf.Usage = USAGE_DEFAULT;
	vbuf.BindFlags = BIND_VERTEX_BUFFER;
	vbuf.Size = (Uint64)(sizeof(TerrainVertex) * mesh->GetVertices().size());

	BufferData vdata;
	vdata.DataSize = vbuf.Size;
	vdata.pData = mesh->GetVertices().data();


	Q3D::Engine::QEngine::GetDevice()->CreateBuffer(vbuf, &vdata, &VertexBuffer);


	BufferDesc ibuf;

	ibuf.Name = "Mesh Buffer IB";
	ibuf.Usage = USAGE_IMMUTABLE;
	ibuf.BindFlags = BIND_INDEX_BUFFER;
	ibuf.Size = (Uint64)(sizeof(Uint32) * mesh->GetTriangles().size() * 3);

	Uint32* indices = new Uint32[mesh->GetTriangles().size() * 3];

	int i = 0;
	i = 0;
	for (auto t : mesh->GetTriangles())
	{
		indices[i++] = t.v0;
		indices[i++] = t.v2;
		indices[i++] = t.v1;
	}


	BufferData idata;
	idata.DataSize = ibuf.Size;
	idata.pData = indices;


	Q3D::Engine::QEngine::GetDevice()->CreateBuffer(ibuf, &idata, &IndexBuffer);
	m_Mesh = mesh;
}

void TerrainMeshBuffer::Update() {

	float* data = m_Data;

	int i = 0;


	//Diligent::BufferUpdateDesc updateDesc(pVertexBuffer, 0); // Offset 0 to start at the beginning of the buffer
	//updateDesc.pData = pData;
	//updateDesc.DataSize = DataSize;
	Q3D::Engine::QEngine::GetContext()->UpdateBuffer(VertexBuffer, 0, VertexBuffer->GetDesc().Size, m_Mesh->GetVertices().data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);




}