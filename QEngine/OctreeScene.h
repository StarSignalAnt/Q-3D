#pragma once
#include "GraphNode.h"
 // Must be before the forward declaration of OctreeNode below
#include <vector>
#include <glm/glm.hpp>
#include <set> 



class SceneGraph;


struct SubMesh;
class CameraComponent;
class RenderTarget2D;
class Draw2D;

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
};

class OctreeNode;

class Octree
{
public:

	Octree(const Bounds& graph,GraphNode* cam);

    void Build(GraphNode* sceneRoot);
    void DebugLog() const;
    void Optimize();
    void RenderBruteForce(GraphNode* camera);
    void BakeRenderCache();
    void RenderCulled(GraphNode* camera);
    void SetViewCam(GraphNode* vcam) {
        m_ViewCam = vcam;
    }
private:
    void DebugLogRecursive(const OctreeNode* node, int depth, const std::string& path, size_t& nodeCount) const;

	SceneGraph* m_Owner = nullptr;
    void Insert(OctreeNode* node, const OctreeTriangle& triangle, int depth);

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

    // --- MODIFIED: Configuration settings for triangle count ---
    const int m_MaxTrianglesPerNode = 25000;
    const int m_MaxDepth = 22; // Polygon-level trees can be deeper
    void GetVisibleNodesRecursive(const OctreeNode* node, CameraComponent* camera,
        std::vector<const OctreeNode*>& visibleNodes,
        int& nodesTested, int& nodesVisible);
    std::vector<RenderTarget2D*> m_LightBuffers;
    Draw2D* m_Draw;
    GraphNode* m_Camera;
    GraphNode* m_ViewCam;
};

