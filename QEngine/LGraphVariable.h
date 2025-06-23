#pragma once
#include <string>
#include "LGInput.h" // We reuse the ValueVariant for convenience
#include "DataType.h"
#include "json.hpp"

using json = nlohmann::json;

class LGraphVariable
{
public:
    LGraphVariable(const std::string& name, DataType type);

    // Getters
    std::string GetName() const { return m_name; }
    DataType GetType() const { return m_type; }
    const LGInput::ValueVariant& GetDefaultValue() const {
        return m_defaultValue;
    }

    // Setters
    void SetName(const std::string& name) { m_name = name; }
    void SetDefaultValue(const LGInput::ValueVariant& value) { m_defaultValue = value; }

    // Serialization
    void ToJson(json& j) const;
    void FromJson(const json& j);

private:
    std::string m_name;
    DataType m_type;
    LGInput::ValueVariant m_defaultValue;
};