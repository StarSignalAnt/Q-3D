#pragma once
#include <vector>
#include "data.h"
class NodeEntity;
class MeshBuffer;
class MaterialBase;
class NodeCamera;
class NodeLight;
class MaterialDepth;

class Mesh3D
{
public:

	Mesh3D();
	void AddVertex(Vertex vertex,bool reset);
	void AddTri(Triangle tri);
	std::vector<Vertex> GetVertices();
	std::vector<Triangle> GetTris();
	void Build();
	void BuildFast(float* v, Uint32* i,int verts,int tris);
	MaterialBase* GetMaterial() {
		return m_Material;
	}
	void SetMaterial(MaterialBase* material) {
		m_Material = material;
	}
	MaterialBase* GetDepthMaterial()
	{
		return m_DepthMaterial;
	}

	void SetDepthMaterial(MaterialBase* depth)
	{
		m_DepthMaterial = depth;
	}
	void SetOwner(NodeEntity* entity);
	NodeEntity* GetOwner();
	void Render(bool sp);
	void RenderDepth();
	void BuildGeo();
	std::vector<float3> GetGeo() {
		return m_UseBuild;
	}
	float3* GetRawGeo() {
		return m_FUseBuild;
	}
	int TriCount() {

		return m_Tris.size();
	}
	void NeedsRebuild() {
		n_NeedsRebuild = true;
	}
	bool RebuildIf() {
		return n_NeedsRebuild;
	}
	Vertex GetVertex(int num) {
		return m_Vertices[num];
	}
	void SetVertex(int index,Vertex vertex) {
		m_Vertices[index] = vertex;
	}
	float* GetVertexData();
	Uint32* GetTriData();


//	cl::Buffer& GetCLBuf() {
//		return triBuf;
	//}
//	void SetCLBuf(cl::Buffer& buf) {
//		triBuf = buf;
	//}
	MeshBuffer* GetBuffer() {
		return m_Buffer;
	}
private:

	std::vector<Vertex> m_Vertices;
	std::vector<Triangle> m_Tris;
	//
	MeshBuffer* m_Buffer;
	MaterialBase* m_Material;
	MaterialBase* m_DepthMaterial;
	NodeEntity* m_Owner;
	std::vector<float3> m_Build;
	std::vector<float3> m_UseBuild;
	bool n_NeedsRebuild = true;
	float3* m_FBuild;
	float3* m_FUseBuild;
	int m_NumTris, m_NumVerts;
	float* m_VertsData;
	Uint32* m_TrisData;
//	cl::Buffer triBuf;
};

