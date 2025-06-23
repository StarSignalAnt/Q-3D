#include "pch.h"
#include "Animator.h"
#include "Bone.h"
#include "Animation.h"
#include "BoneInfo.h"
#include <unordered_set>
Animator::Animator(Animation* anim) {

	m_CurrentTime = 0.0;
	m_CurrentAnim = anim;
	m_Animations.push_back(anim);

	for (int i = 0; i < 100; i++) {
		m_FinalBones.push_back(float4x4::Identity());
	}



}

void Animator::AddAnimation(Animation* animation) {

	m_Animations.push_back(animation);

}

void Animator::SetAnimation(Animation* animation) {

	m_CurrentAnim = animation;
	m_CurrentTime = 0;
	m_Anim1 = nullptr;
	m_Anim2 = nullptr;

}

void Animator::SetTime(float time) {

	m_CurrentTime = time;
	//CalcBones
	CalculateBoneTransform(m_CurrentAnim->GetRootNode(), float4x4::Identity());

}

void UpdateAnim(Animation* anim,float dt) {

}

void Animator::UpdateAnimation(float dt) {

	if (m_Anim1 != nullptr) {

		float ticks = m_Anim1->GetTicksPerSecond();
		float tickMultiplier = ticks / 1.0f;
		float add = dt * ticks;

		float len = m_Anim1->GetLength();// GetLength();

		int b = 0;


		m_Time1 += add;

		//dt = dt + 0.1f;
		//m_AnimTime+=()
	//	if (dt > m_Animator->GetLength()) {
	//		dt = 0;
	//	}


		if (m_Time1 > len) {

			m_Time1 -= len;

		}



	}
	if (m_Anim2 != nullptr) {

		float ticks = m_Anim2->GetTicksPerSecond();
		float tickMultiplier = ticks / 1.0f;
		float add = dt * ticks;

		float len = m_Anim2->GetLength();// GetLength();

		int b = 0;


		m_Time2 += add;

		//dt = dt + 0.1f;
		//m_AnimTime+=()
	//	if (dt > m_Animator->GetLength()) {
	//		dt = 0;
	//	}


		if (m_Time2 > len) {

			m_Time2 -= len;

		}

		
	}

	if (m_Anim1 != nullptr && m_Anim2 != nullptr) {

		m_CurrentAnim = m_Anim1;
		m_CurrentTime = m_Time1;
		SetTime(m_CurrentTime);
		
		std::vector<float4x4> b1;
		std::vector<float4x4> b2;

		for (auto b : m_FinalBones) {

			b1.push_back(b);

		}

		m_CurrentAnim = m_Anim2;
		m_CurrentTime = m_Time2;
		SetTime(m_CurrentTime);

		for (auto b : m_FinalBones) {

			b2.push_back(b);

		}

		int i = 0;
		for (auto b : b1)
		{
			auto c = b2[i];
			m_FinalBones[i] = (b*0.5f) + (c*0.5f);
			i++;	
		}

		return;

	}

	float ticks = GetTicksPerSecond();
	float tickMultiplier = ticks / 1.0f;
	float add = dt * ticks;

	float len = GetLength();

	int b = 0;


	m_CurrentTime += add;

	//dt = dt + 0.1f;
	//m_AnimTime+=()
//	if (dt > m_Animator->GetLength()) {
//		dt = 0;
//	}


	if (m_CurrentTime > len) {

		m_CurrentTime -= len;

	}

	SetTime(m_CurrentTime);

}

void Animator::CalculateBoneTransform(AssimpNodeData* node, float4x4 transform) {

	std::string nodeName = node->name;
	float4x4 nodeTransform = node->transformation;

	Bone* bone = m_CurrentAnim->FindBone(nodeName);

	if (bone != nullptr) {

		bone->Update(m_CurrentTime);
		nodeTransform = bone->GetLocalTransform();

	}

	float4x4 globalTransform = nodeTransform * transform;
	auto boneInfo = m_CurrentAnim->GetBoneIDMap();
	if (boneInfo.count(nodeName) > 0)
	{
		int index = boneInfo[nodeName]->ID;
		float4x4 offset = boneInfo[nodeName]->Offset;
		m_FinalBones[index] = offset*globalTransform;// *offset;// nodeTransform; // float4x4::Identity();// offset* globalTransform;
	}

	for (int i = 0; i < node->childrenCount; i++) {
		CalculateBoneTransform(node->children[i],globalTransform);
	}

}

float Animator::GetLength() {
	return m_CurrentAnim->GetLength();
}

float Animator::GetTicksPerSecond() {

	return m_CurrentAnim->GetTicksPerSecond();

}

std::vector<Animation*> RemoveDuplicates(const std::vector<Animation*>& animations) {
	std::vector<Animation*> uniqueAnimations;
	std::unordered_set<std::string> seenNames;

	for (Animation* anim : animations) {
		if (anim != nullptr) { // Check for null pointers
			std::string name = anim->GetName();
			if (seenNames.find(name) == seenNames.end()) {
				// Name not found in the set, add to unique list and mark as seen
				uniqueAnimations.push_back(anim);
				seenNames.insert(name);
			}
		}
	}

	return uniqueAnimations;
}


void Animator::ClearCopies() {

	
	m_Animations = RemoveDuplicates(m_Animations);
}

void Animator::SetAnimations(std::vector<Animation*> animations) {

	m_CurrentAnims = animations;


}

void Animator::PlayAnimation(std::string name, bool restart) {

	Animation* anim = nullptr;
	m_Time1=m_Time2 =0.0f;
	for (auto a : m_Animations) {

		if (a->GetName() == name) {
			anim = a;
			break;
		}

	}
	if (anim == nullptr) return;

	if (anim == m_CurrentAnim) {

		if (restart) {
			m_CurrentTime = 0;
		}

	}
	else {
		m_CurrentAnim = anim;
		m_CurrentTime = 0;
	}

}

void Animator::SetAnimation1(std::string name)
{
	if (name == "") {
		m_Anim1 = nullptr;
		return;
	}

	Animation* anim = nullptr;
	for (auto a : m_Animations) {

		if (a->GetName() == name) {
			anim = a;
			break;
		}

	}
	if (anim == nullptr) return;

	if (anim == m_Anim1) {

	

	}
	else {
		m_Anim1 = anim;
		m_Time1 = 0;
	}


}

void Animator::SetAnimation2(std::string name)
{
	if (name == "") {
		m_Anim2 = nullptr;
		return;
	}
	Animation* anim = nullptr;
	for (auto a : m_Animations) {

		if (a->GetName() == name) {
			anim = a;
			break;
		}

	}
	if (anim == nullptr) return;

	if (anim == m_Anim2) {



	}
	else {
		m_Anim2= anim;
		m_Time2  = 0;
		m_Time1 = 0;
	}

}


std::vector<Bone*> Animator::GetRealBones() {
	return m_CurrentAnim->GetRealBones();
}