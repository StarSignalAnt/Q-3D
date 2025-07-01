#include "OctreeScene.h"
#include "OctreeNode.h"
#include "StaticMeshComponent.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "SceneGraph.h"
#include "QEngine.h"
#include "CameraComponent.h"
#include "RenderTarget2D.h"
#include "Draw2D.h"
#include "VFile.h"
#include "MaterialDepth.h"
#include "MaterialPBR.h"

Octree::Octree(const Bounds& sceneBounds,GraphNode* camera) {
    m_Root = std::make_unique<OctreeNode>(sceneBounds, m_NextNodeID++);
    m_Draw = new Draw2D(camera);


}


// --- PUBLIC BUILD FUNCTION ---
void Octree::Build(GraphNode* sceneRoot) {
    if (!sceneRoot || !m_Root) return;

    // Start the recursive extraction and insertion process from the root of the scene graph.
    ExtractTriangles(sceneRoot);
}

// --- PRIVATE SCENEGRAPH TRAVERSAL HELPER ---
void Octree::ExtractTriangles(GraphNode* graphNode) {
    if (!graphNode) return;

    if (auto meshComp = graphNode->GetComponent<StaticMeshComponent>()) { //
        glm::mat4 worldMatrix = graphNode->GetWorldMatrix(); //

        for (auto* subMesh : meshComp->GetSubMeshes()) { //
            if (!subMesh) continue;

            const auto& vertices = subMesh->m_Vertices; //
            const auto& triangles = subMesh->m_Triangles; //

            for (const auto& tri : triangles) {
                OctreeTriangle oct_tri;

                // Store the original indices
                oct_tri.v0 = tri.v0;
                oct_tri.v1 = tri.v1;
                oct_tri.v2 = tri.v2;

                // Temporarily transform vertices just to calculate the triangle's bounds
                glm::vec3 p0 = glm::vec3(worldMatrix * glm::vec4(vertices[tri.v0].position, 1.0f)); //
                glm::vec3 p1 = glm::vec3(worldMatrix * glm::vec4(vertices[tri.v1].position, 1.0f)); //
                glm::vec3 p2 = glm::vec3(worldMatrix * glm::vec4(vertices[tri.v2].position, 1.0f)); //

                oct_tri.bounds.min = glm::min(p0, glm::min(p1, p2));
                oct_tri.bounds.max = glm::max(p0, glm::max(p1, p2));
                oct_tri.bounds.CalculateDerivedValues(); //
                oct_tri.sourceMesh = subMesh;

                Insert(m_Root.get(), oct_tri, 0);
            }
        }
    }

    for (auto* child : graphNode->GetNodes()) { //
        ExtractTriangles(child);
    }
}void Octree::Insert(OctreeNode* node, const OctreeTriangle& triangle, int depth)
{
    // If the node is a branch, pass the triangle down to any children it intersects.
    if (node->m_Children[0] != nullptr)
    {
        for (int i = 0; i < 8; ++i)
        {
            if (Intersects(triangle.bounds, node->m_Children[i]->GetBounds()))
            {
                Insert(node->m_Children[i].get(), triangle, depth + 1);
            }
        }
        return;
    }

    // --- If we reach here, the node is a leaf. Add the triangle. ---
    node->m_Triangles.push_back(triangle);

    // --- Check if the leaf needs to subdivide ---
    if (node->m_Triangles.size() > m_MaxTrianglesPerNode && depth < m_MaxDepth)
    {
        // 1. Create the 8 empty child nodes. This turns the current node into a branch.
        Subdivide(node);

        // 2. We now have a list of triangles in this node that MUST be moved
        //    down into the newly created children.
        for (const auto& tri_to_move : node->m_Triangles)
        {
            for (int i = 0; i < 8; ++i)
            {
                if (Intersects(tri_to_move.bounds, node->m_Children[i]->GetBounds()))
                {
                    // Call Insert on the child directly.
                    Insert(node->m_Children[i].get(), tri_to_move, depth + 1);
                }
            }
        }

        // 3. Finally, clear the triangles from this node, as they are now
        //    held by its children. It is now a pure branch node.
        node->m_Triangles.clear();
    }
}
 void Octree::Subdivide(OctreeNode * node)
{
    Bounds parentBounds = node->GetBounds();
    glm::vec3 center = parentBounds.center;
    glm::vec3 childSize = parentBounds.size * 0.5f;

    for (int i = 0; i < 8; ++i)
    {
        glm::vec3 childMin = parentBounds.min;
        if (i & 1) childMin.x = center.x; // Right
        if (i & 2) childMin.y = center.y; // Top
        if (i & 4) childMin.z = center.z; // Front

        Bounds childBounds;
        childBounds.min = childMin;
        childBounds.max = childMin + childSize;
        childBounds.CalculateDerivedValues();

        node->m_Children[i] = std::make_unique<OctreeNode>(childBounds, m_NextNodeID++);
    }
}

