#pragma once
#include "CLBase.h"
#include "BasicMath.hpp"
#include <map>
#include "StaticMeshComponent.h"
using namespace Diligent;


class TerrainMesh;
struct SubMesh;

struct CastResult {

	float Distance = 0.0;
	bool Hit = false;
    int MeshIndex = -1;
    float3 HitPoint = { 0.0f, 0.0f, 0.0f };

};
class Intersections : public CLBase
{
public:
    Intersections();
    CastResult CastMesh(float3 pos, float3 dir, SubMesh* mesh);
    CastResult CastTerrainMesh(float3 pos, float3 dir, TerrainMesh* mesh);

private:
    // Persistent buffers for optimization
    cl::Buffer m_PosBuffer;
    cl::Buffer m_DirBuffer;
    cl::Buffer m_ResultBuffer;
    cl::Buffer m_HitPointBuffer;
    std::unordered_map<SubMesh*, cl::Buffer> m_TriBuffers;
    bool m_BuffersInitialized = false;

    // Helper methods
    void InitializeBuffers();
    size_t GetOptimalWorkGroupSize(size_t numTris) const;
    size_t RoundUpToMultiple(size_t value, size_t multiple) const;

    // Legacy members (keep for compatibility)
    cl::Buffer triBuf;
    bool first = true;
    int num_tris = 0;
    std::map<SubMesh*, cl::Buffer> m_Buffers;
    std::map<TerrainMesh*, cl::Buffer> m_TBuffers;
};
