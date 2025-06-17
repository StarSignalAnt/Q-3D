#pragma once
#include "Component.h"
#include "LGraph.h"
class LogicGraphComponent :
    public Component
{
public:

    void SetGraph(LGraph* graph);
    void OnUpdate(float up) override;
    std::string GetName() { return m_Name; }

private:

    std::string m_Name = "";
    LGraph* m_Graph;

};

