#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <string>
#include <iostream>
#include <vector>
#include <type_traits> // For std::is_same_v

#ifndef MONO_FIELD_ATTR_STATIC
#define MONO_FIELD_ATTR_STATIC 0x0010
#endif

// Forward declaration
class MClass;

enum SharpType
{
    SHARP_TYPE_INT,
    SHARP_TYPE_LONG,
    SHARP_TYPE_FLOAT,
    SHARP_TYPE_DOUBLE,
    SHARP_TYPE_BOOL,
    SHARP_TYPE_STRING,
    SHARP_TYPE_CLASS,
    SHARP_TYPE_VOID,
    SHARP_TYPE_INTPTR
};

struct SharpVar {
    std::string name;
    std::string etype;
    SharpType ctype = SHARP_TYPE_VOID;
};

class MClass
{
public:
    // CONSTRUCTORS UPDATED: Now require a MonoDomain* to know where they live.
    MClass(MonoDomain* domain, MonoClass* mono);
    MClass(MonoDomain* domain, MonoClass* mono, MonoObject* instance);

    // Creates a C# object instance within this class's domain
    MClass* CreateInstance();

    // Gets all instance fields of the class
    std::vector<SharpVar> GetInstanceFields();

    // Set/Get a native C++ pointer in a C# IntPtr field
    void SetNativePtr(const std::string& name, void* ptr);
    void* GetNativePtr(const std::string& name);

    // Returns the wrapped MonoObject* instance
    MonoObject* GetInstance() const { return m_Instance; }

    //--- Template Methods (Implementations are now inline as requested) ---//

    template<typename T>
    T GetFieldValue(const std::string& fieldName)
    {
        if (!m_Instance)
        {
            std::cerr << "Error: GetFieldValue called on a null instance for field: " << fieldName << std::endl;
            return T{};
        }

        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Error: Instance field not found: " << fieldName << std::endl;
            return T{};
        }

        T value{};
        mono_field_get_value(m_Instance, field, &value);
        return value;
    }

    template<typename T>
    void SetFieldValue(const std::string& fieldName, const T& value)
    {
        if (!m_Instance)
        {
            std::cerr << "Error: SetFieldValue called on a null instance for field: " << fieldName << std::endl;
            return;
        }

        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Error: Instance field not found: " << fieldName << std::endl;
            return;
        }

        // Mono expects a pointer to the value.
        mono_field_set_value(m_Instance, field, (void*)&value);
    }

    // Function to set a field with another class instance
    void SetFieldClass(const std::string& fieldName, MClass* value)
    {
        if (!m_Instance)
        {
            std::cerr << "Error: SetFieldClass called on a null instance for field: " << fieldName << std::endl;
            return;
        }
        if (!value || !value->GetInstance())
        {
            std::cerr << "Error: SetFieldClass called with a null value for field: " << fieldName << std::endl;
            return;
        }

        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Error: Instance field not found: " << fieldName << std::endl;
            return;
        }

        mono_field_set_value(m_Instance, field, value->GetInstance());
    }


    template<typename T>
    T GetStaticFieldValue(const std::string& fieldName)
    {
        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Error: Static field not found: " << fieldName << std::endl;
            return T{};
        }

        // Get the vtable within the correct domain
        MonoVTable* vtable = mono_class_vtable(m_Domain, m_Class);
        mono_runtime_class_init(vtable); // Ensure static constructor is run

        T value{};
        mono_field_static_get_value(vtable, field, &value);
        return value;
    }

    template<typename T>
    void SetStaticFieldValue(const std::string& fieldName, const T& value)
    {
        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Error: Static field not found: " << fieldName << std::endl;
            return;
        }

        MonoVTable* vtable = mono_class_vtable(m_Domain, m_Class);
        if (!vtable)
        {
            std::cerr << "Error: mono_class_vtable returned null." << std::endl;
            return;
        }
        mono_runtime_class_init(vtable);

        mono_field_static_set_value(vtable, field, (void*)&value);
    }

    template<typename... Args>
    MonoObject* CallFunction(const std::string& methodName, Args&&... args)
    {
        if (!m_Instance)
        {
            std::cerr << "Error: CallFunction called on a null instance." << std::endl;
            return nullptr;
        }

        MonoMethod* method = mono_class_get_method_from_name(m_Class, methodName.c_str(), sizeof...(Args));
        if (!method)
        {
            std::cerr << "Error: Method not found: " << methodName << " with " << sizeof...(Args) << " arguments." << std::endl;
            return nullptr;
        }

        void* argArray[] = { ConvertArg(std::forward<Args>(args))... };

        MonoObject* exception = nullptr;
        MonoObject* result = mono_runtime_invoke(method, m_Instance, argArray, &exception);

        if (exception)
        {
            HandleException(exception);
            return nullptr;
        }
        return result;
    }

    // ADDED BACK: Overload for calling methods with no parameters.
    MonoObject* CallFunction(const std::string& methodName)
    {
        if (!m_Instance)
        {
            std::cerr << "Error: CallFunction called on a null instance." << std::endl;
            return nullptr;
        }

        // Look for a method with 0 parameters.
        MonoMethod* method = mono_class_get_method_from_name(m_Class, methodName.c_str(), 0);
        if (!method)
        {
            std::cerr << "Error: Method not found: " << methodName << " with 0 arguments." << std::endl;
            return nullptr;
        }

        MonoObject* exception = nullptr;
        // The third argument is nullptr because there are no parameters to pass.
        MonoObject* result = mono_runtime_invoke(method, m_Instance, nullptr, &exception);

        if (exception)
        {
            HandleException(exception);
            return nullptr;
        }
        return result;
    }


    template<typename... Args>
    MonoObject* CallStaticFunction(const std::string& methodName, Args&&... args)
    {
        MonoMethod* method = mono_class_get_method_from_name(m_Class, methodName.c_str(), sizeof...(Args));
        if (!method)
        {
            std::cerr << "Error: Static method not found: " << methodName << std::endl;
            return nullptr;
        }

        void* argArray[] = { ConvertArg(std::forward<Args>(args))... };
        MonoObject* exception = nullptr;
        MonoObject* result = mono_runtime_invoke(method, nullptr /* static call */, argArray, &exception);

        if (exception)
        {
            HandleException(exception);
            return nullptr;
        }
        return result;
    }

    template<typename Ret, typename... Args>
    Ret CallFunctionValue(const std::string& methodName, Args&&... args)
    {
        MonoObject* result = CallFunction(methodName, std::forward<Args>(args)...);
        if (!result)
            return Ret{};

        // Unbox the value type from the MonoObject* wrapper
        return *(Ret*)mono_object_unbox(result);
    }

    template<typename Ret, typename... Args>
    Ret CallStaticFunctionValue(const std::string& methodName, Args&&... args)
    {
        MonoObject* result = CallStaticFunction(methodName, std::forward<Args>(args)...);
        if (!result)
            return Ret{};

        return *(Ret*)mono_object_unbox(result);
    }

    std::string CallFunctionValue_String(const std::string& name) {
        MonoObject* result = CallFunction(name);
        if (!result) return {};
        MonoString* monoStr = reinterpret_cast<MonoString*>(result);
        char* cStr = mono_string_to_utf8(monoStr);
        std::string finalStr(cStr);
        mono_free(cStr);
        return finalStr;
    }

