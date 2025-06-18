#include "NodeExposeGraphNode.h"
#include "GraphNode.h"

NodeExposeGraphNode::NodeExposeGraphNode() {

    SetName("Expose Node");
    SetTypeName("Expose Node");
    AddInput("Node", DataType::GraphNodeRef);
    
    AddOutput("Name", DataType::String);
    AddOutput("Position", DataType::Vec3);
    AddOutput("Scale", DataType::Vec3);




}

void NodeExposeGraphNode::Exec()  {

    auto gr = GetInputValue<GraphNode*>("Node").value();

    if (gr != nullptr)
    {

    }
    // Get the values from the input pins. Default to a zero-equivalent if not connected.
    //T valA = GetInputValue<T>("A").value_or(T{});
   // T valB = GetInputValue<T>("B").value_or(T{});

    // The '+' operator works automatically for int, float, and glm vectors.
   // T result = valA + valB;

    // Set the result on our output pin.
    m_Outputs[0]->SetValue(gr->GetName());
    m_Outputs[1]->SetValue(gr->GetPosition());
    m_Outputs[2]->SetValue(gr->GetScale());



}