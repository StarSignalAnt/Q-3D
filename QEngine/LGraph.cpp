#include "LGraph.h"
#include "LNode.h"
#include "LEventNode.h"
#include "NodeRegistry.h"
#include <fstream>
#include <map>
#include <iostream>
#include "json.hpp" // Assumes nlohmann/json.hpp is in your project

using json = nlohmann::json;

LGraph::LGraph(std::string name) : m_name(name) {}

LGraph::~LGraph() {
    for (LNode* node : m_nodes) {
        delete node;
    }
}

void LGraph::AddNode(LNode* node) {
    if (!node) return;
    m_nodes.push_back(node);
    if (node->GetCategory() == "Events") {
        if (auto* eventNode = static_cast<LEventNode*>(node)) {
            m_eventNodes.push_back(eventNode);
        }
    }
}

const std::vector<LNode*>& LGraph::GetNodes() const {
    return m_nodes;
}

void LGraph::FireEvent(const std::string& eventName) {
    for (LEventNode* eventNode : m_eventNodes) {
        if (eventNode->GetName() == eventName) {
            eventNode->FireEvent();
        }
    }
}

void LGraph::SaveToFile(const std::string& filepath) {
    json root_json;
    root_json["graph_name"] = m_name;

    json nodes_json = json::array();
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        m_nodes[i]->SetID(i);
        json node_json;
        m_nodes[i]->ToJson(node_json);
        nodes_json.push_back(node_json);
    }
    root_json["nodes"] = nodes_json;

    json connections_json = json::array();
    for (const auto& node : m_nodes) {
        for (size_t i = 0; i < node->GetExecOutputs().size(); ++i) {
            const auto& execPin = node->GetExecOutputs()[i];
            if (execPin->nextNode) {
                json conn_json;
                conn_json["type"] = "exec";
                conn_json["from_node"] = node->GetID();
                conn_json["from_port"] = i; // Save the output pin index
                conn_json["to_node"] = execPin->nextNode->GetID();
                connections_json.push_back(conn_json);
            }
        }
        for (const auto& input : node->GetInputs()) {
            if (input->isConnected()) {
                LGOutput* outputPort = input->GetConnection();
                LNode* outputNode = outputPort->getParentNode();
                json conn_json;
                conn_json["type"] = "data";
                conn_json["from_node"] = outputNode->GetID();
                conn_json["from_port"] = outputPort->getPortIndex();
                conn_json["to_node"] = node->GetID();
                conn_json["to_port"] = input->getPortIndex();
                connections_json.push_back(conn_json);
            }
        }
    }
    root_json["connections"] = connections_json;

    json variables_json = json::array();
    for (const auto& var : m_variables) {
        json var_json;
        var->ToJson(var_json);
        variables_json.push_back(var_json);
    }
    root_json["variables"] = variables_json;


    std::ofstream stream(filepath);
    stream << root_json.dump(4);
    stream.close();
}
LGraph* LGraph::LoadFromFile(const std::string& filepath, NodeRegistry& registry) {
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        std::cerr << "ERROR: Could not open file for reading: " << filepath << std::endl;
        return nullptr;
    }

    json root_json;
    try {
        stream >> root_json;
    }
    catch (json::parse_error& e) {
        std::cerr << "ERROR: Failed to parse JSON file: " << e.what() << std::endl;
        return nullptr;
    }

    LGraph* graph = new LGraph(root_json.value("graph_name", "Loaded Graph"));
    std::map<int, LNode*> idToNodeMap;

    // --- BEGIN FIX: The loading order is now correct ---

    // 1. Load VARIABLES first.
    // This ensures the list of variables exists before any nodes try to reference them.
    if (root_json.contains("variables")) {
        for (const auto& var_json : root_json["variables"]) {
            DataType type = static_cast<DataType>(var_json.at("type").get<int>());
            std::string name = var_json.at("name");
            auto* newVar = new LGraphVariable(name, type);
            newVar->FromJson(var_json);
            graph->AddVariable(newVar);
        }
    }

    // 2. Load NODES second.
    // Now, when a GetVariableNode is created, it can successfully find its variable in the graph.
    for (const auto& node_json : root_json["nodes"]) {
        std::string typeName = node_json.at("typeName");
        // Use the registry to create a blank node
        LNode* newNode = registry.CreateNode(typeName);
        if (newNode) {
            // Initialize the node with its data from the file. This is where
            // GetVariableNode will create its pins.
            newNode->FromJson(node_json, graph);
            graph->AddNode(newNode);
            idToNodeMap[newNode->GetID()] = newNode;
        }
    }

    // 3. Load CONNECTIONS last.
    // This must be last, as it requires all nodes and their pins to be fully created.
    for (const auto& conn_json : root_json["connections"]) {
        std::string type = conn_json.at("type");
        if (type == "exec") {
            int from_id = conn_json.at("from_node");
            int from_port = conn_json.at("from_port");
            int to_id = conn_json.at("to_node");
            if (idToNodeMap.count(from_id) && idToNodeMap.count(to_id)) {
                idToNodeMap.at(from_id)->SetNextExec(from_port, idToNodeMap.at(to_id));
            }
        }
        else if (type == "data") {
            int from_node_id = conn_json.at("from_node");
            int from_port_id = conn_json.at("from_port");
            int to_node_id = conn_json.at("to_node");
            int to_port_id = conn_json.at("to_port");
            if (idToNodeMap.count(from_node_id) && idToNodeMap.count(to_node_id)) {
                LNode* fromNode = idToNodeMap.at(from_node_id);
                LNode* toNode = idToNodeMap.at(to_node_id);
                if (to_port_id < toNode->GetInputs().size() && from_port_id < fromNode->GetOutputs().size()) {
                    toNode->GetInputs().at(to_port_id)->setConnection(fromNode->GetOutputs().at(from_port_id));
                    fromNode->GetOutputs().at(from_port_id)->addConnection(toNode->GetInputs().at(to_port_id));
                }
            }
        }
    }

    // --- END FIX ---

    return graph;
}

