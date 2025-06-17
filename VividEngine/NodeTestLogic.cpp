#include "NodeTestLogic.h"

NodeTestLogic::NodeTestLogic() {
    // Set user-facing name and internal type name for saving
    SetName("Test Logic Node");
    SetTypeName("Test Logic Node");

    // Use the cleaner API to define ports
    AddInput("Name", DataType::String);
    AddInput("Value", DataType::Float);
    AddOutput("Result",DataType::Float);

}

void NodeTestLogic::Exec() {
    // Execution logic will go here.
}