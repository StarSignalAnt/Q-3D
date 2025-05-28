#pragma once

class GraphNode;

class Component
{
public:
    Component() = default;
    virtual ~Component() = default;



    // Called when the component is attached to a node
    virtual void OnAttach(GraphNode* node) {
        m_Owner = node;
    }

    // Optional virtual methods to override
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender(GraphNode* camera) {}
protected:

    // Pointer to the owner node
    GraphNode* m_Owner = nullptr;

};
