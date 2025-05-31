#include "pch.h"
#include "MathsHelp.h"

#include "BasicMath.hpp"

using namespace Diligent;

float MathsHelp::Distance(float3 v1, float3 v2) {

	float xd, yd, zd;
	xd = v2.x - v1.x;
	yd = v2.y - v1.y;
	zd = v2.z - v1.z;

	float dis = sqrt(xd * xd + yd * yd + zd * zd);

	return dis;

}