#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "RenderMaterial.h"
#include "BoneInfo.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class Animator;
class Bone;


struct SkVertex3
{

    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 uv;
    glm::vec3 normal;
    glm::vec3 binormal;
    glm::vec3 tangent;
	glm::vec4 boneIndices; // Weights for skinning
    glm::vec4 boneWeights; // Indices of bones for skinning


};

struct SkTri3 {

    unsigned int v0;
    unsigned int v1;
    unsigned int v2;

};

struct SkSubMesh {

    std::vector<SkVertex3> m_Vertices;
    std::vector<SkTri3> m_Triangles;
    RenderMaterial* m_Material = nullptr;
    RenderMaterial* m_DepthMaterial = nullptr;
    RefCntAutoPtr<IBuffer> VertexBuffer;
    RefCntAutoPtr<IBuffer> IndexBuffer;
    SkVertex3 GetVertex(unsigned int index) const {
        if (index < m_Vertices.size()) {
            return m_Vertices[index];
        }
        return SkVertex3();
	}
    void SetVertex(unsigned int index, SkVertex3 vertex) {
        if (index < m_Vertices.size()) {
            m_Vertices[index] = vertex;
        }
	}
    void AddVertex(SkVertex3 vertex, bool reset) {


        if (reset) {
            //vertex.bone_ids = float4(-1, -1, -1, -1);
            //vertex.bone_weights = float4(0, 0, 0, 0);
            vertex.boneIndices = glm::vec4(-1, -1, -1, -1);
            vertex.boneWeights = glm::vec4(0, 0, 0, 0);
        }
        m_Vertices.push_back(vertex);
    }

};


class SkeletalMeshComponent : public Component
{
public:


    SkeletalMeshComponent();

	void OnUpdate(float deltaTime) override;

    void AddSubMesh(SkSubMesh subMesh) {
        m_SubMeshes.push_back(subMesh);
    }
    const std::vector<SkSubMesh>& GetSubMeshes() const {
        return m_SubMeshes;
    }
    void ClearSubMeshes() {
        m_SubMeshes.clear();
    }

    void Finalize();
    SkSubMesh ProcessMesh(aiMesh* mesh, aiScene* scene, bool cre_buf = true);
    void ExtractBoneWeights(SkSubMesh& mesh, aiMesh* amesh, aiScene* scene);
    SkVertex3 SetVertexBoneData(SkVertex3 v, int boneID, float weight);
    std::map<std::string, BoneInfo*> GetBoneInfoMap() {
        return m_BoneInfoMap;
    }
    int GetBoneCount() {
        return m_BoneCounter;
    }
    void SetBoneCount(int bones) {
        m_BoneCounter = bones;
    }
    void SetBoneInfoMap(std::map<std::string, BoneInfo*> info)
    {
        m_BoneInfoMap = info;
    }
    void SetAnimator(Animator* animator) {
        m_Animator = animator;
    }
    std::vector<float4x4> GetBones() {
        return m_Bones;
        //	return m_Animator->GetBones();
    }
    Animator* GetAnimator() {
        return m_Animator;
    }
    float dt = 0.0f;
    void SetAnimTime(float time)
    {
        m_AnimTime = 0;
    }
    void UpdateAnim(float delta);
private:

    Animator* m_Animator = nullptr;
    std::map<std::string, BoneInfo*> m_BoneInfoMap;
    std::vector<SkSubMesh> m_SubMeshes;
    float m_AnimTime = 0.0;
    std::vector<float4x4> m_Bones;
	int m_BoneCounter = 0;
};

