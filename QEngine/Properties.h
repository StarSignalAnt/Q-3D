#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <any>
#include <memory>
#include <glm/glm.hpp>
#include <typeinfo> 

class PropertyEnumBase {
public:
    virtual ~PropertyEnumBase() = default; // Essential for base classes

    // This is the common interface the UI will use
    virtual std::vector<std::string> getAvailableOptions() const = 0;
    virtual std::string getValueAsString() const = 0;
    virtual void setValueFromString(const std::string& str) = 0;
};

// Step B: The templated wrapper now inherits from the base class
template<typename TEnum>
class PropertyEnum : public PropertyEnumBase {
private:
    TEnum* m_value_ptr;
    const std::vector<std::pair<TEnum, std::string>>& m_options;

public:
    PropertyEnum(TEnum* ptr, const std::vector<std::pair<TEnum, std::string>>& options)
        : m_value_ptr(ptr), m_options(options) {
    }

    // Implement the virtual functions from the base class
    std::vector<std::string> getAvailableOptions() const override {
        std::vector<std::string> names;
        for (const auto& pair : m_options) {
            names.push_back(pair.second);
        }
        return names;
    }

    std::string getValueAsString() const override {
        for (const auto& pair : m_options) {
            if (m_value_ptr && pair.first == *m_value_ptr) {
                return pair.second;
            }
        }
        return "Unknown";
    }

    void setValueFromString(const std::string& str) override {
        for (const auto& pair : m_options) {
            if (pair.second == str) {
                if (m_value_ptr) {
                    *m_value_ptr = pair.first;
                }
                return;
            }
        }
    }
};

enum class PropertyType {
    UNKNOWN,
    INT,
    FLOAT,
    STRING,
    DOUBLE,
    POINTER, // A generic pointer type
    GRAPHNODE,
    VEC3,
    ENUM
};

class GraphNode;

class Properties
{
private:
    std::unordered_map<std::string, std::any> m_Properties;
    std::vector<std::string> m_PropertyOrder;

public:
 
    template<typename T>
    void bind(const std::string& name, T* property_ptr) {
        if (m_Properties.find(name) == m_Properties.end()) {
            m_PropertyOrder.push_back(name);
        }
        m_Properties[name] = property_ptr;

    }
    template<typename TEnum>
    void bind_enum(const std::string& name, TEnum* ptr, const std::vector<std::pair<TEnum, std::string>>& options) {
        if (m_Properties.find(name) == m_Properties.end()) {
            m_PropertyOrder.push_back(name);
        }
        // Create the wrapper and store it as a shared_ptr to the base class
        auto enum_wrapper = std::make_shared<PropertyEnum<TEnum>>(ptr, options);
        m_Properties[name] = std::static_pointer_cast<PropertyEnumBase>(enum_wrapper);
    }

    PropertyEnumBase* get_enum(const std::string& name) const {
        auto it = m_Properties.find(name);
        if (it == m_Properties.end()) return nullptr;

        try {
            // Get the shared_ptr from the any, then get the raw base pointer
            const auto& base_ptr = std::any_cast<std::shared_ptr<PropertyEnumBase>>(it->second);
            return base_ptr.get();
        }
        catch (const std::bad_any_cast&) {
            return nullptr;
        }
    }
  
    template<typename T>
    T* get(const std::string& name) const {
        auto it = m_Properties.find(name);
        if (it == m_Properties.end()) {
            return nullptr;
        }
        try {
            return std::any_cast<T*>(it->second);
        }
        catch (const std::bad_any_cast&) {
            return nullptr;
        }
    }

    
    template<typename T>
    bool set(const std::string& name, const T& value) {
        if (T* ptr = get<T>(name)) {
            *ptr = value;
            return true;
        }
        return false;
    }

    bool has(const std::string& name) const {
        return m_Properties.count(name) > 0;
    }

    
    void unbind(const std::string& name) {
        m_Properties.erase(name);
        m_PropertyOrder.erase(
            std::remove(m_PropertyOrder.begin(), m_PropertyOrder.end(), name),
            m_PropertyOrder.end()
        );
    }

  
    size_t size() const {
        return m_Properties.size();
    }

  
    void clear() {
        m_PropertyOrder.clear();
        m_Properties.clear();
    }

    PropertyType GetType(const std::string& name) const {
        auto it = m_Properties.find(name);
        if (it == m_Properties.end()) return PropertyType::UNKNOWN;

        const std::any& prop = it->second;
        const std::type_info& type = prop.type();

        // Check for our new enum wrapper type first
        if (type == typeid(std::shared_ptr<PropertyEnumBase>)) {
            return PropertyType::ENUM;
        }
        // Check against known types
        if (type == typeid(int*))         return PropertyType::INT;
        if (type == typeid(float*))       return PropertyType::FLOAT;
        if (type == typeid(std::string*)) return PropertyType::STRING;
        if (type == typeid(double*))      return PropertyType::DOUBLE;
        if (type == typeid(glm::vec3*))   return PropertyType::VEC3;
        if (type == typeid(GraphNode*))   return PropertyType::GRAPHNODE;

        // Fallback check for any other pointer type
        // Note: This check is implementation-defined but works on most compilers (like MSVC, GCC, Clang).
        // 'P' is often at the start of the mangled name for a pointer.
        if (std::string_view(type.name()).find('P') != std::string_view::npos) {
            return PropertyType::POINTER;
        }

        return PropertyType::UNKNOWN;
    }

    auto begin() const { return m_PropertyOrder.cbegin(); }
    auto end() const { return m_PropertyOrder.cend(); }
};

