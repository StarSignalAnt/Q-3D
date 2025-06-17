#include "LGInput.h"

LGInput::LGInput(LNode* parent, int portIndex, std::string name, DataType type)
    : m_parentNode(parent), m_portIndex(portIndex), m_Name(name), m_Type(type)
{
    switch (m_Type) {
    case DataType::Int:    m_defaultValue = 0;                break;
    case DataType::Float:  m_defaultValue = 0.0f;             break;
    case DataType::Bool:   m_defaultValue = false;            break; // <-- ADD THIS
    case DataType::String: m_defaultValue = std::string("");   break;
    case DataType::Vec2:   m_defaultValue = glm::vec2(0.0f);   break;
    case DataType::Vec3:   m_defaultValue = glm::vec3(0.0f);   break;
    case DataType::Vec4:   m_defaultValue = glm::vec4(0.0f);   break;
        // Other types can be added here as needed.
    default:               m_defaultValue = 0;                break;
    }
    // The initializer list handles all member setup.
}