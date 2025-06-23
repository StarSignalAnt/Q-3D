#pragma once
#include <cmath> // For M_PI constant
#include "BasicMath.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace Diligent;
constexpr double MPI = 3.14159265358979323846;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <random>
class MathsHelp
{
public:

	static float Deg2Rad(float deg) {
		return deg * (float)MPI / 180.0;
	}
	static float Rad2Deg(float r)
	{
		float degrees = r * (180.0f / static_cast<float>(M_PI));
		// Normalize degrees to be within 0 to 360
	
		return degrees;
	}

	static float Distance(float3 v1, float3 v2);


	static bool IsPositionInsideBounds(float3 centre, float3 size, float3 position) {
		// Compute half extents
		float3 halfSize = size / 2.0f;

		// Calculate the min and max bounds of the box
		float3 minBounds = centre - halfSize;
		float3 maxBounds = centre + halfSize;

		// Check if the position is within the bounds
		return (position.x >= minBounds.x && position.x <= maxBounds.x &&
			position.y >= minBounds.y && position.y <= maxBounds.y &&
			position.z >= minBounds.z && position.z <= maxBounds.z);
	}
	static float4 getRandomFloat4(float min, float max) {
		// Use random device and mt19937 for random number generation
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(min, max);

		// Generate random components for float4
		float4 randomValue;
		randomValue.x = dis(gen);
		randomValue.y = dis(gen);
		randomValue.z = dis(gen);
		randomValue.w = dis(gen);

		return randomValue;
	}

	static float3 atov(aiVector3D v)
	{

		return float3(v.x, v.y, v.z);

	}

	static Diligent::float4x4 atov(const aiMatrix4x4& assimpMat) {
		Diligent::float4x4 diligentMat;

		diligentMat.m00 = assimpMat.a1; diligentMat.m01 = assimpMat.a2; diligentMat.m02 = assimpMat.a3; diligentMat.m03 = assimpMat.a4;
		diligentMat.m10 = assimpMat.b1; diligentMat.m11 = assimpMat.b2; diligentMat.m12 = assimpMat.b3; diligentMat.m13 = assimpMat.b4;
		diligentMat.m20 = assimpMat.c1; diligentMat.m21 = assimpMat.c2; diligentMat.m22 = assimpMat.c3; diligentMat.m23 = assimpMat.c4;
		diligentMat.m30 = assimpMat.d1; diligentMat.m31 = assimpMat.d2; diligentMat.m32 = assimpMat.d3; diligentMat.m33 = assimpMat.d4;

		return diligentMat;
	}
	static Diligent::float4x4 atov(Diligent::QuaternionF quat) {
		// Normalize the quaternion to ensure the matrix is a proper rotation matrix
		Diligent::QuaternionF normalizedQuat = quat;
		normalizedQuat = Diligent::normalize<float>(quat);





		// Extract the quaternion components
		float x = normalizedQuat.q.x;
		float y = normalizedQuat.q.y;
		float z = normalizedQuat.q.z;
		float w = normalizedQuat.q.w;

		// Calculate the matrix elements
		Diligent::float4x4 rotationMatrix;
		rotationMatrix.m00 = 1 - 2 * (y * y + z * z);
		rotationMatrix.m01 = 2 * (x * y - z * w);
		rotationMatrix.m02 = 2 * (x * z + y * w);
		rotationMatrix.m03 = 0;

		rotationMatrix.m10 = 2 * (x * y + z * w);
		rotationMatrix.m11 = 1 - 2 * (x * x + z * z);
		rotationMatrix.m12 = 2 * (y * z - x * w);
		rotationMatrix.m13 = 0;

		rotationMatrix.m20 = 2 * (x * z - y * w);
		rotationMatrix.m21 = 2 * (y * z + x * w);
		rotationMatrix.m22 = 1 - 2 * (x * x + y * y);
		rotationMatrix.m23 = 0;

		rotationMatrix.m30 = 0;
		rotationMatrix.m31 = 0;
		rotationMatrix.m32 = 0;
		rotationMatrix.m33 = 1;

		return rotationMatrix;
	}

};

