#pragma once
#include "BasicMath.hpp"
using namespace Diligent;

class BoneInfo
{
public:

	int ID = 0;
	float4x4 Offset = float4x4::Identity();

};