// --- AABB INTERSECTION HELPER ---
bool Octree::Intersects(const Bounds& a, const Bounds& b) {
    // --- NEW: Add a small epsilon for robust floating-point comparisons. ---
    constexpr float OCTREE_EPSILON = 1e-5f;

    return (a.min.x <= b.max.x + OCTREE_EPSILON && a.max.x >= b.min.x - OCTREE_EPSILON) &&
        (a.min.y <= b.max.y + OCTREE_EPSILON && a.max.y >= b.min.y - OCTREE_EPSILON) &&
        (a.min.z <= b.max.z + OCTREE_EPSILON && a.max.z >= b.min.z - OCTREE_EPSILON);

}

static std::string Vec3ToString(const glm::vec3& v) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2)
        << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return ss.str();
}

// --- NEW PUBLIC FUNCTION IMPLEMENTATION ---
void Octree::DebugLog() const
{
    if (!m_Root) {
        std::cout << "Octree is empty or has not been built." << std::endl;
        return;
    }

    size_t nonEmptyNodeCount = 0; // Initialize counter

    std::cout << "\n--- Octree Debug Log ---\n" << std::endl;

    // Start the recursion, passing the counter by reference.
    DebugLogRecursive(m_Root.get(), 0, "Root", nonEmptyNodeCount);

    // Print the final summary count.
    std::cout << "\nTotal Nodes with Triangles: " << nonEmptyNodeCount << std::endl;
    std::cout << "\n--- End of Log ---\n" << std::endl;
}

void Octree::DebugLogRecursive(const OctreeNode* node, int depth, const std::string& path, size_t& nodeCount) const
{
    if (!node) return;

    const size_t triCount = node->GetTriangles().size(); //

    // --- Core Logic Change ---
    // Only print information and increment the count if the node actually contains triangles.
    if (triCount > 0)
    {
        nodeCount++;

        std::string indent(depth * 2, ' ');
        const Bounds& bounds = node->GetBounds(); //

        // Print the details for the current non-empty node.
        std::cout << indent << "- Node: " << path << " (ID: " << node->GetID() << ", Depth: " << depth << ")" << std::endl;
        std::cout << indent << "  Bounds Min: " << Vec3ToString(bounds.min) << std::endl;
        std::cout << indent << "  Bounds Max: " << Vec3ToString(bounds.max) << std::endl;
        std::cout << indent << "  Triangle Count: " << triCount << std::endl;
    }

    // --- Traversal remains the same ---
    // We must still recurse through all children, even if the current branch node is "empty".
    // This is how we find the non-empty leaf nodes deep inside the tree.
    for (int i = 0; i < 8; ++i)
    {
        if (node->GetChildren()[i] != nullptr) //
        {
            DebugLogRecursive(node->GetChildren()[i].get(), depth + 1, path + "/" + std::to_string(i), nodeCount);
        }
    }
}
void Octree::Optimize()
{
    if (!m_Root) return;
    std::cout << "Optimizing Octree by rebuilding..." << std::endl;

    // 1. Collect all essential triangles from the non-empty leaves of the current tree.
    std::vector<OctreeTriangle> allGoodTriangles;
    CollectAllTriangles(m_Root.get(), allGoodTriangles);
    m_NextNodeID = 0;
    // 2. Create a new root node for our new, optimized tree.
    // It will have the same initial bounds as the old one.
    Bounds rootBounds = m_Root->GetBounds();
    auto newRoot = std::make_unique<OctreeNode>(rootBounds, m_NextNodeID++);

    // 3. Re-insert every essential triangle into the new tree.
    // The Insert function will build the new tree structure correctly and efficiently.
    for (const auto& tri : allGoodTriangles)
    {
        Insert(newRoot.get(), tri, 0);
    }

    // 4. Replace the old, un-optimized tree root with our new one.
    // The old tree is automatically deleted by the smart pointer.
    m_Root = std::move(newRoot);

    std::cout << "Rebuild complete. The new tree is optimized." << std::endl;
}
void Octree::OptimizeRecursive(std::unique_ptr<OctreeNode>& node)
{
    if (!node) return;

    // First, check if this node is a branch (has children).
    // This is checked BEFORE we recurse.
    bool isBranch = (node->m_Children[0] != nullptr);

    // If it's a branch, we recursively call optimize on its children.
    if (isBranch)
    {
        for (int i = 0; i < 8; ++i)
        {
            OptimizeRecursive(node->m_Children[i]);
        }
        // After recursing, a branch node has done its job. We NEVER prune it.
        return;
    }

    // --- If we reach here, the node MUST be a leaf node. ---

    // We can now safely prune this leaf ONLY if its triangle list is empty.
    if (node->GetTriangles().empty())
    {
        node.reset(); // This deletes the empty leaf node.
    }
}
void Octree::RenderBruteForce(GraphNode* camera) {
    if (m_RenderCache.empty() || !camera) return;

    bool firstLightPass = true;
    for (auto light : SceneGraph::m_CurrentGraph->GetLights()) { //
        for (const auto& batch : m_RenderCache) {
            RenderMaterial* mat = batch.sourceSubMesh->m_Material; //
            if (!mat || batch.indexCount == 0 || !batch.vertexBuffer) continue;

            mat->SetIndexCount(batch.indexCount);
            mat->SetBuffer(batch.vertexBuffer, 0); // Use the ORIGINAL, complete Vertex Buffer
            mat->SetBuffer(batch.indexBuffer, 1);  // Use the NEW baked Index Buffer

            // The world matrix is now the ORIGINAL object's world matrix.
            // This is because the vertices in the VB are in local object space.
            mat->SetMatrix(glm::inverse(camera->GetWorldMatrix()), 0); //
            mat->SetMatrix(batch.sourceSubMesh->m_Owner->GetWorldMatrix(), 1); //
            mat->SetMatrix(camera->GetComponent<CameraComponent>()->GetProjectionMatrix(), 2); //
            mat->SetCameraPosition(camera->GetPosition()); //
            mat->SetLight(light);

            mat->Bind(!firstLightPass);
            mat->Render();

            firstLightPass = false;
        }
    }
}