void LGraph::RemoveNode(LNode* nodeToRemove)
{
    if (!nodeToRemove) return;

    // --- Disconnect all other nodes that point TO this node ---
    for (LNode* node : m_nodes) {
        // Disconnect execution pins
        for (auto& execPin : node->GetExecOutputs()) {
            if (execPin->nextNode == nodeToRemove) {
                execPin->nextNode = nullptr;
            }
        }
        // Disconnect data pins
        for (LGInput* input : node->GetInputs()) {
            if (input->isConnected() && input->GetConnection()->getParentNode() == nodeToRemove) {
                input->setConnection(nullptr);
            }
        }
    }

    // --- Remove the node from the graph's lists ---

    // Erase from the main nodes vector
    m_nodes.erase(std::remove(m_nodes.begin(), m_nodes.end(), nodeToRemove), m_nodes.end());

    // Erase from the event nodes vector if it's an event node
    if (nodeToRemove->GetCategory() == "Events") {
        m_eventNodes.erase(std::remove(m_eventNodes.begin(), m_eventNodes.end(), static_cast<LEventNode*>(nodeToRemove)), m_eventNodes.end());
    }

    // --- Finally, delete the logical node object ---
    delete nodeToRemove;
}

void LGraph::AddVariable(LGraphVariable* var) {
    if (var) m_variables.push_back(var);
}

void LGraph::RemoveVariable(const std::string& name) {
    m_variables.erase(std::remove_if(m_variables.begin(), m_variables.end(),
        [&](LGraphVariable* var) {
            if (var->GetName() == name) {
                delete var;
                return true;
            }
            return false;
        }), m_variables.end());
}

LGraphVariable* LGraph::FindVariable(const std::string& name) {
    for (auto* var : m_variables) {
        if (var->GetName() == name) return var;
    }
    return nullptr;
}