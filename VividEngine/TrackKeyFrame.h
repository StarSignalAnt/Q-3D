#pragma once
#include "BasicMath.hpp"
using namespace Diligent;

class TrackKeyFrame
{
public:

	void SetPosition(float3 position);
	void SetRotation(float4x4 rotation);
	void SetScale(float3 scale);
	float3 GetPosition();
	float4x4 GetRotation();
	float3 GetScale();
	void SetTime(double time);
	double GetTime();

private:

	double m_Time = 0.0;
	float3 m_Position;
	float4x4 m_Rotation;
	float3 m_Scale;
	bool m_InverseWorld = false;

};

