#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "LNode.h"
// Forward declaration
class LNode;

// The categories for nodes, used for organizing them in the UI.
enum class NodeType {
    Event,
    Logic,
    Data
};

// A factory function is a function that returns a new instance of an LNode.
using NodeFactory = std::function<LNode* ()>;

// This struct holds all the information the UI needs to list and create a node.
struct NodeInfo {
    std::string name;
    std::string category; // Changed from NodeType enum to string
    NodeFactory factory;
};

// A Singleton class to hold a central registry of all available node types.
class NodeRegistry {
public:

    static NodeRegistry& GetInstance();

    // Get the single, global instance of the registry.
    template<typename T>
    void RegisterNode(const std::string& name) {
        T tempNode; // Create a temporary instance to ask for its category
        m_nodeRegistry[name] = { name, tempNode.GetCategory(), []() { return new T(); } };
    }

    LNode* CreateNode(const std::string& name);
    std::vector<NodeInfo> GetAllNodes() const;




private:
    // Private constructor and deleted copy/assignment to enforce the Singleton pattern.
    NodeRegistry() = default;
    NodeRegistry(const NodeRegistry&) = delete;
    void operator=(const NodeRegistry&) = delete;

    // The map that stores all registered node types, keyed by their unique string name.
    std::map<std::string, NodeInfo> m_nodeRegistry;
};