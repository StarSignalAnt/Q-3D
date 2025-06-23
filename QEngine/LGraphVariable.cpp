#include "LGraphVariable.h"
#include "GraphNode.h"
#include "JsonGlm.h"
LGraphVariable::LGraphVariable(const std::string& name, DataType type)
    : m_name(name), m_type(type)
{
    // Initialize the variant with a sensible default for its type
    switch (m_type) {
    case DataType::Int:    m_defaultValue = 0;              break;
    case DataType::Float:  m_defaultValue = 0.0f;           break;
    case DataType::Bool:   m_defaultValue = false;          break;
    case DataType::String: m_defaultValue = std::string(""); break;
    case DataType::Vec2:   m_defaultValue = glm::vec2(0.0f); break;
    case DataType::Vec3:   m_defaultValue = glm::vec3(0.0f); break;
    case DataType::Vec4:   m_defaultValue = glm::vec4(0.0f); break;
    case DataType::Mat4:   m_defaultValue = glm::mat4(1.0f); break;
    case DataType::GraphNodeRef: m_defaultValue = std::string(""); break;
    default:               m_defaultValue = 0;              break;
    }
}

void LGraphVariable::ToJson(json& j) const
{
    j["name"] = m_name;
    j["type"] = static_cast<int>(m_type);

    // The std::visit block that was causing the error will now compile
    // correctly because the required glm helpers are visible via JsonGlm.h
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, GraphNode*>) {
            j["defaultValue"] = arg ? arg->GetName() : "";
        }
        else {
            j["defaultValue"] = arg; // This line now works for all types
        }
        }, m_defaultValue);
}


void LGraphVariable::FromJson(const json& j)
{
    m_name = j.at("name");
    m_type = static_cast<DataType>(j.at("type").get<int>());

    // This logic needs to be expanded in your LNode::FromJson as well
    // to correctly load the variant from JSON.
    switch (m_type) {
    case DataType::Int:    m_defaultValue = j.at("defaultValue").get<int>(); break;
    case DataType::Float:  m_defaultValue = j.at("defaultValue").get<float>(); break;
    case DataType::Bool:   m_defaultValue = j.at("defaultValue").get<bool>(); break;
    case DataType::String:
    case DataType::GraphNodeRef: m_defaultValue = j.at("defaultValue").get<std::string>(); break;
    case DataType::Vec2:   m_defaultValue = j.at("defaultValue").get<glm::vec2>(); break;
    case DataType::Vec3:   m_defaultValue = j.at("defaultValue").get<glm::vec3>(); break;
    case DataType::Vec4:   m_defaultValue = j.at("defaultValue").get<glm::vec4>(); break;
    case DataType::Mat4:   m_defaultValue = j.at("defaultValue").get<glm::mat4>(); break;
    }
}