#pragma once
#include "BasicMath.hpp"

using namespace Diligent;

struct Vertex {

	float3 position; //0
	float4 color; //3
	float3 texture; //7
	float3 normal; //10
	float3 binormal; //13
	float3 tangent; //16 
	float4 bone_ids; //19
	float4 bone_weights; //23

};

struct Triangle
{

	Uint32 v0=0, v1=0, v2=0;

};

struct TerrainVertex {

	float3 position;
	float4 color;
	float3 texture;
	float3 normal;
	float3 binormal;
	float3 tangent;
	float3 layercoord;

};