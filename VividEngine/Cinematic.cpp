#include "pch.h"
#include "Cinematic.h"
#include "CineTrack.h"
#include "TrackKeyFrame.h"
#include "BasicMath.hpp"
#include "Node.h"
using namespace Diligent;

QuaternionF MatrixToQuaternion(const float4x4& mat) {
	QuaternionF q;
	float trace = mat.m00 + mat.m11 + mat.m22;

	if (trace > 0) {
		float s = 0.5f / std::sqrt(trace + 1.0f);
		q.q.x = 0.25f / s;
		q.q.y = (mat.m21 - mat.m12) * s;
		q.q.y = (mat.m02 - mat.m20) * s;
		q.q.z = (mat.m10 - mat.m01) * s;
	}
	else {
		if (mat.m00 > mat.m11 && mat.m00 > mat.m22) {
			float s = 2.0f * std::sqrt(1.0f + mat.m00 - mat.m11 - mat.m22);
			q.q.w = (mat.m21 - mat.m12) / s;
			q.q.x = 0.25f * s;
			q.q.y = (mat.m01 + mat.m10) / s;
			q.q.z = (mat.m02 + mat.m20) / s;
		}
		else if (mat.m11 > mat.m22) {
			float s = 2.0f * std::sqrt(1.0f + mat.m11 - mat.m00 - mat.m22);
			q.q.w = (mat.m02 - mat.m20) / s;
			q.q.x = (mat.m01 + mat.m10) / s;
			q.q.y = 0.25f * s;
			q.q.z = (mat.m12 + mat.m21) / s;
		}
		else {
			float s = 2.0f * std::sqrt(1.0f + mat.m22 - mat.m00 - mat.m11);
			q.q.w = (mat.m10 - mat.m01) / s;
			q.q.x = (mat.m02 + mat.m20) / s;
			q.q.y = (mat.m12 + mat.m21) / s;
			q.q.z = 0.25f * s;
		}
	}
	return q;
}

QuaternionF Slerp(const QuaternionF& q1, const QuaternionF& q2, float factor) {
	return Diligent::slerp(q1, q2, factor);
}

// Linearly interpolate between two float3 vectors
float3 Lerp(const float3& a, const float3& b, float factor) {
	return a * (1.0f - factor) + b * factor;
}

// Linearly interpolate between two float4x4 matrices
float4x4 Lerp(const float4x4& a, const float4x4& b, float factor) {
	float4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = a.m[i][j] * (1.0f - factor) + b.m[i][j] * factor;
		}
	}
	return result;
}

//




Diligent::QuaternionF SlerpQuaternion(const Diligent::QuaternionF& q1, const Diligent::QuaternionF& q2, float t)
{
	// Compute the cosine of the angle between the two vectors
	float dot = q1.q[0] * q2.q[0] + q1.q[1] * q2.q[1] + q1.q[2] * q2.q[2] + q1.q[3] * q2.q[3];

	// If the dot product is negative, slerp won't take the shorter path.
	// Fix by reversing one quaternion.
	Diligent::QuaternionF q2_adj = q2;
	if (dot < 0.0f) {
		q2_adj = Diligent::QuaternionF(-q2.q[0], -q2.q[1], -q2.q[2], -q2.q[3]);
		dot = -dot;
	}

	const float DOT_THRESHOLD = 0.9995f;
	if (dot > DOT_THRESHOLD) {
		// If the quaternions are close, use linear interpolation
		Diligent::QuaternionF result = Diligent::QuaternionF(
			q1.q[0] + t * (q2_adj.q[0] - q1.q[0]),
			q1.q[1] + t * (q2_adj.q[1] - q1.q[1]),
			q1.q[2] + t * (q2_adj.q[2] - q1.q[2]),
			q1.q[3] + t * (q2_adj.q[3] - q1.q[3])
		);
		return Diligent::normalize(result);
	}

	// Calculate the angle between the quaternions
	float theta_0 = std::acos(dot);
	float theta = theta_0 * t;

	float sin_theta = std::sin(theta);
	float sin_theta_0 = std::sin(theta_0);

	float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
	float s1 = sin_theta / sin_theta_0;

	Diligent::QuaternionF result = Diligent::QuaternionF(
		s0 * q1.q[0] + s1 * q2_adj.q[0],
		s0 * q1.q[1] + s1 * q2_adj.q[1],
		s0 * q1.q[2] + s1 * q2_adj.q[2],
		s0 * q1.q[3] + s1 * q2_adj.q[3]
	);

	return Diligent::normalize(result);
}



