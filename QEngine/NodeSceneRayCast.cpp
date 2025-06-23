#include "NodeSceneRayCast.h"
#include "SceneGraph.h"
#include "GraphNode.h"

NodeSceneRayCast::NodeSceneRayCast() {
    // Set user-facing name and internal type name for saving
    SetName("Scene Raycast");
    SetTypeName("Scene Raycast");
    //m_hasExecOut = true;
    AddExecOutput("Then");
    // Use the cleaner API to define the input port
    //AddInput("Node", DataType::GraphNodeRef);
    //AddInput("Delta", DataType::Vec3);
    AddInput("From", DataType::Vec3);
    AddInput("To", DataType::Vec3);

    AddOutput("Hit", DataType::Bool);
    AddOutput("Point", DataType::Vec3);
    AddOutput("Distance", DataType::Float);
    AddOutput("Node", DataType::GraphNodeRef);

    // Keeping this commented out as per your file
    // AddOutput("Result", OutputType::Om_FLOAT);
}

void NodeSceneRayCast::Exec() {
    // We do NOT do the raycast here.
    ExecNext(0); // Fire the "Then" pin.
}

// CalculateOutputs' ONLY job is to calculate and set the output values.
void NodeSceneRayCast::CalculateOutputs() {
    auto from = GetInputValue<glm::vec3>("From").value_or(glm::vec3(0.0f));
    auto to = GetInputValue<glm::vec3>("To").value_or(glm::vec3(0.0f));

    auto r = SceneGraph::m_Instance->RayCast(from, to);

    m_Outputs[0]->SetValue(r.m_Hit);
    m_Outputs[1]->SetValue(r.m_Point);
    m_Outputs[2]->SetValue(r.m_Distance);
    m_Outputs[3]->SetValue(r.m_Node);

    // We do NOT call ExecNext() here.
}