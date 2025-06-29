#pragma once
#include <vector>
#include <memory>
#include "GraphNode.h" // For Bounds struct
#include "OctreeScene.h"
class OctreeNode
{
public:
    OctreeNode(const Bounds& bounds);

    const Bounds& GetBounds() const { return m_Bounds; }
    const std::unique_ptr<OctreeNode>* GetChildren() const { return m_Children; }

    // MODIFIED: Returns the list of triangles contained within this node.
    const std::vector<OctreeTriangle>& GetTriangles() const { return m_Triangles; }
    const std::vector<std::unique_ptr<RenderBatchCache>>& GetRenderBatches() const { return m_RenderBatches; }
private:
    friend class Octree;

    Bounds m_Bounds;

    // MODIFIED: Stores the actual triangles, not GraphNode pointers.
    std::vector<OctreeTriangle> m_Triangles;

    std::unique_ptr<OctreeNode> m_Children[8];
    std::vector<std::unique_ptr<RenderBatchCache>> m_RenderBatches;

};

