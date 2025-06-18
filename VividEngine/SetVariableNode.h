#pragma once
#include "LGLogicNode.h" // Inherit from the Logic Node base class
#include <string>

class LGraph; // Forward-declare

class SetVariableNode : public LGLogicNode
{
public:
    // The constructor needs to know which graph it belongs to and which variable to set.
    SetVariableNode(LGraph* graph, const std::string& varName);

    std::string GetCategory() const override { return "Variables"; }

    // This is a logic node, so its main logic is in the Exec() method.
    void Exec() override;

private:
    LGraph* m_graph;
    std::string m_variableName;
};