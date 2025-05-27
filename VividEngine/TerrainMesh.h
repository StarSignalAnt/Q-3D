#pragma once
#include <vector>
#include "data.h"
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Texture.h>
#include <Graphics/GraphicsEngine/interface/TextureView.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
//#include <Common/interface/FileStream.h>

#include <TextureUtilities.h>
#include "Engine.h"
#include <thread>
#include <mutex>

using namespace Diligent;
class TerrainMeshBuffer;
class NodeTerrain;

class TerrainMesh
{
public:

	void AddVertex(TerrainVertex vertex);
	void AddTriangle(Triangle tri);

	void SetVertices(std::vector<TerrainVertex> vertices) {
		m_Vertices = vertices;
	}
	void SetTriangles(std::vector<Triangle> triangles) {
		m_Triangles = triangles;
	}
	void Build();
    RefCntAutoPtr<IBottomLevelAS> GetBLAS();

	TerrainMeshBuffer* GetBuffer() {
		return m_Buffer;
	}
	void CalculateNormals() {

        std::vector<float3> normals(m_Vertices.size());
        std::vector<float3> binormals(m_Vertices.size());
        std::vector <float3> tangents(m_Vertices.size());

        for(auto triangle : m_Triangles)
        {

            int vertexIndex0 = triangle.v0;
            int vertexIndex1 = triangle.v1;
            int vertexIndex2 = triangle.v2;

            TerrainVertex vertex0 = m_Vertices[vertexIndex0];
            TerrainVertex vertex1 = m_Vertices[vertexIndex1];
            TerrainVertex vertex2 = m_Vertices[vertexIndex2];

            // Calculate the edges of the triangle
            float3 edge1 = vertex1.position - vertex0.position;
            float3 edge2 = vertex2.position - vertex0.position;


            // Get the 2D texture coordinates
            float2 texCoord0 = float2(vertex0.texture.x, vertex0.texture.y);
            float2 texCoord1 = float2(vertex1.texture.x, vertex1.texture.y);
            float2 texCoord2 = float2(vertex2.texture.x, vertex2.texture.y);


            // Calculate the texture coordinate differences
            float2 deltaUV1 = texCoord1 - texCoord0;
            float2 deltaUV2 = texCoord2 - texCoord0;

            // Adjust for texture coordinate repeats
            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            deltaUV1 *= f;
            deltaUV2 *= f;

            // Calculate the tangent and binormal vectors
            float3 tangent;
            float3 binormal;


            tangent.x = deltaUV2.y * edge1.x - deltaUV1.y * edge2.x;
            tangent.y = deltaUV2.y * edge1.y - deltaUV1.y * edge2.y;
            tangent.z = deltaUV2.y * edge1.z - deltaUV1.y * edge2.z;
            tangent = Diligent::normalize(tangent);


            binormal.x = -deltaUV2.x * edge1.x + deltaUV1.x * edge2.x;
            binormal.y = -deltaUV2.x * edge1.y + deltaUV1.x * edge2.y;
            binormal.z = -deltaUV2.x * edge1.z + deltaUV1.x * edge2.z;
            binormal = Diligent::normalize(binormal);


            // Calculate the face normal of the current triangle
            float3 faceNormal = Diligent::normalize(Diligent::cross(edge1, edge2));

            // Accumulate the normal, tangent, and binormal for each vertex of the triangle
            normals[vertexIndex0] += faceNormal;
            normals[vertexIndex1] += faceNormal;
            normals[vertexIndex2] += faceNormal;

            tangents[vertexIndex0] += tangent;
            tangents[vertexIndex1] += tangent;
            tangents[vertexIndex2] += tangent;

            binormals[vertexIndex0] += binormal;
            binormals[vertexIndex1] += binormal;
            binormals[vertexIndex2] += binormal;
        }

        for (int i = 0; i < m_Vertices.size(); i++)
        {
            normals[i] = Diligent::normalize(normals[i]);
            binormals[i] = Diligent::normalize(binormals[i]);
            tangents[i] = Diligent::normalize(tangents[i]);

            // Update the normal, binormal, and tangent properties of each vertex
            TerrainVertex vertex = m_Vertices[i];
            vertex.normal = -normals[i];
            vertex.binormal = binormals[i];
            vertex.tangent = tangents[i];
            m_Vertices[i] = vertex;
        }

	}
    bool NeedRebuild() {
        return m_Rebuild;
    }
    void RebuildGeo();
    std::vector<Triangle>& GetTriangles() {
        return m_Triangles;
    }
    std::vector<TerrainVertex>& GetVertices() {
        return m_Vertices;
    }
    void SetOwner(NodeTerrain* owner) {
        m_Owner = owner;
    }
    NodeTerrain* GetOwner() {
        return m_Owner;
    }
    std::vector<float3> GetGeo() {
        return m_Build;
    }
 
private:
	
	std::vector<TerrainVertex> m_Vertices;
	std::vector<Triangle> m_Triangles;
	TerrainMeshBuffer* m_Buffer;
    bool m_Rebuild = true;
    NodeTerrain* m_Owner = nullptr;
    std::vector<float3> m_Build;
    std::string m_Name;
    RefCntAutoPtr<IBottomLevelAS> m_BLAS;
    RefCntAutoPtr<IBuffer> VertexBuffer;
    RefCntAutoPtr<IBuffer> IndexBuffer;
    Node* m_Node;
     bool m_Dynamic = false;
    float* m_VertData = nullptr;
    RefCntAutoPtr<IBuffer> pScratchBuffer;
    TLASBuildInstanceData m_Instance;
    bool m_HasInstance = false;
};