// --- NEW: RECURSIVE HELPER TO COLLECT TRIANGLES ---
void Octree::CollectAllTriangles(const OctreeNode* node, std::vector<OctreeTriangle>& triangles) const
{
    if (!node) return;

    // If it's a leaf node, add its triangles to our master list.
    if (node->m_Children[0] == nullptr)
    {
        if (!node->GetTriangles().empty())
        {
            triangles.insert(triangles.end(), node->GetTriangles().begin(), node->GetTriangles().end());
        }
    }
    else // If it's a branch, recurse into its children.
    {
        for (int i = 0; i < 8; ++i)
        {
            if (node->m_Children[i] != nullptr)
            {
                CollectAllTriangles(node->GetChildren()[i].get(), triangles);
            }
        }
    }
}


void Octree::BakeRenderCache() {
    if (!m_Root) return;
    std::cout << "Baking Node-Based Render Cache..." << std::endl;
    BakeRenderCacheRecursive(m_Root.get());
    std::cout << "Node-based baking complete." << std::endl;
}
void Octree::RenderCulled(GraphNode* camera)
{


    if (!m_Root || !camera) return;

    if (m_LightBuffers.size() == 0) {
        for (auto light : SceneGraph::m_CurrentGraph->GetLights()) {
            m_LightBuffers.push_back(new RenderTarget2D(QEngine::GetFrameWidth(), QEngine::GetFrameHeight(),false));
        }
    }


    auto cameraComponent = camera->GetComponent<CameraComponent>();
    if (!cameraComponent) return;

    std::vector<const OctreeNode*> visibleNodes;
    int nodesTested = 0;
    int nodesVisible = 0;
    GetVisibleNodesRecursive(m_Root.get(), cameraComponent, visibleNodes, nodesTested, nodesVisible);

    size_t trisRendered = 0;
    size_t batchCount = 0;
    for (const auto* node : visibleNodes) {
        for (const auto& batch : node->GetRenderBatches()) {
            trisRendered += batch->indexCount / 3;
            batchCount++;
        }
    }

    std::cout << "\r" << "Visible Nodes: " << nodesVisible << " / " << nodesTested
        << " | Batches (Draw Calls): " << batchCount
        << " | Tris Rendered: " << trisRendered << "      ";

    if (visibleNodes.empty()) return;

    bool firstLightPass = true;
    int li = 0;
    for (auto light : SceneGraph::m_CurrentGraph->GetLights()) { //
        
        auto lb = m_LightBuffers[li];
        lb->SetClearCol(glm::vec4(0, 0, 0, 1));
        lb->Bind();
        for (const auto* node : visibleNodes)
        {
            for (const auto& batch_ptr : node->GetRenderBatches()) //
            {
                const RenderBatchCache* batch = batch_ptr.get();
                if (!batch) continue;

                RenderMaterial* mat = batch->m_Material;
                if (!mat || batch->indexCount == 0 || !batch->vertexBuffer) continue;

                mat->SetIndexCount(batch->indexCount);
                mat->SetBuffer(batch->vertexBuffer, 0);
                mat->SetBuffer(batch->indexBuffer, 1);
                mat->SetMatrix(glm::inverse(camera->GetWorldMatrix()), 0);

                // MODIFIED: Use the batch's local world matrix
                mat->SetMatrix(batch->m_WorldMatrix, 1);

                mat->SetMatrix(cameraComponent->GetProjectionMatrix(), 2);
                mat->SetCameraPosition(camera->GetPosition());
                mat->SetLight(light);
                //mat->Bind(false);
                //mat->Render();



                auto lb = m_LightBuffers[li];

             

                mat->Bind(false);
                mat->Render();
                
            }
        }
        // --- CRITICAL FIX: Move this line outside the inner loops ---
        // This ensures that the NEXT light source triggers the additive blending state,
        // not the next render batch.
        firstLightPass = false;
        lb->Release();
        li++;
    }

    //QEngine::ClearZ();
    QEngine::SetScissor(0, 0, QEngine::GetFrameWidth(), QEngine::GetFrameHeight());
    bool add = false;
    
    for (auto lb : m_LightBuffers) {

        QEngine::ClearZ();
        m_Draw->SetAdditive(add);
        m_Draw->BeginFrame();
        m_Draw->Rect(lb->GetTexture2D(), glm::vec2(0, 0), glm::vec2(QEngine::GetFrameWidth(), QEngine::GetFrameHeight()), glm::vec4(1, 1, 1, 1));
        m_Draw->Flush();
        add = true;

    }
}
void Octree::GetVisibleNodesRecursive(const OctreeNode* node, CameraComponent* camera,
    std::vector<const OctreeNode*>& visibleNodes,
    int& nodesTested, int& nodesVisible)
{
    if (!node) return;
    nodesTested++;

    if (!camera->InFrustum(node->GetBounds())) {
        return;
    }
    nodesVisible++;

    // If it's a leaf node, check if it's ready to be rendered.
    if (node->GetChildren()[0] == nullptr) {
        // MODIFIED: Only add leaf nodes that are render-ready and have batches.
        if (node->IsRenderReady() && !node->GetRenderBatches().empty()) {
            visibleNodes.push_back(node);
        }
        return;
    }

    // If it's a branch, just recurse. The check happens at the leaf level.
    for (int i = 0; i < 8; ++i) {
        if (node->GetChildren()[i] != nullptr) {
            GetVisibleNodesRecursive(node->GetChildren()[i].get(), camera, visibleNodes, nodesTested, nodesVisible);
        }
    }
}void Octree::BakeRenderCacheRecursive(OctreeNode* node) {
    if (!node) return;

    if (node->m_Children[0] != nullptr) {
        for (int i = 0; i < 8; ++i) {
            BakeRenderCacheRecursive(node->m_Children[i].get());
        }
        return;
    }

    if (node->m_Triangles.empty()) return;

    std::map<SubMesh*, std::vector<OctreeTriangle>> subMeshGroups;
    for (const auto& tri : node->m_Triangles) {
        subMeshGroups[tri.sourceMesh].push_back(tri);
    }

    for (const auto& pair : subMeshGroups) {
        SubMesh* sourceSubMesh = pair.first;
        const auto& trianglesInBatch = pair.second;
        const auto& sourceVertices = sourceSubMesh->m_Vertices;

        auto batch = std::make_unique<RenderBatchCache>();
        batch->sourceSubMesh = sourceSubMesh;

        // NEW: Copy material and matrix info directly into the batch
        batch->m_Material = sourceSubMesh->m_Material;
        batch->m_DepthMaterial = sourceSubMesh->m_DepthMaterial;
        batch->m_WorldMatrix = sourceSubMesh->m_Owner->GetWorldMatrix();

        std::map<uint32_t, uint32_t> oldIndexToNewIndex;
        std::vector<Vertex3> uniqueVertices;
        std::vector<uint32_t> remappedIndices;
        remappedIndices.reserve(trianglesInBatch.size() * 3);

        for (const auto& tri : trianglesInBatch) {
            uint32_t originalIndices[] = { tri.v0, tri.v1, tri.v2 };
            for (uint32_t oldIndex : originalIndices) {
                if (oldIndexToNewIndex.find(oldIndex) == oldIndexToNewIndex.end()) {
                    uint32_t newIndex = static_cast<uint32_t>(uniqueVertices.size());
                    oldIndexToNewIndex[oldIndex] = newIndex;
                    uniqueVertices.push_back(sourceVertices[oldIndex]);
                }
                remappedIndices.push_back(oldIndexToNewIndex[oldIndex]);
            }
        }

        BufferDesc vbDesc;
        vbDesc.Name = "Octree Node Optimized VB";
        vbDesc.Usage = USAGE_IMMUTABLE;
        vbDesc.BindFlags = BIND_VERTEX_BUFFER;
        vbDesc.Size = sizeof(uniqueVertices[0]) * uniqueVertices.size();
        BufferData vbInitData;
        vbInitData.pData = uniqueVertices.data();
        vbInitData.DataSize = vbDesc.Size;
        QEngine::m_pDevice->CreateBuffer(vbDesc, &vbInitData, &batch->vertexBuffer);

        BufferDesc ibDesc;
        ibDesc.Name = "Octree Node Optimized IB";
        ibDesc.Usage = USAGE_IMMUTABLE;
        ibDesc.BindFlags = BIND_INDEX_BUFFER;
        ibDesc.Size = sizeof(uint32_t) * remappedIndices.size();
        BufferData ibInitData_IB;
        ibInitData_IB.pData = remappedIndices.data();
        ibInitData_IB.DataSize = ibDesc.Size;
        QEngine::m_pDevice->CreateBuffer(ibDesc, &ibInitData_IB, &batch->indexBuffer);

        batch->indexCount = remappedIndices.size();
        batch->cpuVertexData = std::move(uniqueVertices);
        batch->cpuIndexData = std::move(remappedIndices);
        node->m_RenderBatches.push_back(std::move(batch));
    }
    node->m_Triangles.clear();
}

