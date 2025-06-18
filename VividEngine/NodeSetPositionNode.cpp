#include "NodeSetPositionNode.h"
#include "GraphNode.h"

NodeSetPositionNode::NodeSetPositionNode() {
    // Set user-facing name and internal type name for saving
    SetName("Set Position GraphNode");
    SetTypeName("Set Position GraphNode");

    // Use the cleaner API to define the input port
    AddInput("Node", DataType::GraphNodeRef);
    AddInput("Position", DataType::Vec3);


    // Keeping this commented out as per your file
    // AddOutput("Result", OutputType::Om_FLOAT);
}

void NodeSetPositionNode::Exec() {
    // Execution logic will go here.
    //std::cout << GetName() << " is executing its logic." << std::endl;


    // --- FINAL USAGE ---
    // Get the value from the "Delta" input pin, specifying you expect a glm::vec3.
    auto posOpt = GetInputValue<glm::vec3>("Position");
    auto gr = GetInputValue<GraphNode*>("Node").value();

    if (posOpt) {
        // The optional has a value, so we can use it.
        glm::vec3 pos = *posOpt;
      //  std::cout << "  > Turning by: " << delta.x << ", " << delta.y << ", " << delta.z << std::endl;
        gr->SetPosition(pos);
        // ... apply rotation logic here ...
    }
    else {
        // This will happen if the "Delta" pin is not connected and has no
        // default value, or if its connected value is not a vec3.
      //  std::cout << "  > Could not get valid vec3 for 'Delta' input." << std::endl;
    }
    // --- END FINAL USAGE ---

    ExecNext();
}