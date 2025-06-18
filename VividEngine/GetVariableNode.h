#pragma once
#include "LNode.h"
#include <string>

class LGraph;

class GetVariableNode : public LNode
{
public:
    // A default constructor for the NodeRegistry
    GetVariableNode();

    // An Initialize method to set up the node after creation
    void Initialize(LGraph* graph, const std::string& varName);

    std::string GetCategory() const override { return "Variables"; }

    void CalculateOutputs() override;

    // Override serialization to save/load the variable name
    void ToJson(json& j) override;
    void FromJson(const json& j, LGraph* graph) override;

private:
    LGraph* m_graph;
    std::string m_variableName;
};