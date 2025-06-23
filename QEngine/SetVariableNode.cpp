#include "SetVariableNode.h"
#include "LGraph.h"
#include "LGraphVariable.h"
#include <iostream>

SetVariableNode::SetVariableNode(LGraph* graph, const std::string& varName)
    : m_graph(graph), m_variableName(varName)
{
    // The constructor logic is correct and remains unchanged.
    SetName("Set " + varName);
    SetTypeName("SetVariable_" + varName);
    AddExecOutput("Then");

    if (m_graph) {
        LGraphVariable* var = m_graph->FindVariable(m_variableName);
        if (var) {
            AddInput(var->GetName(), var->GetType());
        }
    }
}

// --- This is the new, corrected Exec() method ---
void SetVariableNode::Exec()
{
    // Ensure the graph and input pins exist.
    if (m_graph && !m_Inputs.empty()) {

        // 1. Find the graph variable this node is supposed to set.
        LGraphVariable* var = m_graph->FindVariable(m_variableName);

        // 2. Get this node's own input pin (there is only one).
        LGInput* inputPin = m_Inputs[0];

        // 3. Check if the variable exists and our input pin is connected to something.
        if (var && inputPin->isConnected()) {

            // 4. Get the output pin that our input is connected to.
            LGOutput* sourceOutput = inputPin->GetConnection();
            LNode* sourceNode = sourceOutput->getParentNode();

            // 5. IMPORTANT: Tell the source node to calculate its value to ensure it's fresh.
            if (sourceNode) {
                sourceNode->CalculateOutputs();
            }

            // 6. Get the entire variant value from the source and set the graph variable's value.
            const LGInput::ValueVariant& newValue = sourceOutput->GetValue();
            var->SetDefaultValue(newValue);

            std::cout << "Set variable '" << m_variableName << "'." << std::endl;
        }
    }

    // 7. Continue the execution chain.
    ExecNext(0);
}