#include "OctreeNode.h"
OctreeNode::OctreeNode(const Bounds& bounds)
    : m_Bounds(bounds)
{
    // The constructor's primary role is to initialize the node's bounding box.
    // The m_Bounds member is initialized using the member initializer list.

    // The m_Triangles vector is default-initialized to be empty.

    // The m_Children array of unique_ptr is default-initialized to all be nullptr.
}