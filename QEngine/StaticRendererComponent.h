#pragma once
#include "Component.h"
class StaticRendererComponent :
    public Component
{
public:
    StaticRendererComponent() {
        m_Name = "Static Renderer Component";
        m_Properties.bind("Use Lod", &m_UseLod);
        m_Category = "Rendering";
    }
    Component* CreateInstance() {
        return new StaticRendererComponent;
    }
    void OnRender(GraphNode* camera) override;
    void OnRenderDirect(GraphNode* light, GraphNode* cam);
private:

    int m_UseLod = 0;

};


