#pragma once
#include "Node.h"
#include "NodeEntity.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include "BoneInfo.h"
#include "data.h"

class Mesh3D;
class Animator;
class Bone;

class NodeActor : public NodeEntity
{
public:


	NodeActor() {
		m_Static = false;
		m_RTEnable = true;
		m_Name = "Actor";
		for (int i = 0; i < 100; i++) {

			m_Bones.push_back(float4x4::Identity());

		}
	};

	Mesh3D* ProcessMesh(aiMesh* mesh, aiScene* scene, bool cre_buf = true);
	void ExtractBoneWeights(Mesh3D* mesh, aiMesh* amesh, aiScene* scene);
	Vertex SetVertexBoneData(Vertex v, int boneID, float weight);
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

	void Render(bool sp);
	void Update(float delta);
	std::vector<float4x4> GetBones();
	Animator* GetAnimator() {
		return m_Animator;
	}
	float dt = 0.0f;
	void SetAnimTime(float time)
	{
		m_AnimTime = 0;
	}
	void SaveAnimList();
	void LoadAnimList();

	void AddAnimFile(std::string file) {
		m_AnimFiles.push_back(file);
	}
	void SaveAnimNames();
	void LoadAnimNames();
	void UpdateAnim(float delta);
	Bone* FindBone(std::string name);
private:

	Animator* m_Animator = nullptr;
	std::map<std::string, BoneInfo*> m_BoneInfoMap;
	int m_BoneCounter = 0;
	Bounds* m_Bounds = nullptr;
	float m_AnimTime = 0.0;
	std::vector<float4x4> m_Bones;
	std::vector<std::string> m_AnimFiles;

};