void Octree::RenderDepthCulled(GraphNode* camera) {


    if (!m_Root || !camera) return;




    auto cameraComponent = camera->GetComponent<CameraComponent>();
    if (!cameraComponent) return;

    std::vector<const OctreeNode*> visibleNodes;
    int nodesTested = 0;
    int nodesVisible = 0;
    GetVisibleNodesRecursive(m_Root.get(), cameraComponent, visibleNodes, nodesTested, nodesVisible);

    size_t trisRendered = 0;
    size_t batchCount = 0;
    for (const auto* node : visibleNodes) {
        for (const auto& batch : node->GetRenderBatches()) {
            trisRendered += batch->indexCount / 3;
            batchCount++;
        }
    }

    if (visibleNodes.empty()) return;

    bool firstLightPass = true;
    int li = 0;
    //for (auto light : SceneGraph::m_CurrentGraph->GetLights()) { //


       // auto lb = m_LightBuffers[li];
      //  lb->SetClearCol(glm::vec4(0, 0, 0, 1));
      //  lb->Bind();
        for (const auto* node : visibleNodes)
        {
            for (const auto& batch_ptr : node->GetRenderBatches()) //
            {
                const RenderBatchCache* batch = batch_ptr.get();
                if (!batch) continue;

                RenderMaterial* mat = batch->m_DepthMaterial;
                if (!mat || batch->indexCount == 0 || !batch->vertexBuffer) continue;


                mat->SetIndexCount(batch->indexCount);
                mat->SetBuffer(batch->vertexBuffer, 0);
                mat->SetBuffer(batch->indexBuffer, 1);
                mat->SetMatrix(glm::inverse(camera->GetWorldMatrix()), 0);
                mat->SetMatrix(batch->m_WorldMatrix, 1);
                mat->SetMatrix(cameraComponent->GetProjectionMatrix(), 2);
                mat->SetCameraPosition(camera->GetPosition());
                mat->SetCameraExt(glm::vec4(cameraComponent->GetExtents().x, cameraComponent->GetExtents().y, 0, 0));
                //mat->SetLight(light);
                mat->SetCameraExt(glm::vec4(cameraComponent->GetExtents().x, cameraComponent->GetExtents().y, 0, 0));
                //auto lb = m_LightBuffers[li];




                mat->Bind(false);
                mat->Render();

            }
        }
        // --- CRITICAL FIX: Move this line outside the inner loops ---
        // This ensures that the NEXT light source triggers the additive blending state,
        // not the next render batch.
        firstLightPass = false;
     //   lb->Release();
        li++;
    

    //QEngine::ClearZ();
   // QEngine::SetScissor(0, 0, QEngine::GetFrameWidth(), QEngine::GetFrameHeight());
   // bool add = false;

    //for (auto lb : m_LightBuffers) {

      //  QEngine::ClearZ();
       // m_Draw->SetAdditive(add);
       // m_Draw->BeginFrame();
       // m_Draw->Rect(lb->GetTexture2D(), glm::vec2(0, 0), glm::vec2(QEngine::GetFrameWidth(), QEngine::GetFrameHeight()), glm::vec4(1, 1, 1, 1));
       // m_Draw->Flush();
        //add = true;

    //}

}

