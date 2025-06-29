#include "pch.h"
#include "Draw2D.h"
#include "Texture2D.h"
#include "Material2D.h"
#include "CameraComponent.h"
#include "QEngine.h" // Assumed to provide Graphics Engine device and context

// Constructor: Creates long-lived GPU resources.
Draw2D::Draw2D(GraphNode* node)
    : m_Camera(node), m_Override(nullptr)
{
    m_Material = new Material2D;
    m_BatchVertices.reserve(MAX_QUADS_PER_BATCH * 4);

    // Create a single dynamic Vertex Buffer that will be reused every frame.
    BufferDesc VertBuffDesc;
    VertBuffDesc.Name = "Draw2D_Dynamic_VB";
    VertBuffDesc.Usage = USAGE_DYNAMIC;
    VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
    VertBuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
    VertBuffDesc.Size = MAX_QUADS_PER_BATCH * 4 * sizeof(Draw2D_Vertex);
    QEngine::m_pDevice->CreateBuffer(VertBuffDesc, nullptr, &m_VertexBuffer);

    // Create a single immutable Index Buffer, as the pattern for drawing quads is constant.
    std::vector<uint32_t> indices(MAX_QUADS_PER_BATCH * 6);
    for (uint32_t i = 0; i < MAX_QUADS_PER_BATCH; ++i)
    {
        uint32_t offset = i * 4;
        indices[i * 6 + 0] = offset + 0;
        indices[i * 6 + 1] = offset + 1;
        indices[i * 6 + 2] = offset + 2;
        indices[i * 6 + 3] = offset + 2;
        indices[i * 6 + 4] = offset + 3;
        indices[i * 6 + 5] = offset + 0;
    }

    BufferDesc IdxBuffDesc;
    IdxBuffDesc.Name = "Draw2D_Immutable_IB";
    IdxBuffDesc.Usage = USAGE_IMMUTABLE;
    IdxBuffDesc.BindFlags = BIND_INDEX_BUFFER;
    IdxBuffDesc.Size = (Uint64)indices.size() * sizeof(uint32_t);

    BufferData IBData;
    IBData.pData = indices.data();
    IBData.DataSize = IdxBuffDesc.Size;
    QEngine::m_pDevice->CreateBuffer(IdxBuffDesc, &IBData, &m_IndexBuffer);
}

// Destructor: Cleans up allocated memory. RefCntAutoPtr handles GPU resources.
Draw2D::~Draw2D()
{
    delete m_Material;
}

// Resets the batching system for a new frame.
void Draw2D::BeginFrame()
{
    m_BatchVertices.clear();
    m_ActiveMaterial = nullptr;
    m_BatchTexture = nullptr;
}

// Sets the default material, flushing any pending work.
void Draw2D::SetMaterial(Material2D* material)
{
    if (m_Material == material) return;
    InternalFlush();
    delete m_Material;
    m_Material = material;
}

// Sets an override material, flushing any pending work.
void Draw2D::SetOverride(RenderMaterial* mat)
{
    if (m_Override == mat) return;
    InternalFlush();
    m_Override = mat;
}

// Adds a rectangle to the current batch, flushing automatically if state changes.
void Draw2D::Rect(Texture2D* image, glm::vec2 pos, glm::vec2 size, glm::vec4 color)
{
    RenderMaterial* materialToUse = m_Override != nullptr ? m_Override : (RenderMaterial*)m_Material;

    // A batch must be flushed if the material or texture is different, or if the current batch is full.
    if ((m_ActiveMaterial != nullptr && m_ActiveMaterial != materialToUse) ||
        (m_BatchTexture != nullptr && m_BatchTexture != image) ||
        (m_BatchVertices.size() / 4 >= MAX_QUADS_PER_BATCH))
    {
        InternalFlush();
    }

    // If the batch is now empty (either initially, or after a flush), set its state.
    if (m_BatchVertices.empty())
    {
        m_ActiveMaterial = materialToUse;
        m_BatchTexture = image;
    }

    // Define the four vertices of the rectangle.
    Draw2D_Vertex v1, v2, v3, v4;
    float z = 0.1f;

    v1.position = glm::vec3(pos.x, pos.y, z);
    v2.position = glm::vec3(pos.x + size.x, pos.y, z);
    v3.position = glm::vec3(pos.x + size.x, pos.y + size.y, z);
    v4.position = glm::vec3(pos.x, pos.y + size.y, z);

    v1.color = v2.color = v3.color = v4.color = color;

    v1.uv = glm::vec3(0, 0, 0);
    v2.uv = glm::vec3(1, 0, 0);
    v3.uv = glm::vec3(1, 1, 0);
    v4.uv = glm::vec3(0, 1, 0);

    v1.view = glm::vec4(QEngine::ScX, QEngine::ScY, QEngine::ScW, QEngine::ScH);
    v2.view = v1.view;
    v3.view = v1.view;
    v4.view = v1.view;

    glm::vec3 zeroVec(0.0f);
   // v1.normal = v2.normal = v3.normal = v4.normal = zeroVec;
   // v1.binormal = v2.binormal = v3.binormal = v4.binormal = zeroVec;
    //v1.tangent = v2.tangent = v3.tangent = v4.tangent = zeroVec;


    m_BatchVertices.push_back(v1);
    m_BatchVertices.push_back(v2);
    m_BatchVertices.push_back(v3);
    m_BatchVertices.push_back(v4);
}

// The user-callable Flush, to render the final batch at the end of a frame.
void Draw2D::Flush()
{
    InternalFlush();
}

// Submits the currently buffered geometry to the GPU.
void Draw2D::InternalFlush()
{
    if (m_BatchVertices.empty() || m_ActiveMaterial == nullptr)
    {
        return;
    }

    // Map the dynamic vertex buffer, copy our CPU-side vertex data to it, and unmap.
    void* pMappedData;
    QEngine::m_pImmediateContext->MapBuffer(m_VertexBuffer, MAP_WRITE, MAP_FLAG_DISCARD, pMappedData);
    memcpy(pMappedData, m_BatchVertices.data(), m_BatchVertices.size() * sizeof(Draw2D_Vertex));
    QEngine::m_pImmediateContext->UnmapBuffer(m_VertexBuffer, MAP_WRITE);

    // Set up material properties for the entire batch.
    uint32_t indexCount = (uint32_t)(m_BatchVertices.size() / 4) * 6;

    m_ActiveMaterial->SetIndexCount(indexCount);
    m_ActiveMaterial->SetBuffer(m_VertexBuffer, 0);
    m_ActiveMaterial->SetBuffer(m_IndexBuffer, 1);
    m_ActiveMaterial->SetMatrix(glm::mat4(1.0f), 0);
    m_ActiveMaterial->SetMatrix(glm::mat4(1.0f), 1);
    m_ActiveMaterial->SetMatrix(m_Camera->GetComponent<CameraComponent>()->Get2DProjectionMatrix(), 2);
    m_ActiveMaterial->SetCameraPosition(m_Camera->GetPosition());
    m_ActiveMaterial->SetCameraExt(m_Camera->GetComponent<CameraComponent>()->GetExtents());
    m_ActiveMaterial->SetTexture(m_BatchTexture, 0);

    // Bind the material and render the entire batch.
    m_ActiveMaterial->Bind(m_Add);
    m_ActiveMaterial->Render();

    // Clear the CPU-side vertex buffer to start the next batch.
    m_BatchVertices.clear();
}

void Draw2D::SetAdditive(bool add) {

    m_Add = add;
}