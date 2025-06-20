#include "NodeDebug.h"
#include "Vivid.h"

NodeDebug::NodeDebug() {
    // Set user-facing name and internal type name for saving
    SetName("Debug Message");
    SetTypeName("Debug Message");

    // Use the cleaner API to define the input port
    AddInput("Message", DataType::String);
    //AddInput("Delta", DataType::Vec3);



    // Keeping this commented out as per your file
    // AddOutput("Result", OutputType::Om_FLOAT);
}

void NodeDebug::Exec() {
    // Execution logic will go here.
    //std::cout << GetName() << " is executing its logic." << std::endl;


    // --- FINAL USAGE ---
    // Get the value from the "Delta" input pin, specifying you expect a glm::vec3.
    //auto deltaOpt = GetInputValue<glm::vec3>("Delta");
    //auto gr = GetInputValue<GraphNode*>("Node").value();


    auto msg = GetInputValue<std::string>("Message").value();

    Vivid::DebugLog(msg);
    std::cout << msg << std::endl;
    
    ExecNext();
}