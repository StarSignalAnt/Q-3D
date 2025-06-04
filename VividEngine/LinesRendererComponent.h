#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <vector>
#include "MaterialLines.h"
#include "RenderMaterial.h"
struct LineVertex {
    glm::vec3 position;
    glm::vec4 color;
};

struct Line {
    unsigned int v0;
    unsigned int v1;
};

class LinesRendererComponent :
    public Component
{
public:

    LinesRendererComponent();

    void AddVertex(LineVertex v);
    void AddLine(Line line);
    void Finalize();
    void OnRender(GraphNode* camera) override;
private:

    RefCntAutoPtr<IBuffer> VertexBuffer;
    RefCntAutoPtr<IBuffer> IndexBuffer;

    std::vector<LineVertex> m_Vertices;
    std::vector<Line> m_Lines;

    MaterialLines* m_Material;

};



