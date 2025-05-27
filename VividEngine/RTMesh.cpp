#include "pch.h"
#include "RTMesh.h"
#include "Mesh3D.h"
#include "NodeEntity.h"
#include "Engine.h"
#include "NodeActor.h"


RTMesh::RTMesh(Mesh3D* mesh) {


	
	m_Node = mesh->GetOwner();
	m_Mesh = mesh;
	BLASTriangleDesc Triangles;

	auto name = mesh->GetOwner()->GetFullName();

	m_Name = name;
	Triangles.GeometryName = name.c_str();
	Triangles.MaxVertexCount = mesh->GetVertices().size();
	Triangles.VertexValueType = VT_FLOAT32;
	Triangles.VertexComponentCount = 3;
	Triangles.MaxPrimitiveCount = mesh->GetTris().size();
	Triangles.IndexType = VT_UINT32;

	BottomLevelASDesc ASDesc;
	ASDesc.Name = mesh->GetOwner()->GetFullName().c_str();
	ASDesc.Flags = RAYTRACING_BUILD_AS_PREFER_FAST_TRACE;
	ASDesc.pTriangles = &Triangles;
	ASDesc.TriangleCount = 1;

	Engine::m_pDevice->CreateBLAS(ASDesc, &m_BLAS);

	
	//

	float* data = new float[mesh->GetVertices().size() * 3];

	int i = 0;
	for (auto v : mesh->GetVertices()) {

		data[i++] = v.position.x;
		data[i++] = v.position.y;

		data[i++] = v.position.z;
		
	}
	m_VertData = data;

	BufferDesc vbuf;
	vbuf.Name = "Mesh Buffer VB";
	vbuf.Usage = USAGE_DEFAULT;
	vbuf.BindFlags = BIND_VERTEX_BUFFER | BIND_RAY_TRACING;
	vbuf.Size = (Uint64)(sizeof(float3) * mesh->GetVertices().size());

	BufferData vdata;
	vdata.DataSize = vbuf.Size;
	vdata.pData = data;

	Engine::m_pDevice->CreateBuffer(vbuf, &vdata, &VertexBuffer);


	BufferDesc ibuf;

	ibuf.Name = "Mesh Buffer IB";
	ibuf.Usage = USAGE_IMMUTABLE;
	ibuf.BindFlags = BIND_INDEX_BUFFER | BIND_RAY_TRACING;
	ibuf.Size = (Uint64)(sizeof(Uint32) * mesh->GetTris().size() * 3);

	Uint32* indices = new Uint32[mesh->GetTris().size() * 3];

	i = 0;
	for (auto t : mesh->GetTris())
	{
		indices[i++] = t.v0;
		indices[i++] = t.v1;
		indices[i++] = t.v2;
	}


	BufferData idata;
	idata.DataSize = ibuf.Size;
	idata.pData = indices;

	Engine::m_pDevice->CreateBuffer(ibuf, &idata, &IndexBuffer);


	//

	
	{
		BufferDesc BuffDesc;
		BuffDesc.Name = "BLAS Scratch Buffer";
		BuffDesc.Usage = USAGE_DEFAULT;
		BuffDesc.BindFlags = BIND_RAY_TRACING;
		BuffDesc.Size = m_BLAS->GetScratchBufferSizes().Build;


		Engine::m_pDevice->CreateBuffer(BuffDesc, nullptr, &pScratchBuffer);
		VERIFY_EXPR(pScratchBuffer != nullptr);
	}


	//

	BLASBuildTriangleData TriangleData;
	TriangleData.GeometryName = Triangles.GeometryName;
	TriangleData.pVertexBuffer = VertexBuffer;
	TriangleData.VertexStride = sizeof(float3() * 3);
	TriangleData.VertexCount = Triangles.MaxVertexCount;
	TriangleData.VertexValueType = Triangles.VertexValueType;
	TriangleData.VertexComponentCount = Triangles.VertexComponentCount;
	TriangleData.pIndexBuffer = IndexBuffer;
	TriangleData.PrimitiveCount = Triangles.MaxPrimitiveCount;
	TriangleData.IndexType = Triangles.IndexType;
	TriangleData.Flags = RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	BuildBLASAttribs Attribs;
	Attribs.pBLAS = m_BLAS;
	Attribs.pTriangleData = &TriangleData;
	Attribs.TriangleDataCount = 1;
	Attribs.pScratchBuffer = pScratchBuffer;

	Engine::m_pImmediateContext->BuildBLAS(Attribs);

	if (dynamic_cast<NodeActor*>(mesh->GetOwner())!=nullptr) {

		m_Dynamic = true;

	}


}

