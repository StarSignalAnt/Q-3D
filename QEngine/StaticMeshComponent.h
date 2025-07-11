#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <vector>
#include "RenderMaterial.h"
#define MESHOPTIMIZER_IMPLEMENTATION
#include "meshoptimizer.h" 
#include "MaterialProducer.h"
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


class LODLevel {

public:
    RefCntAutoPtr<IBuffer> VertexBuffer;
    RefCntAutoPtr<IBuffer> IndexBuffer;
    std::vector<Vertex3> m_Vertices;
    std::vector<Tri3> m_Triangles;
};

struct SubMesh {

    GraphNode* m_Owner = nullptr;


    RenderMaterial* m_Material = nullptr;
    RenderMaterial* m_DepthMaterial = nullptr;

    float3* m_FBuild = nullptr;
    std::vector<LODLevel*> m_LODs;

    void GenerateLod(int total_levels) {
        if (m_LODs.empty() || m_LODs[0] == nullptr || total_levels <= 1) {
            return;
        }

        // Get the original, full-quality mesh data from LOD 0.
        LODLevel* lod0 = m_LODs[0];
        const std::vector<Vertex3>& source_vertices = lod0->m_Vertices;
        const std::vector<Tri3>& source_triangles = lod0->m_Triangles;

        // The meshoptimizer library works with flat index buffers, so we convert our Tri3 struct.
        std::vector<unsigned int> source_indices;
        source_indices.reserve(source_triangles.size() * 3);
        for (const auto& tri : source_triangles) {
            source_indices.push_back(tri.v0);
            source_indices.push_back(tri.v1);
            source_indices.push_back(tri.v2);
        }

        // Generate the requested number of lower-quality LODs.
        // We start at level 1 because level 0 is our source.
        for (int i = 1; i < total_levels; ++i) {
            // Determine the target quality for this level.
            // LOD 1 (i=1) = 50% of original triangles.
            // LOD 2 (i=2) = 25% of original triangles.
            float target_quality = powf(0.5f, (float)i);
            size_t target_index_count = size_t(source_indices.size() * target_quality);

            // MODIFIED: Increased target_error significantly to make simplification much more aggressive.
            // A higher value allows the algorithm to make larger changes to the mesh shape
            // in order to reach the target polygon count.
            float target_error = 0.35;

            // --- Use meshoptimizer to simplify the mesh ---
            std::vector<unsigned int> lod_indices(source_indices.size());
            lod_indices.resize(meshopt_simplifySloppy(&lod_indices[0], &source_indices[0], source_indices.size(),
                &source_vertices[0].position.x, source_vertices.size(), sizeof(Vertex3),
                target_index_count, target_error));

            // --- Create a new, compact vertex buffer for the simplified mesh ---
            std::vector<unsigned int> remap_table(source_vertices.size());
            size_t unique_vertex_count = meshopt_optimizeVertexFetchRemap(&remap_table[0], &lod_indices[0],
                lod_indices.size(), source_vertices.size());

            std::vector<Vertex3> lod_vertices(unique_vertex_count);
            meshopt_remapVertexBuffer(&lod_vertices[0], &source_vertices[0], source_vertices.size(), sizeof(Vertex3), &remap_table[0]);
            meshopt_remapIndexBuffer(&lod_indices[0], &lod_indices[0], lod_indices.size(), &remap_table[0]);

            // --- Create and populate the new LODLevel object ---
            LODLevel* new_lod = new LODLevel();
            new_lod->m_Vertices = std::move(lod_vertices);

            // Convert the flat index buffer back to our Tri3 struct format.
            new_lod->m_Triangles.reserve(lod_indices.size() / 3);
            for (size_t t = 0; t < lod_indices.size(); t += 3) {
                new_lod->m_Triangles.push_back({ lod_indices[t], lod_indices[t + 1], lod_indices[t + 2] });
            }

            // Add the newly generated LOD to our list.
            m_LODs.push_back(new_lod);
        }
    }

    bool RebuildIf() {

        if (m_FBuild == nullptr) {
            return true;
        }
        return false;
    }
    float3* BuildGeo() {

        //m_Build.clear();

        auto m_Triangles = m_LODs[0]->m_Triangles;
        auto m_Vertices = m_LODs[0]->m_Vertices;

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

    void Write(VFile* f) {



        f->WriteString(m_Material->GetPath().c_str());

        f->WriteInt(m_LODs.size());

        for (auto lod : m_LODs) {

            f->WriteInt(lod->m_Vertices.size());

            for (auto vertex : lod->m_Vertices) {

                f->WriteVec3(vertex.position);
                f->WriteVec3(vertex.uv);
                f->WriteVec4(vertex.color);
                f->WriteVec3(vertex.normal);
                f->WriteVec3(vertex.binormal);
                f->WriteVec3(vertex.tangent);

            }

            f->WriteInt(lod->m_Triangles.size());

            for (auto tri : lod->m_Triangles) {

                f->WriteInt(tri.v0);
                f->WriteInt(tri.v1);
                f->WriteInt(tri.v2);

            }

        }


    }

    void Read(VFile* f) {




        std::string mat_path = f->ReadString();


        m_Material = (RenderMaterial*)MaterialProducer::m_Instance->GetPBR();
        m_DepthMaterial = (RenderMaterial*)MaterialProducer::m_Instance->GetDepth();
        m_Material->Load(mat_path);
        int lods = f->ReadInt();

        for (int j = 0; j < lods; j++) {

            LODLevel* lod = new LODLevel;
            m_LODs.push_back(lod);

            int vc = f->ReadInt();

            for (int k = 0; k < vc; k++) {

                Vertex3 vertex;
                vertex.position = f->ReadVec3();
                vertex.uv = f->ReadVec3();
                vertex.color = f->ReadVec4();
                vertex.normal = f->ReadVec3();
                vertex.binormal = f->ReadVec3();
                vertex.tangent = f->ReadVec3();
                lod->m_Vertices.push_back(vertex);
            }

            int tc = f->ReadInt();

            for (int k = 0; k < tc; k++) {

                Tri3 tri;
                tri.v0 = f->ReadInt();
                tri.v1 = f->ReadInt();
                tri.v2 = f->ReadInt();
                lod->m_Triangles.push_back(tri);

            }

        }

    }

};



class StaticMeshComponent :
    public Component
{
public:
    StaticMeshComponent() {

        m_Name = "Static Mesh Component";
        m_Properties.bind("mesh_path", &mesh_path);

    }
    void Initialize() override;
    Component* CreateInstance() {
        return new StaticMeshComponent;
    }
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
    const std::vector<SubMesh*>& GetSubMeshes() const {
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

    std::string mesh_path;
private:

    std::vector<SubMesh*> m_SubMeshes;


};


