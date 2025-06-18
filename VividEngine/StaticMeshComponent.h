#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <vector>
#include "RenderMaterial.h"

struct Vertex3
{

    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 uv;
    glm::vec3 normal;
    glm::vec3 binormal;
    glm::vec3 tangent;

};

struct Tri3 {

    unsigned int v0;
    unsigned int v1;
    unsigned int v2;

};

struct SubMesh {

    GraphNode* m_Owner = nullptr;

    std::vector<Vertex3> m_Vertices;
    std::vector<Tri3> m_Triangles;
    RenderMaterial* m_Material = nullptr;
    RenderMaterial* m_DepthMaterial = nullptr;
    RefCntAutoPtr<IBuffer> VertexBuffer;
    RefCntAutoPtr<IBuffer> IndexBuffer;
    float3* m_FBuild = nullptr;


    bool RebuildIf() {

        if (m_FBuild == nullptr) {
            return true;
        }
        return false;
    }
    float3* BuildGeo() {

        //m_Build.clear();


        m_FBuild = new float3[m_Triangles.size() * 3];
        int i = 0;

        auto world = m_Owner->GetWorldMatrix();


        for (auto t : m_Triangles) {


            glm::vec3 v0 = glm::vec3(world * glm::vec4(m_Vertices[t.v0].position, 1.0f));
            glm::vec3 v1 = glm::vec3(world * glm::vec4(m_Vertices[t.v2].position, 1.0f));
            glm::vec3 v2 = glm::vec3(world * glm::vec4(m_Vertices[t.v1].position, 1.0f));

            //	m_Build.push_back(v0);
            //	m_Build.push_back(v1);
                //m_Build.push_back(v2);
            m_FBuild[i++] = float3(v0.x, v0.y, v0.z);
            m_FBuild[i++] = float3(v1.x, v1.y, v1.z);
            m_FBuild[i++] = float3(v2.x, v2.y, v2.z);



        }

        return m_FBuild;

    }

};



class StaticMeshComponent :
    public Component
{
public:

    void OnAttach(GraphNode* n) override
    {
        m_Owner = n;
        for (auto sub : m_SubMeshes) {
            sub->m_Owner = n;
        }
    }
    void AddSubMesh(SubMesh* subMesh) {
        m_SubMeshes.push_back(subMesh);
        subMesh->m_Owner = m_Owner;
	}
    const std::vector<SubMesh*> GetSubMeshes() const {
        return m_SubMeshes;
    }
    void ClearSubMeshes() {
        m_SubMeshes.clear();
    }
    void Updated() {
        for (auto mesh : m_SubMeshes) {
            mesh->m_FBuild = nullptr;
        }
    }
    void Finalize();


private:

    std::vector<SubMesh*> m_SubMeshes;

};