std::vector<OctreeNode*> Octree::GetNodeVector()
{
    std::vector<OctreeNode*> nodes;
    if (m_Root) {
        CollectAllNodesRecursive(m_Root.get(), nodes);
    }
    return nodes;
}

// NEW: Recursively traverses the tree and adds each node to the vector.
void Octree::CollectAllNodesRecursive(OctreeNode* node, std::vector<OctreeNode*>& nodes)
{
    if (!node) return;

    nodes.push_back(node);

    for (int i = 0; i < 8; ++i)
    {
        if (node->GetChildren()[i] != nullptr)
        {
            CollectAllNodesRecursive(node->GetChildren()[i].get(), nodes);
        }
    }
}


void Octree::Export(const std::string& path) {
    if (!m_Root) {
        std::cout << "Cannot export an empty Octree." << std::endl;
        return;
    }

    std::string index_path = path + ".nodeindex";
    std::string data_path = path + ".nodedata";

    VFile* idf = new VFile(index_path.c_str(), FileMode::Write);
    VFile* df = new VFile(data_path.c_str(), FileMode::Write);

    // Start the recursive export from the root node.
    ExportRecursive(m_Root.get(), idf, df);

    df->Close();
    idf->Close();
    delete df;
    delete idf;

    std::cout << "Export complete." << std::endl;
}


