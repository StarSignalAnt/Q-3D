#include "GetVariableNode.h"
#include "LGraph.h"
#include "LGraphVariable.h"

// Default constructor creates a "blank" node
GetVariableNode::GetVariableNode() : m_graph(nullptr), m_variableName("")
{
    // We can't set the real name or pins until we know the variable
    SetName("Get Variable");
    SetTypeName("GetVariableNode"); // Use a generic typeName for registration
    m_hasExecIn = false;
    m_hasExecOut = false;
}

// Initialize sets up the node with its specific data
void GetVariableNode::Initialize(LGraph* graph, const std::string& varName)
{
    m_graph = graph;
    m_variableName = varName;
    SetName("Get " + varName); // Now we can set the real name

    // Add the specific output pin based on the variable's type
    m_Outputs.clear(); // Clear any previous (placeholder) pins
    if (m_graph) {
        LGraphVariable* var = m_graph->FindVariable(m_variableName);
        if (var) {
            AddOutput(var->GetName(), var->GetType());
        }
    }
}

// FromJson reads the variable name and then calls Initialize
void GetVariableNode::FromJson(const json& j, LGraph* graph)
{
    LNode::FromJson(j, graph); // Call the base class version first
    std::string varName = j.at("variableName");
    Initialize(graph, varName);
}

// ToJson saves the variable name
void GetVariableNode::ToJson(json& j)
{
    LNode::ToJson(j); // Call the base class version first
    j["variableName"] = m_variableName;
}

void GetVariableNode::CalculateOutputs()
{
    if (!m_graph || m_Outputs.empty()) return;
    LGraphVariable* var = m_graph->FindVariable(m_variableName);
    if (var) {
        m_Outputs[0]->SetValue(var->GetDefaultValue());
    }
}