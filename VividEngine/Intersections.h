#pragma once
#include "CLBase.h"
#include "BasicMath.hpp"
#include <map>

using namespace Diligent;

class Mesh3D;
class TerrainMesh;

struct CastResult {

	float Distance = 0.0;
	bool Hit = false;

};

class Intersections : public CLBase
{
public:

	Intersections();
	CastResult CastMesh(float3 pos, float3 dir, Mesh3D* mesh);
	CastResult CastTerrainMesh(float3 pos, float3 dir, TerrainMesh* mesh);

private:
	cl::Buffer triBuf;
	bool first = true;
	int num_tris = 0;
	std::map<Mesh3D*, cl::Buffer> m_Buffers;
	std::map<TerrainMesh*, cl::Buffer> m_TBuffers;
};


