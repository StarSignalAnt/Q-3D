#include "NodeStartVideo.h"
#include <iostream>
#include "GraphNode.h"
#include "QEngine.h"

NodeStartVideo::NodeStartVideo() {
    // Set user-facing name and internal type name for saving
    SetName("Start Video");
    SetTypeName("Start Video");
    
    AddExecOutput("Then");

    // Use the cleaner API to define the input port
    AddInput("Path", DataType::String);
    //AddInput("Delta", DataType::Vec3);


    // Keeping this commented out as per your file
    // AddOutput("Result", OutputType::Om_FLOAT);
}

void NodeStartVideo::Exec() {
    // Execution logic will go here.
    std::cout << GetName() << " is executing its logic." << std::endl;

    // --- FINAL USAGE ---
    // Get the value from the "Delta" input pin, specifying you expect a glm::vec3.
    auto path = GetInputValue<std::string>("Path").value();

    Q3D::Engine::QEngine::StartVideo(path);


    ExecNext();
}