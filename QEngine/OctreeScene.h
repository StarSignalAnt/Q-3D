#pragma once
#include "GraphNode.h"
// Must be before the forward declaration of OctreeNode below
#include <vector>
#include <glm/glm.hpp>
#include <set> 
#include <cstdint>
#include "StaticMeshComponent.h"
#include <thread> 
#include <mutex>   // Required for std::mutex
#include <queue> 
#include <optional> 
class SceneGraph;


struct SubMesh;
class SceneGraph;
class CameraComponent;
class RenderTarget2D;
class Draw2D;
class OctreeNode;
class RenderMaterial;

struct OctreeTriangle
{
    // The three indices of the triangle in its source SubMesh's vertex list.
    unsigned int v0, v1, v2;

    // The pre-calculated bounding box for this single triangle.
    Bounds bounds;

    // A pointer back to the original SubMesh for rendering and material access.
    SubMesh* sourceMesh = nullptr;
};



struct RenderBatchCache
{
    // Pointer to the original SubMesh to get material information.
    SubMesh* sourceSubMesh = nullptr;

    // The pre-baked GPU vertex and index buffers.
    RefCntAutoPtr<IBuffer> vertexBuffer;
    RefCntAutoPtr<IBuffer> indexBuffer;

    // The number of indices to draw for this batch.
    Uint32 indexCount = 0;
    std::vector<uint32_t> cpuIndexData;
    std::vector<Vertex3> cpuVertexData;
    RenderMaterial* m_Material = nullptr;
    RenderMaterial* m_DepthMaterial = nullptr;
    glm::mat4 m_WorldMatrix;
    ~RenderBatchCache() {}
};


struct RenderBatchPayload
{
    std::string MaterialPath;
    glm::mat4 WorldMatrix;
    std::vector<Vertex3> cpuVertexData;
    std::vector<uint32_t> cpuIndexData;
};

struct StreamedNodePayload
{
    OctreeNode* TargetNode;
    RenderBatchPayload LoadedBatch;
};

// NEW: Struct to hold a pair of pooled GPU buffers.
struct PooledBufferSet
{
    RefCntAutoPtr<IBuffer> vertexBuffer;
    RefCntAutoPtr<IBuffer> indexBuffer;
};

// NEW: Struct for managing retired buffers to prevent race conditions.
struct RetiredBufferSet
{
    PooledBufferSet buffers;
    uint64_t frameNumber;
};


class Octree
{
public:

    Octree(const Bounds& graph, GraphNode* cam);
    Octree(std::string path, GraphNode* camera);
    void SetGraph(SceneGraph* graph) {
        m_Graph = graph;
    }
    void Build(GraphNode* sceneRoot);
    void DebugLog() const;
    void Optimize();
    void RenderBruteForce(GraphNode* camera);
    void BakeRenderCache();
    void RenderCulled(GraphNode* camera);
    void RenderDepthCulled(GraphNode* camera);
    void SetViewCam(GraphNode* vcam) {
        m_ViewCam = vcam;
    }
    void Export(const std::string& path);
    std::vector<OctreeNode*> GetNodeVector();
    void LoadAllNodes();

    void CheckNodes();
    void FinalizeStreamedNodes();
private:
    void DebugLogRecursive(const OctreeNode* node, int depth, const std::string& path, size_t& nodeCount) const;
    void LoadAllNodesRecursive(OctreeNode* node, VFile* dataFile);
    SceneGraph* m_Owner = nullptr;
    void Insert(OctreeNode* node, const OctreeTriangle& triangle, int depth);
    void ExportRecursive(OctreeNode* node, VFile* idf, VFile* df);
    std::unique_ptr<OctreeNode> LoadRecursive(VFile* idf);
    // Subdivides a node, creating 8 children and distributing its triangles.
    void Subdivide(OctreeNode* node);
    void CollectAllTriangles(const OctreeNode* node, std::vector<OctreeTriangle>& allTriangles) const;
    std::vector<RenderBatchCache> m_RenderCache;
    // Helper to traverse the SceneGraph and kick off triangle insertion.
    void ExtractTriangles(GraphNode* graphNode);
    void BakeRenderCacheRecursive(OctreeNode* node);
    // Helper to check for AABB intersection.
    bool Intersects(const Bounds& a, const Bounds& b);
    void OptimizeRecursive(std::unique_ptr<OctreeNode>& node);
    std::unique_ptr<OctreeNode> m_Root;
    void CollectAllNodesRecursive(OctreeNode* node, std::vector<OctreeNode*>& nodes);
    // --- MODIFIED: Configuration settings for triangle count ---
    const int m_MaxTrianglesPerNode = 25000;
    const int m_MaxDepth = 64; // Polygon-level trees can be deeper
    void GetVisibleNodesRecursive(const OctreeNode* node, CameraComponent* camera,
        std::vector<const OctreeNode*>& visibleNodes,
        int& nodesTested, int& nodesVisible);

    void InitializeBufferPool(size_t initialSize);
    PooledBufferSet AcquireBufferSet();
    void ReleaseBufferSet(PooledBufferSet& bufferSet);

    void CheckNodesRecursive(OctreeNode* node, const Bounds& streamingBox);
    void StreamNode(OctreeNode* node, std::string dataFilePath);
    void UnstreamNode(OctreeNode* node);
    PooledBufferSet CreateBufferSet();
    void ProcessRetiredBuffers(); // NEW: Manages delayed buffer reuse

    std::vector<RenderTarget2D*> m_LightBuffers;
    Draw2D* m_Draw;
    GraphNode* m_Camera;
    GraphNode* m_ViewCam;
    int m_NextNodeID = 0;
    std::string m_DataFilePath;
    std::queue<StreamedNodePayload> m_StreamerQueue;
    mutable std::mutex m_StreamerMutex;
    // NEW: The distance at which nodes should be streamed in.
    float m_StreamMinDistance = 100.0f;
    glm::vec3 m_StreamingBoxExtents = glm::vec3(350.0f, 350.0f, 350.0f);
    SceneGraph* m_Graph = nullptr;

    // --- Buffer Pool Members ---
    std::vector<PooledBufferSet> m_BufferPool;
    std::vector<RetiredBufferSet> m_RetiredBuffers; // NEW: Queue for safe buffer recycling
    mutable std::mutex m_BufferPoolMutex;
    uint64_t m_CurrentFrame = 0; // NEW: Frame counter for buffer retirement
    const uint32_t m_BufferRetireFrameCount = 3; // Retire buffers for 3 frames
};
