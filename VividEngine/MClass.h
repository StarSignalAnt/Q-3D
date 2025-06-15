#pragma once
#include "MonoHost.h"
#include <string>
#include <iostream>
#include <vector>
#ifndef MONO_FIELD_ATTR_STATIC
#define MONO_FIELD_ATTR_STATIC 0x0010
#endif
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
    MClass(MonoClass* mono);
    MClass(MonoClass* mono, MonoObject* instance);

    MClass* CreateInstance();


    std::vector<SharpVar> GetInstanceFields()
    {
        std::vector<SharpVar> fields;

        void* iter = nullptr;
        MonoClassField* field = nullptr;

        while ((field = mono_class_get_fields(m_Class, &iter)))
        {
            // Only instance fields, skip static fields
            uint32_t flags = mono_field_get_flags(field);

            bool isStatic = (flags & MONO_FIELD_ATTR_STATIC) != 0;

            if (isStatic) continue;

            std::string fieldName = std::string(mono_field_get_name(field));
            MonoType* fieldType = mono_field_get_type(field);
            std::string typeName = std::string(mono_type_get_name(fieldType));

            SharpType ctype = SHARP_TYPE_VOID;

            if (typeName == "System.Int32")
            {
                ctype = SHARP_TYPE_INT;
            }
            if (typeName == "System.Int64")
            {
                ctype = SHARP_TYPE_LONG;
            }

            if (typeName == "System.Single")
            {
                ctype = SHARP_TYPE_FLOAT;
            }
            if (typeName == "System.Double")
            {
                ctype = SHARP_TYPE_DOUBLE;
            }
            if (typeName == "System.String")
            {
                ctype = SHARP_TYPE_STRING;
            }
            if(typeName == "System.Boolean")
            {
                ctype = SHARP_TYPE_BOOL;
			}


            fields.push_back({ std::string(fieldName), std::string(typeName),ctype });
        }

        return fields;
    }

    void SetNativePtr(std::string name, void* ptr)
    {
        // Assume `instance` is a valid `MonoObject*` of type MyClass
       //MonoClass* klass = mono_object_get_class(instance);

        // Find the field
        MonoClassField* field = mono_class_get_field_from_name(m_Class, name.c_str());
        if (!field)
        {
            std::cerr << "Field not found!" << std::endl;
            return;
        }

        // Set the field directly (Mono and .NET treat IntPtr as void*)
        mono_field_set_value(m_Instance, field, &ptr);

    }

    void* GetNativePtr(std::string name) {


        // Find the field
        MonoClassField* field = mono_class_get_field_from_name(m_Class,name.c_str());
        if (!field)
        {
            std::cerr << "Field 'nativePtr' not found!" << std::endl;
            return nullptr;
        }

        // Retrieve the IntPtr field as void*
        void* nativePtr = nullptr;
        mono_field_get_value(m_Instance, field, nativePtr);

        // Now `nativePtr` holds the pointer stored in C# as IntPtr
        return nativePtr;

    }


    template<typename T>
    T GetFieldValue(const std::string& fieldName)
    {
        if (!m_Instance)
        {
            std::cerr << "Null MonoObject passed to GetFieldValue<>()" << std::endl;
            return T{};
        }

        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Instance field not found: " << fieldName << std::endl;
            return T{};
        }

        T value{};
        mono_field_get_value(m_Instance, field, &value);
        return value;
    }

    template<>
    inline std::string GetFieldValue<std::string>(const std::string& fieldName)
    {
        if (!m_Instance)
        {
            std::cerr << "Null MonoObject passed to GetFieldValue<std::string>()" << std::endl;
            return {};
        }

        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Instance string field not found: " << fieldName << std::endl;
            return {};
        }

        MonoString* monoStr = nullptr;
        mono_field_get_value(m_Instance, field, & monoStr);

        if (!monoStr)
            return {};

        char* utf8 = mono_string_to_utf8(monoStr);
        std::string result(utf8);
        mono_free(utf8);
        return result;
    }

    template<>
    inline MClass* GetFieldValue<MClass*>(const std::string& fieldName)
    {
        if (!m_Instance)
        {
            std::cerr << "Null instance passed to GetFieldValue<MClass*>() for field: " << fieldName << std::endl;
            return nullptr;
        }

        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Field not found: " << fieldName << std::endl;
            return nullptr;
        }

        MonoObject* innerObject = nullptr;
        mono_field_get_value(m_Instance, field, &innerObject);


        if (!innerObject)
        {
            std::cerr << "Field '" << fieldName << "' is null." << std::endl;
            return nullptr;
        }

        MonoClass* innerClass = mono_object_get_class(innerObject);

		MClass* wrapped = new MClass(innerClass, innerObject);
       
       // wrapped->m_Class = innerClass;
       // wrapped->m_Instance = innerObject;

        return wrapped;
    }

    template<typename T>
    T GetStaticFieldValue(const std::string& fieldName)
    {
        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Static field not found: " << fieldName << std::endl;
            return T{};
        }

        MonoVTable* vtable = mono_class_vtable(MonoHost::m_Instance->GetDomain(), m_Class);
        mono_runtime_class_init(vtable);

        T value{};
        mono_field_static_get_value(vtable, field, &value);
        return value;
    }

    template<>
    std::string GetStaticFieldValue<std::string>(const std::string& fieldName)
    {
        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field) {
            std::cerr << "Static field not found: " << fieldName << std::endl;
            return {};
        }

        MonoVTable* vtable = mono_class_vtable(MonoHost::m_Instance->GetDomain(), m_Class);
        mono_class_init(m_Class);

        MonoString* monoStr = nullptr;
        mono_field_static_get_value(vtable, field, &monoStr);

        if (!monoStr) return {};

        char* utf8 = mono_string_to_utf8(monoStr);
        std::string result(utf8);
        mono_free(utf8);
        return result;
    }

    template<>
    inline MClass* GetStaticFieldValue<MClass*>(const std::string& fieldName)
    {
        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Static field not found: " << fieldName << std::endl;
            return nullptr;
        }

        MonoVTable* vtable = mono_class_vtable(MonoHost::m_Instance->GetDomain(), m_Class);
        mono_runtime_class_init(vtable);// Ensures static constructors are run

        MonoObject* innerObject = nullptr;
        mono_field_static_get_value(vtable, field, &innerObject);

        if (!innerObject)
        {
            std::cerr << "Static field '" << fieldName << "' is null." << std::endl;
            return nullptr;
        }

        MonoClass* innerClass = mono_object_get_class(innerObject);

        MClass* wrapped = new MClass(innerClass, innerObject);
     
        return wrapped;
    }

    template<typename... Args>
    MonoObject* CallFunction(const std::string& methodName, Args&&... args)
    {
        MonoMethod* method = mono_class_get_method_from_name(m_Class, methodName.c_str(), sizeof...(Args));
        if (!method)
        {
            std::cerr << "Method not found: " << methodName << std::endl;
            return nullptr;
        }

        void* argArray[] = { ConvertArg(std::forward<Args>(args))... };

        MonoObject* exception = nullptr;
        MonoObject* result = mono_runtime_invoke(method, m_Instance, argArray, &exception);

        if (exception)
        {
            MonoString* excStr = mono_object_to_string(exception, nullptr);
            char* excMsg = mono_string_to_utf8(excStr);
            std::cerr << "Mono Exception: " << excMsg << std::endl;
            mono_free(excMsg);
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

        if constexpr (std::is_same_v<Ret, void*>)
        {
            return *(void**)mono_object_unbox(result);
        }
        else
        {
            return *(Ret*)mono_object_unbox(result);
        }
    }


    // Static method versions
    template<typename... Args>
    MonoObject* CallStaticFunction(const std::string& methodName, Args&&... args)
    {
        MonoMethod* method = mono_class_get_method_from_name(m_Class, methodName.c_str(), sizeof...(Args));
        if (!method)
        {
            std::cerr << "Static method not found: " << methodName << std::endl;
            return nullptr;
        }

        void* argArray[] = { ConvertArg(std::forward<Args>(args))... };

        MonoObject* exception = nullptr;
        MonoObject* result = mono_runtime_invoke(method, nullptr /* static call */, argArray, &exception);

        if (exception)
        {
            MonoString* excStr = mono_object_to_string(exception, nullptr);
            char* excMsg = mono_string_to_utf8(excStr);
            std::cerr << "Mono Exception: " << excMsg << std::endl;
            mono_free(excMsg);
            return nullptr;
        }

        return result;
    }

    template<typename Ret, typename... Args>
    Ret CallStaticFunctionValue(const std::string& methodName, Args&&... args)
    {
        MonoObject* result = CallStaticFunction(methodName, std::forward<Args>(args)...);
        return result ? *(Ret*)mono_object_unbox(result) : Ret{};
    }

    template<typename T>
    inline void SetStaticFieldValue(const std::string& fieldName, const T& value);

    inline void SetStaticClass(std::string name, MClass* cls) {

        if (!cls|| !cls->m_Instance)
        {
            std::cerr << "SetStaticFieldValue<MClass*> called with null instance for field: " << name << std::endl;
            return;
        }

        MonoClassField* field = mono_class_get_field_from_name(m_Class, name.c_str());
        if (!field)
        {
            std::cerr << "Static field not found: " << name << std::endl;
            return;
        }

        MonoDomain* domain = MonoHost::m_Instance->GetDomain();
        MonoVTable* vtable = mono_class_vtable(domain, m_Class);
        if (!vtable)
        {
            std::cerr << "mono_class_vtable returned null." << std::endl;
            return;
        }

        mono_runtime_class_init(vtable); // Important: run static .cctor

        mono_field_static_set_value(vtable, field, cls->GetInstance());

    }

    //template<>
    //inline void SetStaticFieldValue<MClass*>(const std::string& fieldName, MClass* value);

    template<typename T>
    void SetFieldValue(const std::string& fieldName, const T& value)
    {
        if (!m_Instance)
        {
            std::cerr << "Null MonoObject instance in SetFieldValue<>() for field: " << fieldName << std::endl;
            return;
        }

        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Instance field not found: " << fieldName << std::endl;
            return;
        }

        mono_field_set_value(m_Instance, field, (void*)&value);
    }

    // Specialization for std::string
    template<>
    inline void SetFieldValue<std::string>(const std::string& fieldName, const std::string& value)
    {
        if (!m_Instance)
        {
            std::cerr << "Null MonoObject instance in SetFieldValue<std::string>() for field: " << fieldName << std::endl;
            return;
        }

        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Instance string field not found: " << fieldName << std::endl;
            return;
        }

        MonoString* monoStr = mono_string_new(MonoHost::m_Instance->GetDomain(), value.c_str());
        mono_field_set_value(m_Instance, field, monoStr);
    }

    void SetFieldClass(const std::string& fieldName, MClass* value)
    {
        if (!m_Instance)
        {
            std::cerr << "Null MonoObject instance in SetFieldValue<>() for field: " << fieldName << std::endl;
            return;
        }

        MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
        if (!field)
        {
            std::cerr << "Instance field not found: " << fieldName << std::endl;
            return;
        }

        mono_field_set_value(m_Instance, field, (void*)value->GetInstance());
    }

    std::string CallFunctionValue_String(const std::string& name) {
        if (!m_Class || !m_Instance) {
            std::cerr << "Class or instance is null." << std::endl;
            return {};
        }

        MonoMethod* method = mono_class_get_method_from_name(m_Class, name.c_str(), 0);
        if (!method) {
            std::cerr << "Method not found: " << name << std::endl;
            return {};
        }

        MonoObject* result = mono_runtime_invoke(method, m_Instance, nullptr, nullptr);
        if (!result) {
            std::cerr << "Null return from method: " << name << std::endl;
            return {};
        }

        MonoString* monoStr = reinterpret_cast<MonoString*>(result);
        char* cStr = mono_string_to_utf8(monoStr);
        std::string finalStr(cStr);
        mono_free(cStr);
        return finalStr;
    }

	MonoObject* GetInstance() const { return m_Instance; }

