#include "GetSceneNode.h"
#include <iostream>
#include "SceneGraph.h"


    GraphNode* FindNodeByName(const std::string& name) {
        // Replace this with your actual engine code to find a 3D node by name.
        // For this example, we'll just print a message.
        auto n = SceneGraph::m_Instance->FindNode(name);
        std::cout << "Engine is searching for GraphNode named: " << name << std::endl;
        // Return nullptr for now, but in your engine this would return the real pointer.
        return n;
    }


    GetSceneNode::GetSceneNode() {
        SetName("Get Scene Node");
        SetTypeName("Get Scene Node");

        // This node does not participate in the execution chain.
        m_hasExecIn = false;
        m_hasExecOut = false;

        // The INPUT pin takes a string. This will automatically create a QLineEdit in the UI.
        AddInput("Node Name", DataType::String);

        // The OUTPUT pin provides a reference to the found GraphNode.
        AddOutput("Node", DataType::GraphNodeRef);
    }

    void GetSceneNode::CalculateOutputs() {
        // This is called automatically when another node requests our output.

        // 1. Get the string name from our input pin's default value.
        auto nodeNameOpt = GetInputValue<std::string>("Node Name");

        GraphNode* foundNode = nullptr;
        if (nodeNameOpt && !nodeNameOpt->empty()) {
            // 2. Use the engine's API to find the actual GraphNode*.
            foundNode = FindNodeByName(*nodeNameOpt);
        }

        // 3. Set the output pin's value to the found pointer (or nullptr if not found).
        m_Outputs[0]->SetValue(foundNode);
    }