void Octree::ExportRecursive(OctreeNode* node, VFile* idf, VFile* df) {
    if (!node) return;

    // --- Write this node's index entry ---
    idf->WriteInt(node->GetID());
    const Bounds& bounds = node->GetBounds();
    idf->WriteVec3(bounds.min);
    idf->WriteVec3(bounds.max);

    // Write the node's data and record its position and size.
    long startPos = df->GetPosition();
    node->WriteNode(df);
    long endPos = df->GetPosition();

    idf->WriteLong(startPos);
    idf->WriteLong(endPos - startPos); // Write size, which is more robust.

    // Write a bitmask to signify which children exist.
    uint8_t childMask = 0;
    for (int i = 0; i < 8; ++i) {
        if (node->GetChildren()[i]) {
            childMask |= (1 << i);
        }
    }
    idf->WriteByte(childMask);

    // --- Recurse for all existing children ---
    for (int i = 0; i < 8; ++i) {
        if (node->GetChildren()[i]) {
            ExportRecursive(node->GetChildren()[i].get(), idf, df);
        }
    }
}



Octree::Octree(std::string path,GraphNode* camera) {

    m_Camera = camera;
    m_Draw = new Draw2D(camera);

    std::string index_path = path + ".nodeindex";
    m_DataFilePath = path + ".nodedata";

    m_StreamingBoxExtents = glm::vec3(120,160,120);


    if (!VFile::Exists(index_path.c_str())) {
        std::cerr << "Error: Node index file not found: " << index_path << std::endl;
        m_Root = nullptr;
        return;
    }

    VFile* idf = new VFile(index_path.c_str(), FileMode::Read);

    // Check if the file has any data
    if (idf->Length(index_path.c_str()) > 0) {
        // Recursively load the tree structure
        m_Root = LoadRecursive(idf);
    }
    else {
        m_Root = nullptr;
    }

    idf->Close();
    delete idf;

    std::cout << "Octree loaded from path: " << path << std::endl;

}
std::unique_ptr<OctreeNode> Octree::LoadRecursive(VFile* idf) {
    int id = idf->ReadInt();
    Bounds bounds;
    bounds.min = idf->ReadVec3();
    bounds.max = idf->ReadVec3();
    bounds.CalculateDerivedValues();

    long dataOffset = idf->ReadLong();
    long dataSize = idf->ReadLong();

    auto node = std::make_unique<OctreeNode>(bounds, id);
    node->SetRenderReady(false);

    // NEW: Store the data location for later streaming.
    node->m_dataOffset = dataOffset;
    node->m_dataSize = dataSize;

    uint8_t childMask = idf->ReadByte();
    for (int i = 0; i < 8; ++i) {
        if ((childMask >> i) & 1) {
            node->m_Children[i] = LoadRecursive(idf);
        }
    }
    return node;
}
void Octree::LoadAllNodes()
{
    if (!m_Root || m_DataFilePath.empty()) {
        std::cout << "Octree not loaded from file or data path is missing. Cannot load node data." << std::endl;
        return;
    }

    std::cout << "Starting brute-force load of all node data..." << std::endl;
    VFile* df = new VFile(m_DataFilePath.c_str(), FileMode::Read);

    if (!df) { // Simplified check, assumes constructor handles file errors
        std::cerr << "Error: Could not open node data file: " << m_DataFilePath << std::endl;
        return;
    }

    LoadAllNodesRecursive(m_Root.get(), df);

    df->Close();
    delete df;
    std::cout << "All node data loaded." << std::endl;
}

