#pragma once
#include <vector>
#include <memory>
#include "GraphNode.h" // For Bounds struct
#include "OctreeScene.h"
#include <atomic>

class OctreeNode
{
public:
    OctreeNode(const Bounds& bounds, int id);
    int GetID() const { return m_ID; }


    const Bounds& GetBounds() const { return m_Bounds; }
    const std::unique_ptr<OctreeNode>* GetChildren() const { return m_Children; }
    void WriteNode(VFile* file);
    // MODIFIED: Returns the list of triangles contained within this node.
    const std::vector<OctreeTriangle>& GetTriangles() const { return m_Triangles; }
    const std::vector<std::unique_ptr<RenderBatchCache>>& GetRenderBatches() const { return m_RenderBatches; }
    bool IsRenderReady() const { return m_RenderReady; }
    void SetRenderReady(bool ready) { m_RenderReady = ready; }
    bool IsStreaming() const { return m_IsStreaming; }
    void SetStreaming(bool streaming) { m_IsStreaming = streaming; }
private:
    friend class Octree;

    Bounds m_Bounds;
    int m_ID;
    // MODIFIED: Stores the actual triangles, not GraphNode pointers.
    std::vector<OctreeTriangle> m_Triangles;
    bool m_RenderReady;

    // NEW: Members to store location in the data file for streaming.
    long m_dataOffset;
    long m_dataSize;
    std::atomic<bool> m_IsStreaming{ false };
    std::unique_ptr<OctreeNode> m_Children[8];
    std::vector<std::unique_ptr<RenderBatchCache>> m_RenderBatches;

};