private:
    void HandleException(MonoObject* exception)
    {
        if (!exception) return;
        MonoString* excStr = mono_object_to_string(exception, nullptr);
        char* excMsg = mono_string_to_utf8(excStr);
        std::cerr << "Mono Exception: " << excMsg << std::endl;
        mono_free(excMsg);
    }

    // --- Argument Conversion Helpers (now domain-aware) ---

    // For primitive types, Mono expects a pointer to the value.
    void* ConvertArg(int value) { return &value; }
    void* ConvertArg(float value) { return &value; }
    void* ConvertArg(double value) { return &value; }
    void* ConvertArg(bool value) { return &value; }
    void* ConvertArg(long long value) { return &value; }

    // For strings, create a MonoString in the correct domain.
    void* ConvertArg(const std::string& str) {
        return mono_string_new(m_Domain, str.c_str());
    }
    void* ConvertArg(const char* str) {
        return mono_string_new(m_Domain, str);
    }

    // For raw pointers (used for C# IntPtr).
    void* ConvertArg(void* ptr) {
        return ptr;
    }

    // For passing other wrapped class instances.
    void* ConvertArg(MClass* value) {
        return value ? value->GetInstance() : nullptr;
    }

    MonoDomain* m_Domain;     // The domain this class/instance belongs to
    MonoClass* m_Class;      // The C# class definition
    MonoObject* m_Instance;   // The C# object instance (if any)
};


// --- Template Specializations ---

template<>
inline std::string MClass::GetFieldValue<std::string>(const std::string& fieldName)
{
    if (!m_Instance) return {};
    MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
    if (!field) return {};

    MonoString* monoStr = nullptr;
    mono_field_get_value(m_Instance, field, &monoStr);
    if (!monoStr) return {};

    char* utf8 = mono_string_to_utf8(monoStr);
    std::string result(utf8);
    mono_free(utf8);
    return result;
}

template<>
inline MClass* MClass::GetFieldValue<MClass*>(const std::string& fieldName)
{
    if (!m_Instance) return nullptr;
    MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
    if (!field) return nullptr;

    MonoObject* innerObject = nullptr;
    mono_field_get_value(m_Instance, field, &innerObject);
    if (!innerObject) return nullptr;

    MonoClass* innerClass = mono_object_get_class(innerObject);
    // Create the new MClass wrapper, passing along the correct domain
    return new MClass(m_Domain, innerClass, innerObject);
}

template<>
inline void MClass::SetFieldValue<std::string>(const std::string& fieldName, const std::string& value)
{
    if (!m_Instance) return;
    MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
    if (field) {
        // Create the MonoString in the correct domain
        MonoString* monoStr = mono_string_new(m_Domain, value.c_str());
        mono_field_set_value(m_Instance, field, monoStr);
    }
}
