#pragma once
#include "Component.h"
#include "LGraph.h"

class CameraComponent;
class GraphNode;

class LogicGraphComponent :
    public Component
{
public:

    void SetGraph(LGraph* graph);
    void OnUpdate(float up) override;
    void OnPlay() override;
    void OnStop() override;
    void OnRender(GraphNode* camera) override;
    std::string GetName() { return m_Name; }
    std::vector<LGraphVariable*> GetVars();

private:

    std::string m_Name = "";
    LGraph* m_Graph;

};

