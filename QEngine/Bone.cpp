#include "pch.h"
#include "MathsHelp.h"
#include "Bone.h"
using namespace Diligent;

float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}


Bone::Bone(std::string name, int id, aiNodeAnim* anim) {

	m_Name = name;
	m_ID = id;
	m_LocalTransform = float4x4::Identity();

	m_NumPositions = anim->mNumPositionKeys;
	m_NumRotations = anim->mNumRotationKeys;
	m_NumScales = anim->mNumScalingKeys;

	for (int i = 0; i < m_NumPositions; i++) {

		KeyPosition pos(MathsHelp::atov(anim->mPositionKeys[i].mValue),anim->mPositionKeys[i].mTime);
		m_Positions.push_back(pos);

	}

	for (int i = 0; i < m_NumRotations; i++) {
		auto arot = anim->mRotationKeys[i].mValue;
		KeyRotation rot(QuaternionF(arot.x, arot.y, arot.z, arot.w),anim->mRotationKeys[i].mTime);
		m_Rotations.push_back(rot);
	}

	for (int i = 0; i < m_NumScales; i++) {
		KeyScale scal(MathsHelp::atov(anim->mScalingKeys[i].mValue), anim->mScalingKeys[i].mTime);
		m_Scales.push_back(scal);
	}


}

void Bone::Update(float time) {

	float4x4 translation = InterpolatePosition(time);
	float4x4 rotation = InterpolateRotation(time).Inverse();
	float4x4 scale = InterpolateScale(time);
	//m_LocalTransform = translation * rotation * scale;
	m_LocalTransform = scale * rotation * translation;
	m_LocalTransform *= m_LocalOffset;


	//m_LocalTransform = float4x4::Identity();

}


float4x4 Bone::InterpolatePosition(float time) {

	if (1 == m_NumPositions) return float4x4::Translation(m_Positions[0].position);

	int p0Index = GetPositionIndex(time);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, time);
	auto finalPos = Diligent::lerp(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
	return float4x4::Translation(finalPos);
	
}

float4x4 Bone::InterpolateRotation(float time) {

	if (1 == m_NumRotations)
	{
		return MathsHelp::atov(m_Rotations[0].rotation);
	}

	int p0Index = GetRotationIndex(time);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, time);

	auto q1 = m_Rotations[p0Index].rotation;
	auto q2 = m_Rotations[p1Index].rotation;

	QuaternionF final = Diligent::slerp(q1, q2, scaleFactor);

	return MathsHelp::atov(final);

}

float4x4 Bone::InterpolateScale(float time) {


	if (1 == m_NumScales) {

		return float4x4::Scale(m_Scales[0].scale);

	}

	int p0Index = GetScaleIndex(time);
	int p1Index = p0Index + 1;

	float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, time);
	float3 finalScale = Diligent::lerp(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
	return float4x4::Scale(finalScale);




}

void Bone::OffsetRotate(float3 ang)
{

	float yaw = MathsHelp::Deg2Rad(ang.y);
	float pitch = MathsHelp::Deg2Rad(ang.x);
	float roll = MathsHelp::Deg2Rad(ang.z);
	//BuildGeo();



	//m_Rotation = CreateRotationMatrix(pitch, yaw, roll);

//	if (edit) {
	m_LocalOffset = float4x4::RotationY(yaw) * float4x4::RotationX(pitch);


}