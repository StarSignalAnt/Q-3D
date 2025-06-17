#pragma once
#include <vector>
#include <string>

// Forward declarations
class LNode;
class LEventNode;
class NodeRegistry;

class LGraph
{
public:
    LGraph(std::string name);
    ~LGraph();

    void AddNode(LNode* node);
    const std::vector<LNode*>& GetNodes() const;

    // Fires an event in the graph by its name (e.g., "On Tick").
    void FireEvent(const std::string& eventName);

    // JSON-based save and load
    void SaveToFile(const std::string& filepath);
    static LGraph* LoadFromFile(const std::string& filepath, NodeRegistry& registry);
    std::string GetName() {
        return m_name;
    }

private:
    std::string m_name;
    std::vector<LNode*> m_nodes;
    std::vector<LEventNode*> m_eventNodes;
};