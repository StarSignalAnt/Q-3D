#include "NodeSetRotationNode.h"
#include "GraphNode.h"

NodeSetRotationNode::NodeSetRotationNode() {
    // Set user-facing name and internal type name for saving
    SetName("Set Rotation GraphNode");
    SetTypeName("Set Rotation GraphNode");

    // Use the cleaner API to define the input port
    AddInput("Node", DataType::GraphNodeRef);
    AddInput("Rotation", DataType::Mat4);


    // Keeping this commented out as per your file
    // AddOutput("Result", OutputType::Om_FLOAT);
}

void NodeSetRotationNode::Exec() {
    // Execution logic will go here.
    //std::cout << GetName() << " is executing its logic." << std::endl;


    // --- FINAL USAGE ---
    // Get the value from the "Delta" input pin, specifying you expect a glm::vec3.
    auto rotOpt = GetInputValue<glm::mat4>("Rotation");
    auto gr = GetInputValue<GraphNode*>("Node").value();

    if (rotOpt) {
        // The optional has a value, so we can use it.
        glm::mat4 rot = *rotOpt;
        gr->SetRotation(rot);
        // ... apply rotation logic here ...
    }
    else {
        // This will happen if the "Delta" pin is not connected and has no
        // default value, or if its connected value is not a vec3.
//        std::cout << "  > Could not get valid vec3 for 'Delta' input." << std::endl;
    }
    // --- END FINAL USAGE ---

    ExecNext();
}