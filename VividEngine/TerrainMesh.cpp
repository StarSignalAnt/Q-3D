#include "pch.h"
#include "TerrainMesh.h"
#include "TerrainMeshBuffer.h"
#include "GraphNode.h"

void TerrainMesh::AddVertex(TerrainVertex vertex) {

	m_Vertices.push_back(vertex);

}

void TerrainMesh::AddTriangle(Triangle tri) {

	m_Triangles.push_back(tri);

}

void TerrainMesh::Build() {

    if (m_Buffer) {
        m_Buffer->Update();
    }
    else {
        m_Buffer = new TerrainMeshBuffer(this);
    }

	

	


}


void TerrainMesh::RebuildGeo() {

    if (m_Rebuild == false) return;
    //printf("REBUILDING GEO: Tris:%d\n", GetTriangles().size());
    auto world = m_Owner->GetWorldMatrix();

    auto tris = GetTriangles();
    auto verts = GetVertices();
    m_Build.clear();

    for (auto t : tris) {

        glm::vec3 v0 = glm::vec4(verts[t.v0].position,1.0) * world;
        glm::vec3 v1 = glm::vec4(verts[t.v1].position,1.0) * world;
        glm::vec3 v2 = glm::vec4(verts[t.v2].position,1.0) * world;
        m_Build.push_back(v0);
        m_Build.push_back(v1);
        m_Build.push_back(v2);


    }

    m_Rebuild = false;

}

RefCntAutoPtr<IBottomLevelAS> TerrainMesh::GetBLAS() {
	return m_BLAS;
	/*
	if (m_BLAS != nullptr) {
		return m_BLAS;
	}
	BLASTriangleDesc Triangles;

	auto name = GetOwner()->GetFullName();

	
	m_Name = name;
	Triangles.GeometryName = name.c_str();
	Triangles.MaxVertexCount = GetVertices().size();
	Triangles.VertexValueType = VT_FLOAT32;
	Triangles.VertexComponentCount = 3;
	Triangles.MaxPrimitiveCount = GetTriangles().size();
	Triangles.IndexType = VT_UINT32;

	BottomLevelASDesc ASDesc;
	ASDesc.Name = GetOwner()->GetFullName().c_str();
	ASDesc.Flags = RAYTRACING_BUILD_AS_PREFER_FAST_TRACE;
	ASDesc.pTriangles = &Triangles;
	ASDesc.TriangleCount = 1;

	Engine::m_pDevice->CreateBLAS(ASDesc, &m_BLAS);


	//

	float* data = new float[GetVertices().size() * 3];

	int i = 0;
	for (auto v : GetVertices()) {

		data[i++] = v.position.x;
		data[i++] = v.position.y;

		data[i++] = v.position.z;

	}
	m_VertData = data;

	BufferDesc vbuf;
	vbuf.Name = "Mesh Buffer VB";
	vbuf.Usage = USAGE_DEFAULT;
	vbuf.BindFlags = BIND_VERTEX_BUFFER | BIND_RAY_TRACING;
	vbuf.Size = (Uint64)(sizeof(float3) * GetVertices().size());

	BufferData vdata;
	vdata.DataSize = vbuf.Size;
	vdata.pData = data;

	Engine::m_pDevice->CreateBuffer(vbuf, &vdata, &VertexBuffer);


	BufferDesc ibuf;

	ibuf.Name = "Mesh Buffer IB";
	ibuf.Usage = USAGE_IMMUTABLE;
	ibuf.BindFlags = BIND_INDEX_BUFFER | BIND_RAY_TRACING;
	ibuf.Size = (Uint64)(sizeof(Uint32) * GetTriangles().size() * 3);

	Uint32* indices = new Uint32[GetTriangles().size() * 3];

	i = 0;
	for (auto t : GetTriangles())
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

	return m_BLAS;
	*/
}