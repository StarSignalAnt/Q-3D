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

	std::vector<Vertex3> m_Vertices;
    std::vector<Tri3> m_Triangles;
    RenderMaterial* m_Material = nullptr;
    RefCntAutoPtr<IBuffer> VertexBuffer;
    RefCntAutoPtr<IBuffer> IndexBuffer;

};


class StaticMeshComponent :
    public Component
{
public:

    void AddSubMesh(SubMesh subMesh) {
        m_SubMeshes.push_back(subMesh);
	}
    const std::vector<SubMesh>& GetSubMeshes() const {
        return m_SubMeshes;
    }
    void ClearSubMeshes() {
        m_SubMeshes.clear();
    }

    void Finalize();


private:

    std::vector<SubMesh> m_SubMeshes;

};