// --- NEW: Recursive helper to load data for a node and its children ---
void Octree::LoadAllNodesRecursive(OctreeNode* node, VFile* dataFile)
{
    if (!node) return;

    dataFile->Seek(node->m_dataOffset);
    int32_t batchCount = dataFile->ReadInt();

    for (int i = 0; i < batchCount; ++i)
    {
        auto batch = std::make_unique<RenderBatchCache>();

        const char* materialPath_cstr = dataFile->ReadString();

        // NEW: Load materials and matrix from file
        // NOTE: This assumes a static MaterialManager singleton exists.
        if (strlen(materialPath_cstr) > 0) {
            batch->m_Material = new MaterialPBR;
            batch->m_Material->Load(materialPath_cstr);

            // Assuming depth material can be derived or is managed similarly
            batch->m_DepthMaterial = new MaterialDepth;
        }
        delete[] materialPath_cstr;

        batch->m_WorldMatrix = dataFile->ReadMatrix();

        int32_t numVertices = dataFile->ReadInt();
        if (numVertices > 0)
        {
            size_t verticesDataSize = numVertices * sizeof(Vertex3);
            batch->cpuVertexData.resize(numVertices);
            void* vertexData = dataFile->ReadBytes(verticesDataSize);
            memcpy(batch->cpuVertexData.data(), vertexData, verticesDataSize);
            free(vertexData);

            BufferDesc vbDesc;
            vbDesc.Name = "Streamed Node VB";
            vbDesc.Usage = USAGE_IMMUTABLE;
            vbDesc.BindFlags = BIND_VERTEX_BUFFER;
            vbDesc.Size = verticesDataSize;
            BufferData vbInitData;
            vbInitData.pData = batch->cpuVertexData.data();
            vbInitData.DataSize = vbDesc.Size;
            QEngine::m_pDevice->CreateBuffer(vbDesc, &vbInitData, &batch->vertexBuffer);
        }

        int32_t indexCount = dataFile->ReadInt();
        if (indexCount > 0)
        {
            size_t indicesDataSize = indexCount * sizeof(uint32_t);
            batch->cpuIndexData.resize(indexCount);
            void* indexData = dataFile->ReadBytes(indicesDataSize);
            memcpy(batch->cpuIndexData.data(), indexData, indicesDataSize);
            free(indexData);

            BufferDesc ibDesc;
            ibDesc.Name = "Streamed Node IB";
            ibDesc.Usage = USAGE_IMMUTABLE;
            ibDesc.BindFlags = BIND_INDEX_BUFFER;
            ibDesc.Size = indicesDataSize;
            BufferData ibInitData;
            ibInitData.pData = batch->cpuIndexData.data();
            ibInitData.DataSize = ibDesc.Size;
            QEngine::m_pDevice->CreateBuffer(ibDesc, &ibInitData, &batch->indexBuffer);
            batch->indexCount = batch->cpuIndexData.size();
        }

        node->m_RenderBatches.push_back(std::move(batch));
    }

    node->SetRenderReady(true);

    for (int i = 0; i < 8; ++i)
    {
        if (node->GetChildren()[i])
        {
            LoadAllNodesRecursive(node->GetChildren()[i].get(), dataFile);
        }
    }
}


