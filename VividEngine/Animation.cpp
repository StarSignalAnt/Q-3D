#include "pch.h"
#include "Animation.h"
#include "NodeActor.h"
#include "Bone.h"
#include "BasicMath.hpp"
#include "MathsHelp.h"
using namespace Diligent;




Animation::Animation(aiScene* scene, aiAnimation* anim, NodeActor* actor) {

	m_Duration = anim->mDuration;
	m_TicksPerSecond = anim->mTicksPerSecond;
	m_Name = anim->mName.C_Str();
	ReadHeirarchyData(m_RootNode, scene->mRootNode);
	ReadMissingBones(anim,actor);



}

void Animation::ReadHeirarchyData(AssimpNodeData* dest,aiNode* node)
{

	dest->name = node->mName.C_Str();
	
	auto tm = node->mTransformation;
	dest->transformation = MathsHelp::atov(tm);
	dest->childrenCount = node->mNumChildren;
	for (int i = 0; i < node->mNumChildren; i++) {
		auto newData = new AssimpNodeData;
		ReadHeirarchyData(newData, node->mChildren[i]);
		dest->children.push_back(newData);
	}

}

void Animation::ReadMissingBones(aiAnimation* anim, NodeActor* actor) {

	int size = anim->mNumChannels;

	auto boneInfo = actor->GetBoneInfoMap();

	int bone_count = actor->GetBoneCount();

	for (int i = 0; i < size; i++) {

		auto chan = anim->mChannels[i];
		std::string name = chan->mNodeName.C_Str();
		if (boneInfo.count(name) == 0)
		{
			BoneInfo* bone = new BoneInfo;

			bone->ID = bone_count;

			boneInfo[name] = bone;
			bone_count++;
			actor->SetBoneCount(bone_count);



		}

		auto nb = new Bone(std::string(chan->mNodeName.C_Str()), boneInfo[chan->mNodeName.C_Str()]->ID, chan);

		m_Bones.push_back(nb);
		




	}
	actor->SetBoneInfoMap(boneInfo);
	m_BoneInfoMap = boneInfo;

}

Bone* Animation::FindBone(std::string name) {

	for (auto bone : m_Bones) {

		if (bone->GetName() == name) {
			return bone;
		}

	}
	return nullptr;

}