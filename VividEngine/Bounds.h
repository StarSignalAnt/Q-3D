#pragma once
#include "BasicMath.hpp"
using namespace Diligent;

class Bounds
{
public:

	Bounds() {

	};
	float3 Min;
	float3 Max;
	float3 Centre;
	float3 Size() {

		return (Max - Min);

	}

};

