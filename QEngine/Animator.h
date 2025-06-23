#pragma once
#include <vector>
#include <string>
#include "BasicMath.hpp"

using namespace Diligent;

class NodeActor;
class Animation;
class Bone;
class AssimpNodeData;

class Animator
{

public:

	Animator(Animation* anim);
	void SetTime(float time);
	void UpdateAnimation(float dt);
	void AddAnimation(Animation* animation);
	void SetAnimation(Animation* animation);
	void SetAnimations(std::vector<Animation*> animations);
	void CalculateBoneTransform(AssimpNodeData* node, float4x4 transform);
	float GetLength();
	void ClearCopies();
	std::vector<float4x4> GetBones() {
		return m_FinalBones;
	}
	std::vector<Bone*> GetRealBones();
	float GetTicksPerSecond();
	std::vector<Animation*> GetAnimations() {
		return m_Animations;
	}
	void SetAnimation1(std::string name);
	void SetAnimation2(std::string name);

	void PlayAnimation(std::string name, bool restart = false);
private:


	NodeActor* m_Actor;
	
	std::vector<float4x4> m_FinalBones;
	float m_Speed = 0.025f;
	Animation* m_CurrentAnim = nullptr;
	std::vector<Animation*> m_CurrentAnims;
	Animation* m_Anim1, * m_Anim2;
	std::vector<Animation*> m_Animations;
	float m_CurrentTime = 0.0;
	float m_Time1 = 0 , m_Time2 = 0;
	float m_DeltaTime;

};