Diligent::QuaternionF QuaternionFromMatrix(const Diligent::float4x4& mat)
{
	float trace = mat.m00 + mat.m11 + mat.m22;
	Diligent::QuaternionF q;

	if (trace > 0.0f) {
		float s = std::sqrt(trace + 1.0f) * 2.0f;
		q.q[3] = 0.25f * s;
		q.q[0] = (mat.m21 - mat.m12) / s;
		q.q[1] = (mat.m02 - mat.m20) / s;
		q.q[2] = (mat.m10 - mat.m01) / s;
	}
	else if (mat.m00 > mat.m11 && mat.m00 > mat.m22) {
		float s = std::sqrt(1.0f + mat.m00 - mat.m11 - mat.m22) * 2.0f;
		q.q[3] = (mat.m21 - mat.m12) / s;
		q.q[0] = 0.25f * s;
		q.q[1] = (mat.m01 + mat.m10) / s;
		q.q[2] = (mat.m02 + mat.m20) / s;
	}
	else if (mat.m11 > mat.m22) {
		float s = std::sqrt(1.0f + mat.m11 - mat.m00 - mat.m22) * 2.0f;
		q.q[3] = (mat.m02 - mat.m20) / s;
		q.q[0] = (mat.m01 + mat.m10) / s;
		q.q[1] = 0.25f * s;
		q.q[2] = (mat.m12 + mat.m21) / s;
	}
	else {
		float s = std::sqrt(1.0f + mat.m22 - mat.m00 - mat.m11) * 2.0f;
		q.q[3] = (mat.m10 - mat.m01) / s;
		q.q[0] = (mat.m02 + mat.m20) / s;
		q.q[1] = (mat.m12 + mat.m21) / s;
		q.q[2] = 0.25f * s;
	}

	return Diligent::normalize(q);
}


// The FrameLerp function
void FrameLerp(const float3& pos1, const float4x4& rot1, const float3& scal1,
	const float3& pos2, const float4x4& rot2, const float3& scal2,
	float factor,
	float3& outPos, float4x4& outRot, float3& outScale) {
	outPos = Lerp(pos1, pos2, factor);
	//outRot = Lerp(rot1, rot2, factor);
	outScale = Lerp(scal1, scal2, factor);

	//
	Diligent::QuaternionF q1 = QuaternionFromMatrix(rot1);
	Diligent::QuaternionF q2 = QuaternionFromMatrix(rot2);

	// Perform quaternion slerp
	Diligent::QuaternionF result = SlerpQuaternion(q1, q2, factor);




	// Convert the resulting quaternion back to a rotation matrix
	outRot = result.ToMatrix().Inverse();
}

void Cinematic::SetTime(double time) {

	for (auto track : m_Tracks) {

		auto frames = track->GetFrames(time);

		if (frames.size() == 2) {

			float width = frames[1]->GetTime() - frames[0]->GetTime();

			float factor = (time-frames[0]->GetTime()) / width;

			float3 pos = frames[0]->GetPosition();
			float4x4 rot = frames[0]->GetRotation();
			float3 scale = frames[0]->GetScale();


			float3 t_pos = frames[1]->GetPosition();
			float4x4 t_rot = frames[1]->GetRotation();
			float3 t_scale = frames[1]->GetScale();

			float3 f_pos;
			float4x4 f_rot;
			float3 f_scale;

			FrameLerp(pos, rot, scale, t_pos, t_rot, t_scale, factor, f_pos, f_rot, f_scale);

			auto node = track->GetNode();

			node->SetPosition(f_pos);
			node->SetRotation(f_rot);
			node->SetScale(f_scale);
			continue;
		}
		if (frames.size() == 1) {

			if (frames[0]->GetTime() > time)
			{

				float width = frames[0]->GetTime();

				float factor = time / width;

				float3 pos = float3(0, 0, 0);
				float4x4 rot = float4x4::Identity();
				float3 scale = float3(1, 1, 1);

				float3 t_pos = frames[0]->GetPosition();
				float4x4 t_rot = frames[0]->GetRotation();
				float3 t_scale = frames[0]->GetScale();

				float3 f_pos;
				float4x4 f_rot;
				float3 f_scale;

				FrameLerp(pos, rot, scale, t_pos, t_rot, t_scale, factor,f_pos, f_rot, f_scale);
				 
				auto node = track->GetNode();

				node->SetPosition(f_pos);
				node->SetRotation(f_rot);
				node->SetScale(f_scale);


			}

		}

	}

}