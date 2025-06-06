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
    GraphNode* GetOwner() {
        return m_Owner;
    }
    virtual void Push() {};
    virtual void Pop() {};
    // Optional virtual methods to override
    virtual void OnPlay() {};
    virtual void OnStop() {};
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender(GraphNode* camera) {}
    virtual void OnRenderDepth(GraphNode* camera) {};
protected:

    // Pointer to the owner node
    GraphNode* m_Owner = nullptr;

};
