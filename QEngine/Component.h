#pragma once
#include "VFile.h"
#include "Properties.h"
class GraphNode;

class Component
{
public:
    Component() {

        GraphNode* n = nullptr;
        m_Properties.bind("Owner",n);
        m_Properties.bind("ComponentName", &m_Name);

    }
    virtual ~Component() = default;
    Properties& GetProperties() {
        return m_Properties;
    }


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
    virtual void Write(VFile* f) {};
    virtual void Read(VFile* f) {};
    std::string GetName() {
        return m_Name;
    }
    virtual Component* CreateInstance()
    {
        return nullptr;
    }
    std::string GetCategory() {
        return m_Category;
    }
    virtual void Initialize() {};

protected:

    // Pointer to the owner node
    GraphNode* m_Owner = nullptr;
    bool m_Playing = false;
    std::string m_Name = "Component";
    std::string m_Category = "General";

    Properties m_Properties;
};
