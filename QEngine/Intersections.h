#pragma once
#include "CLBase.h"
#include "BasicMath.hpp"
#include <map>
#include <unordered_map>
#include <mutex>
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
    // Separate buffer sets for each cast type to prevent interference
    struct CastBuffers {
        cl::Buffer posBuffer;
        cl::Buffer dirBuffer;
        cl::Buffer resultBuffer;
        cl::Buffer hitPointBuffer;
        bool initialized = false;
    };

    CastBuffers m_MeshBuffers;
    CastBuffers m_TerrainBuffers;

    // Thread safety
    std::mutex m_CastMutex;

    // Geometry buffers (separate for each mesh type)
    std::unordered_map<SubMesh*, cl::Buffer> m_TriBuffers;
    std::unordered_map<TerrainMesh*, cl::Buffer> m_TBuffers;

    // Helper methods
    void InitializeMeshBuffers();
    void InitializeTerrainBuffers();
    size_t GetOptimalWorkGroupSize(size_t numTris) const;
    size_t RoundUpToMultiple(size_t value, size_t multiple) const;

    // Error handling helper
    bool CheckCLError(cl_int err, const char* operation) const;

    // Legacy members (keep for compatibility but unused)
    cl::Buffer triBuf;
    bool first = true;
    int num_tris = 0;
    std::map<SubMesh*, cl::Buffer> m_Buffers;
};