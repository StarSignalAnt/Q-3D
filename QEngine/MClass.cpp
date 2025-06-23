#include "MClass.h"
#include <iostream>
#include <vector>

// Constructor for class representations (no object instance)
MClass::MClass(MonoDomain* domain, MonoClass* cls)
    : m_Domain(domain), m_Class(cls), m_Instance(nullptr)
{
    if (!m_Domain || !m_Class) {
        std::cerr << "Warning: MClass created with null domain or class." << std::endl;
    }
}

// Constructor for wrapping an existing C# object instance
MClass::MClass(MonoDomain* domain, MonoClass* cls, MonoObject* obj)
    : m_Domain(domain), m_Class(cls), m_Instance(obj)
{
    if (!m_Domain || !m_Class) {
        std::cerr << "Warning: MClass wrapper created with null domain or class." << std::endl;
    }
}

MClass* MClass::CreateInstance() {
    if (!m_Domain || !m_Class) {
        std::cerr << "Error: Cannot create instance from a null domain or class." << std::endl;
        return nullptr;
    }

    // Create a new object instance within the correct domain
    MonoObject* instance = mono_object_new(m_Domain, m_Class);
    if (!instance) {
        std::cerr << "Error: mono_object_new failed." << std::endl;
        return nullptr;
    }

    mono_runtime_object_init(instance); // Call the default C# constructor

    // Wrap the new instance in a new MClass object
    return new MClass(m_Domain, m_Class, instance);
}

std::vector<SharpVar> MClass::GetInstanceFields() {
    std::vector<SharpVar> fields;
    if (!m_Class) return fields;

    void* iter = nullptr;
    MonoClassField* field = nullptr;

    while ((field = mono_class_get_fields(m_Class, &iter))) {
        // Skip static fields, we only want instance fields
        if ((mono_field_get_flags(field) & MONO_FIELD_ATTR_STATIC) != 0) {
            continue;
        }

        const char* fieldName = mono_field_get_name(field);
        MonoType* fieldType = mono_field_get_type(field);
        const char* typeName = mono_type_get_name(fieldType);

        SharpVar var;
        var.name = fieldName ? fieldName : "";
        var.etype = typeName ? typeName : "";

        // You can expand this logic as needed to map more types to your enum
        if (var.etype == "System.Int32") var.ctype = SHARP_TYPE_INT;
        else if (var.etype == "System.Single") var.ctype = SHARP_TYPE_FLOAT;
        else if (var.etype == "System.String") var.ctype = SHARP_TYPE_STRING;
        else if (var.etype == "System.Boolean") var.ctype = SHARP_TYPE_BOOL;
        else if (mono_type_get_type(fieldType) == MONO_TYPE_CLASS) var.ctype = SHARP_TYPE_CLASS;
        // ... etc

        fields.push_back(var);
    }
    return fields;
}

void MClass::SetNativePtr(const std::string& name, void* ptr) {
    if (!m_Class || !m_Instance) return;
    MonoClassField* field = mono_class_get_field_from_name(m_Class, name.c_str());
    if (field) {
        mono_field_set_value(m_Instance, field, &ptr);
    }
    else {
        std::cerr << "Error: Field '" << name << "' not found for SetNativePtr." << std::endl;
    }
}

void* MClass::GetNativePtr(const std::string& name) {
    if (!m_Class || !m_Instance) return nullptr;
    MonoClassField* field = mono_class_get_field_from_name(m_Class, name.c_str());
    if (field) {
        void* ptrValue = nullptr;
        mono_field_get_value(m_Instance, field, &ptrValue);
        return ptrValue;
    }
    else {
        std::cerr << "Error: Field '" << name << "' not found for GetNativePtr." << std::endl;
    }
    return nullptr;
}
