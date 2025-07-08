#pragma once
#include "Component.h"
class StaticDepthRendererComponent :
    public Component
{
public:
    StaticDepthRendererComponent() {
        m_Name = "Static Depth Renderer Component";
        m_Properties.bind("Use Lod", &m_UseLod);
    }

    void OnRenderDepth(GraphNode* camera) override;
    //void OnRenderDirect(GraphNode* camera)

private:

    int m_UseLod = 0;

};

