#pragma once
#include "json.hpp"
#include "LGInput.h"
#include "LGOutput.h"
#include <vector>
#include <string>
#include <optional>
#include <map>
#include "SceneGraph.h" 
#include "GraphNode.h"
#include "SceneGraph.h" 
class LGraph;
using json = nlohmann::json;

// A new struct to define a named execution output pin
struct LExecOutputPin {
    std::string name;
    LNode* nextNode = nullptr;
};

class LNode
{
public:
    LNode();
    virtual ~LNode() = default;

    virtual void ToJson(json& j);
    virtual void FromJson(const json& j, LGraph* graph);

    // --- Data Pin Methods (Unchanged) ---
    template<typename T>
    std::optional<T> GetInputValue(const std::string& name); // Implementation below
    void AddInput(std::string name, DataType type);
    void AddOutput(std::string name, DataType type);

    // --- NEW: Execution Pin Methods ---
    void AddExecOutput(const std::string& name);
    void SetNextExec(int pinIndex, LNode* node);
    void ExecNext(int pinIndex = 0); // Default to firing the first exec output

    // --- Getters and Setters ---
    const std::vector<LExecOutputPin*>& GetExecOutputs() const { return m_execOutputs; }
    // ... (other getters/setters are unchanged) ...
    void SetID(int id) { m_id = id; }
    int GetID() const { return m_id; }
    void SetEditorPosition(const glm::vec2& pos) { m_editorPosition = pos; }
    const glm::vec2& GetEditorPosition() const { return m_editorPosition; }
    std::string GetTypeName() const { return m_typeName; }
    void SetTypeName(std::string name) { m_typeName = name; }
    virtual void Exec() {}
    bool hasExecIn() const { return m_hasExecIn; }
    bool hasExecOut() const { return m_hasExecOut; }
    void SetName(std::string name) { m_Name = name; }
    std::string GetName() const { return m_Name; }
    virtual std::string GetCategory() const { return "Miscellaneous"; }
    const std::vector<LGInput*>& GetInputs() const { return m_Inputs; }
    const std::vector<LGOutput*>& GetOutputs() const { return m_Outputs; }
    virtual void CalculateOutputs() {}
protected:
    std::string m_Name;
    std::string m_typeName;
    std::vector<LGInput*> m_Inputs;
    std::vector<LGOutput*> m_Outputs;
    bool m_hasExecIn;
    bool m_hasExecOut;
    // --- REPLACED: m_nextExec is now a vector of named pins ---
    std::vector<LExecOutputPin*> m_execOutputs;
    int m_id;
    glm::vec2 m_editorPosition;
};

template<typename T>
std::optional<T> LNode::GetInputValue(const std::string& name) {
    LGInput* input = nullptr;
    for (auto* in : m_Inputs) {
        if (in->GetName() == name) {
            input = in;
            break;
        }
    }
    if (!input) return std::nullopt;

    // First, get the ValueVariant, whether from a connected node or a local default value.
    std::optional<LGInput::ValueVariant> valueOpt;
    if (input->isConnected()) {
        LGOutput* sourceOutput = input->GetConnection();
        LNode* sourceNode = sourceOutput->getParentNode();
        if (sourceNode) {
            // Ensure the source node has calculated its value
            sourceNode->CalculateOutputs();
        }
        if (sourceOutput) {
            valueOpt = sourceOutput->GetValue();
        }
    }
    else {
        // For unconnected pins, get the local default value
        valueOpt = input->GetDefaultValue();
    }

    if (!valueOpt.has_value()) {
        return std::nullopt;
    }
    const LGInput::ValueVariant& val_variant = valueOpt.value();

    // --- BEGIN NEW LOGIC ---

    // This 'if constexpr' block adds special handling ONLY when you explicitly
    // request a GraphNode* (i.e., GetInputValue<GraphNode*>)
    if constexpr (std::is_same_v<T, GraphNode*>) {
        // Check if the variant holds a string, which is how we store GraphNode references
        if (const std::string* nodeName = std::get_if<std::string>(&val_variant)) {
            if (!nodeName->empty()) {
                // If it's a string, use your SceneGraph to find the node.
                // NOTE: You must have a way to access your scene graph,
                // for example, through a singleton instance.
                return SceneGraph::m_Instance->FindNode(*nodeName);
            }
        }
    }

    // --- END NEW LOGIC ---

    // For all other cases (or if the GraphNode* was already a pointer in the variant),
    // use the default behavior. This keeps it working for int, float, vec3, etc.
    if (const T* pval = std::get_if<T>(&val_variant)) {
        return *pval;
    }

    return std::nullopt;
}