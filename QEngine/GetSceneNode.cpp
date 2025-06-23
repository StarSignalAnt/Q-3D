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

  

        // The INPUT pin takes a string. This will automatically create a QLineEdit in the UI.
        AddInput("Node Name", DataType::String);

        // The OUTPUT pin provides a reference to the found GraphNode.
        AddOutput("NodeRef", DataType::GraphNodeRef);
    }

    void GetSceneNode::CalculateOutputs() {
        if (m_Outputs.empty()) {
            return;
        }

        // Get the desired node name from our input pin.
        // We request it as a <string> because that's its data type.
        auto nodeNameOpt = GetInputValue<std::string>("Node Name");

        if (nodeNameOpt) {
            // The job of this node is to simply pass the name string along.
            // We set our output pin's value to the string we got from the input.
            // The downstream node (like NodeTurnNode) will be the one to call
            // GetInputValue<GraphNode*>(), which will trigger our new lookup logic.
            m_Outputs[0]->SetValue(*nodeNameOpt);
        }
        else {
            // If the input is invalid, set the output to an empty string to avoid errors.
            m_Outputs[0]->SetValue(std::string(""));
        }
    }