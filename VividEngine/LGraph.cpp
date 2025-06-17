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

    for (const auto& node_json : root_json["nodes"]) {
        std::string typeName = node_json.at("typeName");
        LNode* newNode = registry.CreateNode(typeName);
        if (newNode) {
            newNode->FromJson(node_json);
            graph->AddNode(newNode);
            idToNodeMap[newNode->GetID()] = newNode;
        }
        else {
            std::cerr << "LOAD ERROR: NodeRegistry could not create node of type '" << typeName << "'. Is it registered?" << std::endl;
        }
    }

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

    return graph;
}