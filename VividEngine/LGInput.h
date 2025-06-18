#pragma once
#include <string>
#include <variant>
#include <glm/glm.hpp>
#include "DataType.h"

// Forward declarations
class LNode;
class LGOutput;
class GraphNode;
// The types of data an input pin can accept.
enum InputType {
    m_INT, m_FLOAT, m_STRING, m_NODE, m_VEC2, m_VEC3, m_VEC4, m_MAT4
};

class LGInput
{
public:
    // Constructor requires its parent node and its index within that node's input list.
    LGInput(LNode* parent, int portIndex, std::string name, DataType type);

    // --- Getters ---
    std::string GetName() const { return m_Name; }
    DataType GetType() const { return m_Type; }
    LNode* getParentNode() const { return m_parentNode; }
    int getPortIndex() const { return m_portIndex; }

    // --- Connection Methods ---
    bool isConnected() const { return m_connection != nullptr; }
    LGOutput* GetConnection() const { return m_connection; }
    void setConnection(LGOutput* output) { m_connection = output; }

    // --- Default Value Methods ---
    template<typename T>
    void SetDefaultValue(T value) {
        m_defaultValue = value;
    }
    using ValueVariant = std::variant<int, float, bool, std::string, glm::vec2, glm::vec3, glm::vec4,glm::mat4, GraphNode*>;

 

    const ValueVariant& GetDefaultValue() const {
        return m_defaultValue;
    }
private:
    std::string m_Name;

    DataType m_Type;
    // --- Core Members ---
    LNode* m_parentNode;
    int m_portIndex;
    LGOutput* m_connection = nullptr;
    ValueVariant m_defaultValue;
};