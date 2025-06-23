#include "NodeRenderVideo.h"
#include <iostream>
#include "GraphNode.h"
#include "QEngine.h"

NodeRenderVideo::NodeRenderVideo() {
    // Set user-facing name and internal type name for saving
    SetName("Render Video");
    SetTypeName("Render Video");
    AddExecOutput("Then");

    // Use the cleaner API to define the input port
    //AddInput("Node", DataType::GraphNodeRef);
   // AddInput("Delta", DataType::Vec3);


    // Keeping this commented out as per your file
    // AddOutput("Result", OutputType::Om_FLOAT);
}

void NodeRenderVideo::Exec() {
    // Execution logic will go here.
    //std::cout << GetName() << " is executing its logic." << std::endl;


    // --- FINAL USAGE ---
    // Get the value from the "Delta" input pin, specifying you expect a glm::vec3.
   // auto deltaOpt = GetInputValue<glm::vec3>("Delta");
   // auto gr = GetInputValue<GraphNode*>("Node").value();

    //if (deltaOpt) {
        // The optional has a value, so we can use it.
      //  glm::vec3 delta = *deltaOpt;
       // std::cout << "  > Turning by: " << delta.x << ", " << delta.y << ", " << delta.z << std::endl;
        //gr->Turn(delta, false);
        // ... apply rotation logic here ...
    //}
    //else {
        // This will happen if the "Delta" pin is not connected and has no
        // default value, or if its connected value is not a vec3.
    //    std::cout << "  > Could not get valid vec3 for 'Delta' input." << std::endl;
   // }
    // --- END FINAL USAGE ---

    QEngine::RenderVideo();
    ExecNext();
}