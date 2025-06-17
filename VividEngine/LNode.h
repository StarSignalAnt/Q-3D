#pragma once
#include "json.hpp"
#include "LGInput.h"
#include "LGOutput.h"
#include <vector>
#include <string>
#include <optional>
#include <map>

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
    virtual void FromJson(const json& j);

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

// Template implementation must be in the header file
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

    if (input->isConnected()) {
        LGOutput* sourceOutput = input->GetConnection();
        LNode* sourceNode = sourceOutput->getParentNode();
        if (sourceNode) sourceNode->Exec();
        if (const T* pval = std::get_if<T>(&sourceOutput->GetValue())) return *pval;
    }
    else {
        if (const T* pval = std::get_if<T>(&input->GetDefaultValue())) return *pval;
    }
    return std::nullopt;
}