private:


    void* ConvertArg(int value) { return new int(value); }
    void* ConvertArg(float value) { return new float(value); }
    void* ConvertArg(double value) { return new double(value); }
    void* ConvertArg(bool value) { return new bool(value); }
	void* ConvertArg(const std::string& str) { 
        
        MonoString* monoStr = mono_string_new(MonoHost::m_Instance->GetDomain(), str.c_str());
        return monoStr;
        
    }
	void* ConvertArg(const char* str) { 
    
        MonoString* monoStr = mono_string_new(MonoHost::m_Instance->GetDomain(), str);
        return monoStr;
    }
    void* ConvertArg(void* ptr)
{
    return ptr; // Mono will interpret this as an IntPtr
}
	MonoClass* m_Class;
    MonoObject* m_Instance;
};

template<typename T>
inline void MClass::SetStaticFieldValue(const std::string& fieldName, const T& value)
{
    MonoClassField* field = mono_class_get_field_from_name(m_Class, fieldName.c_str());
    if (!field)
    {
        std::cerr << "Static field not found: " << fieldName << std::endl;
        return;
    }

    MonoDomain* domain = MonoHost::m_Instance->GetDomain();
    MonoVTable* vtable = mono_class_vtable(domain, m_Class);
    if (!vtable)
    {
        std::cerr << "mono_class_vtable returned null." << std::endl;
        return;
    }

    mono_runtime_class_init(vtable);

    mono_field_static_set_value(vtable, field, (void*)&value);
}