void RTMesh::UpdateBLAS() {

	BLASTriangleDesc Triangles;

	auto mesh = m_Mesh;

	auto name = m_Mesh->GetOwner()->GetFullName();

	auto act = (NodeActor*)m_Node;

	auto bones = act->GetBones();

//	auto mats = act->GetMatrices();

	m_Name = name;
	Triangles.GeometryName = name.c_str();
	Triangles.MaxVertexCount = mesh->GetVertices().size();
	Triangles.VertexValueType = VT_FLOAT32;
	Triangles.VertexComponentCount = 3;
	Triangles.MaxPrimitiveCount = mesh->GetTris().size();
	Triangles.IndexType = VT_UINT32;

	BottomLevelASDesc ASDesc;
	ASDesc.Name = mesh->GetOwner()->GetFullName().c_str();
	ASDesc.Flags = RAYTRACING_BUILD_AS_PREFER_FAST_TRACE;
	ASDesc.pTriangles = &Triangles;
	ASDesc.TriangleCount = 1;

	//m_BLAS = nullptr;
	//Engine::m_pDevice->CreateBLAS(ASDesc, &m_BLAS);



	//

	float* data = m_VertData;

	int ii = 0;
	for (auto v : mesh->GetVertices()) {

		auto pos = v.position;

		float4x4 s = float4x4(0);

		for (int i = 0; i < 4; i++) {
			if (v.bone_ids[i] >= 0)
			{
				s+= (bones[(int)v.bone_ids[i]] * v.bone_weights[i]);
			}
		}

		pos = pos * s;

		data[ii++] = pos.x;
		data[ii++] = pos.y;
		data[ii++] = pos.z;

		//ii++;

	}

	int i = 0;
	//for (auto v : mesh->GetVertices()) {

	//	data[i++] = v.position.x;
	//	data[i++] = v.position.y;

		//data[i++] = v.position.z;

//	}




//	VertexBuffer = nullptr;

	//Engine::m_pDevice->CreateBuffer(vbuf, &vdata, &VertexBuffer);
	Engine::m_pImmediateContext->UpdateBuffer(VertexBuffer, 0, VertexBuffer->GetDesc().Size,data, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);



	BufferDesc ibuf;

	ibuf.Name = "Mesh Buffer IB";
	ibuf.Usage = USAGE_IMMUTABLE;
	ibuf.BindFlags = BIND_INDEX_BUFFER | BIND_RAY_TRACING;
	ibuf.Size = (Uint64)(sizeof(Uint32) * mesh->GetTris().size() * 3);

	Uint32* indices = new Uint32[mesh->GetTris().size() * 3];

	i = 0;
	for (auto t : mesh->GetTris())
	{
		indices[i++] = t.v0;
		indices[i++] = t.v1;
		indices[i++] = t.v2;
	}


	BufferData idata;
	idata.DataSize = ibuf.Size;
	idata.pData = indices;
//	IndexBuffer = nullptr;

//	Engine::m_pDevice->CreateBuffer(ibuf, &idata, &IndexBuffer);


	//




	//

	BLASBuildTriangleData TriangleData;
	TriangleData.GeometryName = Triangles.GeometryName;
	TriangleData.pVertexBuffer = VertexBuffer;
	TriangleData.VertexStride = sizeof(float3() * 3);
	TriangleData.VertexCount = Triangles.MaxVertexCount;
	TriangleData.VertexValueType = Triangles.VertexValueType;
	TriangleData.VertexComponentCount = Triangles.VertexComponentCount;
	TriangleData.pIndexBuffer = IndexBuffer;
	TriangleData.PrimitiveCount = Triangles.MaxPrimitiveCount;
	TriangleData.IndexType = Triangles.IndexType;
	TriangleData.Flags = RAYTRACING_GEOMETRY_FLAG_OPAQUE;



	BuildBLASAttribs Attribs;
	Attribs.pBLAS = m_BLAS;
	Attribs.pTriangleData = &TriangleData;
	Attribs.TriangleDataCount = 1;
	Attribs.pScratchBuffer = pScratchBuffer;

	
	Engine::m_pImmediateContext->BuildBLAS(Attribs);
	

}