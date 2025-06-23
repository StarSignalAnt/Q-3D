#include "NodeRegistry.h"
#include "LNode.h" // Required for LNode definition

// Implementation of the Singleton's static GetInstance method.
NodeRegistry& NodeRegistry::GetInstance() {
    // This static instance is created only once, the first time this function is called.
    static NodeRegistry instance;
    return instance;
}

// Looks up a node by name in the map and calls its factory function.
LNode* NodeRegistry::CreateNode(const std::string& name) {
    auto it = m_nodeRegistry.find(name);
    if (it != m_nodeRegistry.end()) {
        // If found, execute the factory function to get a new node instance.
        return it->second.factory();
    }
    // Return nullptr if no node with that name is registered.
    return nullptr;
}

// Collects all registered nodes into a vector.
std::vector<NodeInfo> NodeRegistry::GetAllNodes() const {
    std::vector<NodeInfo> nodes;
    nodes.reserve(m_nodeRegistry.size());
    for (const auto& pair : m_nodeRegistry) {
        nodes.push_back(pair.second);
    }
    return nodes;
}