// MODIFIED: Now creates an AABB around the camera for streaming checks.
void Octree::CheckNodes()
{
    if (!m_Root || !m_Camera || m_DataFilePath.empty()) {
        return;
    }

    glm::vec3 camPos = m_Camera->GetPosition();
    Bounds streamingBox;
    streamingBox.min = camPos - m_StreamingBoxExtents;
    streamingBox.max = camPos + m_StreamingBoxExtents;

    CheckNodesRecursive(m_Root.get(), streamingBox);
}


void Octree::CheckNodesRecursive(OctreeNode* node, const Bounds& streamingBox)
{
    if (!node) return;

    if (Intersects(node->GetBounds(), streamingBox))
    {
        if (!node->IsRenderReady() && !node->IsStreaming())
        {
            node->SetStreaming(true);
            std::thread streamer(&Octree::StreamNode, this, node, m_DataFilePath);
            streamer.detach();
        }
    }
    else
    {
        if (node->IsRenderReady())
        {
            UnstreamNode(node);
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        if (node->GetChildren()[i])
        {
            CheckNodesRecursive(node->GetChildren()[i].get(), streamingBox);
        }
    }
}
void Octree::StreamNode(OctreeNode* node, std::string dataFilePath)
{
    if (!node) {
        node->SetStreaming(false);
        return;
    }

    VFile dataFile(dataFilePath.c_str(), FileMode::Read);

    dataFile.Seek(node->m_dataOffset);
    int32_t batchCount = dataFile.ReadInt();

    std::vector<std::unique_ptr<RenderBatchCache>> loadedBatches;

    for (int i = 0; i < batchCount; ++i)
    {
        auto batch = std::make_unique<RenderBatchCache>();

        const char* materialPath_cstr = dataFile.ReadString();
        if (strlen(materialPath_cstr) > 0) {
            batch->m_Material = new MaterialPBR;
            batch->m_Material->Load(materialPath_cstr);
            batch->m_DepthMaterial = new MaterialDepth;// MaterialManager::GetDepth(materialPath_cstr);
        }
        delete[] materialPath_cstr;

        batch->m_WorldMatrix = dataFile.ReadMatrix();

        int32_t numVertices = dataFile.ReadInt();
        if (numVertices > 0)
        {
            size_t verticesDataSize = numVertices * sizeof(Vertex3);
            batch->cpuVertexData.resize(numVertices);
            void* vertexData = dataFile.ReadBytes(verticesDataSize);
            memcpy(batch->cpuVertexData.data(), vertexData, verticesDataSize);
            free(vertexData);

            // As noted, creating GPU resources here is not thread-safe for most graphics APIs.
            // This should ideally be moved to a queue processed by the main render thread.
            BufferDesc vbDesc;
            vbDesc.Name = "Streamed Node VB";
            vbDesc.Usage = USAGE_IMMUTABLE;
            vbDesc.BindFlags = BIND_VERTEX_BUFFER;
            vbDesc.Size = verticesDataSize;
            BufferData vbInitData;
            vbInitData.pData = batch->cpuVertexData.data();
            vbInitData.DataSize = vbDesc.Size;
            QEngine::m_pDevice->CreateBuffer(vbDesc, &vbInitData, &batch->vertexBuffer);
        }

        int32_t indexCount = dataFile.ReadInt();
        if (indexCount > 0)
        {
            size_t indicesDataSize = indexCount * sizeof(uint32_t);
            batch->cpuIndexData.resize(indexCount);
            void* indexData = dataFile.ReadBytes(indicesDataSize);
            memcpy(batch->cpuIndexData.data(), indexData, indicesDataSize);
            free(indexData);

            BufferDesc ibDesc;
            ibDesc.Name = "Streamed Node IB";
            ibDesc.Usage = USAGE_IMMUTABLE;
            ibDesc.BindFlags = BIND_INDEX_BUFFER;
            ibDesc.Size = indicesDataSize;
            BufferData ibInitData;
            ibInitData.pData = batch->cpuIndexData.data();
            ibInitData.DataSize = ibDesc.Size;
            QEngine::m_pDevice->CreateBuffer(ibDesc, &ibInitData, &batch->indexBuffer);
            batch->indexCount = batch->cpuIndexData.size();
        }

        loadedBatches.push_back(std::move(batch));
    }

    dataFile.Close();

    node->m_RenderBatches = std::move(loadedBatches);
    node->SetRenderReady(true);
    node->SetStreaming(false);
}

void Octree::UnstreamNode(OctreeNode* node)
{
    if (!node || node->IsStreaming()) return;

    node->m_RenderBatches.clear();
    node->SetRenderReady(false);
}
