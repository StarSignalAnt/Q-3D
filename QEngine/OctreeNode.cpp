#include "OctreeNode.h"
#include "StaticMeshComponent.h"
OctreeNode::OctreeNode(const Bounds& bounds,int id)
    : m_Bounds(bounds), m_ID(id), m_RenderReady(true), m_dataOffset(0),
    m_dataSize(0)
{
    // The constructor's primary role is to initialize the node's bounding box.
    // The m_Bounds member is initialized using the member initializer list.

    // The m_Triangles vector is default-initialized to be empty.

    // The m_Children array of unique_ptr is default-initialized to all be nullptr.
}void OctreeNode::WriteNode(VFile* file)
{
    int32_t batchCount = m_RenderBatches.size();
    file->WriteInt(batchCount);

    for (const auto& batch_ptr : m_RenderBatches)
    {
        const RenderBatchCache* batch = batch_ptr.get();
        if (!batch || !batch->m_Material) {
            file->WriteString("");
            file->WriteInt(0);
            file->WriteInt(0);
            glm::mat4 identity(1.0f);
            file->WriteMatrix(identity);
            continue;
        }

        // 1. Write the material path.
        std::string materialPath = batch->m_Material->GetPath();
        file->WriteString(materialPath.c_str());

        // 2. Write the World Matrix
        file->WriteMatrix(batch->m_WorldMatrix);

        // 3. Write the OPTIMIZED vertex data
        const auto& vertices = batch->cpuVertexData;
        int32_t numVertices = vertices.size();
        file->WriteInt(numVertices);

        if (numVertices > 0)
        {
            size_t verticesDataSize = numVertices * sizeof(vertices[0]);
            file->WriteBytes((void*)vertices.data(), verticesDataSize);
        }

        // 4. Write the OPTIMIZED and remapped index data
        const auto& indices = batch->cpuIndexData;
        int32_t indexCount = indices.size();
        file->WriteInt(indexCount);

        if (indexCount > 0)
        {
            size_t indicesDataSize = indexCount * sizeof(indices[0]);
            file->WriteBytes((void*)indices.data(), indicesDataSize);
        }
    }
}
