#pragma once
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "BasicMath.hpp"
using namespace Diligent;


struct KeyPosition {

	float3 position;
	float timeStamp;
	KeyPosition(float3 pos, float time) {
		position = pos;
		timeStamp = time;
	}
};

struct KeyRotation {

	QuaternionF rotation;
	float timeStamp;
	KeyRotation(QuaternionF rot, float time)
	{
		rotation = rot;
		timeStamp = time;
	}

};

struct KeyScale {

	float3 scale;
	float timeStamp = 0;
	KeyScale(float3 scal, float time)
	{
		scale = scal;
		timeStamp = time;
	}

};

class Bone
{
public:

	Bone();
	Bone(std::string name, int id, aiNodeAnim* channel);
	std::string GetName() { return m_Name; };
	void Update(float time);
	float4x4 GetLocalTransform()
	{
		return m_LocalTransform;
	}
	float4x4 InterpolatePosition(float time);
	float4x4 InterpolateRotation(float time);
	float4x4 InterpolateScale(float time);

	int GetPositionIndex(float time) {
		for (int index = 0; index < m_NumPositions-1; index++) {

			if (time < m_Positions[index+1].timeStamp)
				return index;

		}
		return 0;
	}
	int GetRotationIndex(float time) {
		for (int index = 0; index < m_NumRotations-1; index++) {

			if (time < m_Rotations[index+1].timeStamp)
				return index;

		}
		return 0;
	}
	int GetScaleIndex(float time) {
		for (int index = 0; index < m_NumScales-1; index++) {

			if (time < m_Scales[index+1].timeStamp)
				return index;

		}
		return 0;
	}
	void OffsetRotate(float3 ang);
private:

	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
	int m_NumPositions = 0;
	int m_NumRotations = 0;
	int m_NumScales = 0;
	float4x4 m_LocalTransform = float4x4::Identity();
	float4x4 m_LocalOffset = float4x4::Identity();
	std::string m_Name = "";
	int m_ID = 0;





};

