#include "ScriptHost.h"
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <variant>            // std::variant, std::visit
#include <string>             // std::string
#include <vector>             // std::vector
#include <utility>  
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "GraphNode.h"

namespace py = pybind11;


py::scoped_interpreter* _sh;

ScriptHost* ScriptHost::m_Host = nullptr;

std::string VarTypeToString(VarType type) {
    switch (type) {
    case VarType::T_Int:    return "Int";
    case VarType::T_Float:  return "Float";
    case VarType::T_Bool:   return "Bool";
    case VarType::T_String: return "String";
    case VarType::T_Class:  return "Class";
    case VarType::T_CPtr:   return "CPtr";
    case VarType::T_Object: return "Object";
    case VarType::T_Unknown:return "Unknown";
    default:                return "Invalid";
    }
}


bool LoadRun(const std::string& path) {
    try {
        // Read entire file into a string
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string code = buffer.str();

        // Execute the code in the global namespace
        py::exec(code, py::globals());

        std::cout << "Successfully executed script: " << path << std::endl;
        return true;
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error executing " << path << ": " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error executing " << path << ": " << e.what() << std::endl;
        return false;
    }
}
void LoadPY(const std::string& filepath) {
    std::cout << "Loading Python file: " << filepath << std::endl;
    // Your loading logic here
    LoadRun(filepath);
}

void ScanAndLoadPY(const std::filesystem::path& rootDir) {
    if (!std::filesystem::exists(rootDir) || !std::filesystem::is_directory(rootDir)) {
        std::cerr << "Invalid directory: " << rootDir << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(rootDir)) {
        if (entry.is_regular_file()) {
            const auto& path = entry.path();
            if (path.extension() == ".py") {
                LoadPY(path.string());
            }
        }
    }
}

int testFunc(int x, int y) {
    printf("TestFunc!\n");
    return x + y * 2;
}

py::module_ engine_module;
static pybind11::module_ g_engine_module;
static bool g_module_initialized = false;

void initEngineModule() {
    if (g_module_initialized) return;

    // Create the engine module
    g_engine_module = pybind11::module_::create_extension_module(
        "engine",
        "Engine module with C++ functions",
        new pybind11::module_::module_def
    );

    // Add the module to sys.modules so Python can import it
    pybind11::module_ sys = pybind11::module_::import("sys");
    sys.attr("modules")["engine"] = g_engine_module;

    g_module_initialized = true;
}

template<typename Func>
void addFunction(const std::string& name, Func func) {
    if (!g_module_initialized) {
        initEngineModule();
    }
    g_engine_module.def(name.c_str(), func);
}

template<typename Func>
void addFunction(const std::string& name, Func func, const std::string& description) {
    if (!g_module_initialized) {
        initEngineModule();
    }
    g_engine_module.def(name.c_str(), func, description.c_str());
}


void sf_UpdateGraphNode(uintptr_t node, py::object position) {

    printf("UpdateGraphNode!!!\n");

    GraphNode* node2 = reinterpret_cast<GraphNode*>(node);




    float x = position.attr("x").cast<float>();
    float y = position.attr("y").cast<float>();
    float z = position.attr("z").cast<float>();
    printf("Name:");
    printf(node2->GetName().c_str());
    printf("\n");

    printf("Position: (%f, %f, %f)\n", x, y, z);
    node2->SetPosition(glm::vec3(x, y, z));

}

void sf_TurnGraphNode(uintptr_t node, py::object delta) {

    printf("TurnGraphNode!!!\n");

    GraphNode* node2 = reinterpret_cast<GraphNode*>(node);

    float x = delta.attr("x").cast<float>();
    float y = delta.attr("y").cast<float>();
    float z = delta.attr("z").cast<float>();
    
    node2->Turn(glm::vec3(x, y, z), false);


}

py::object sf_GetPositionGraphNode(uintptr_t node)
{

    GraphNode* node2 = reinterpret_cast<GraphNode*>(node);
    float x = node2->GetPosition().x;
    float y = node2->GetPosition().y;
    float z = node2->GetPosition().z;

    py::module glm = py::module::import("glm");

    // Create a glm.vec3 Python object with (x,y,z)
    py::object pyVec3 = glm.attr("vec3")(x, y, z);

    return pyVec3;

}

void InitNodeScript() {

    addFunction("updateGraphNode", sf_UpdateGraphNode);
    addFunction("turnGraphNode", sf_TurnGraphNode);
    addFunction("getPositionGraphNode", sf_GetPositionGraphNode);

}

ScriptHost::ScriptHost() {

	_sh = new py::scoped_interpreter();


	m_Host = this;

    addFunction("testFunc", testFunc);
    InitNodeScript();

  //  engine_module = py::module_::create_extension_module("engine", "Engine module with C++ functions", new py::module_::module_def);

    // Add your C++ functions to the module
//    engine_module.def("testFunc", &testFunc, "My custom function");
   // engine_module.def("calculate", &calculate, "Calculate something");
  //  engine_module.def("processText", &processText, "Process text input");
   // engine_module.def("checkValue", &checkValue, "Check if value meets condition");

    // Add the module to sys.modules so Python can import it
   // py::module_ sys = py::module_::import("sys");
    //sys.attr("modules")["engine"] = engine_module;

    // 1. Run your Python script that defines NodeModule and MyCustomNode

   ScanAndLoadPY("Engine/PY/Node");
   ScanAndLoadPY("Engine/PY/Component");



    // 2. Get the Python class object
//    py::object my_custom_node_class = py::globals()["MyCustomNode"];

    // 3. Create an instance by calling the class (constructor)
 //   py::object my_custom_node_instance = my_custom_node_class(); // calls MyCustomNode()

//    my_custom_node_instance.attr("DebugInfo")("Is this working?");

    // 4. Optionally call methods or access attributes
   // std::string name = my_custom_node_instance.attr("name").cast<std::string>();
    //std::cout << "Created Python node with name: " << name << "\n";
    
    int aa = 5;


	int b = 5;

}

void ScriptHost::Load(std::string path) {

    LoadRun(path);

}

void* ScriptHost::CreateInstance(std::string name) {

    py::object my_custom_node_class = py::globals()[name.c_str()];
    py::object* my_custom_node_instance = new py::object(my_custom_node_class());
    
    return static_cast<void*>(my_custom_node_instance);
}

void ScriptHost::TestFunc(void* func, std::string name) {

    py::object* obj_ptr = static_cast<py::object*>(func);
    py::object& obj = *obj_ptr;
    obj.attr(name.c_str())("It is now working");

}

py::object ConvertToPyObject(const ParamVariant& param) {
    return std::visit([](auto&& arg) -> py::object {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, int>) {
            return py::cast(arg);
        }
        else if constexpr (std::is_same_v<T, float>) {
            return py::cast(arg);
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return py::cast(arg);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return py::cast(arg);
        }
        else if constexpr (std::is_same_v<T, uintptr_t>) {
            // Just treat it as an integer in Python
            return py::cast(static_cast<std::uint64_t>(arg));
        }
        else if constexpr (std::is_same_v<T, PyClass>) {
            // Convert the raw pointer back to py::object
            py::object* objPtr = reinterpret_cast<py::object*>(arg.ptr);
            return *objPtr;
        }
        else {
            throw std::runtime_error("Unsupported type in ParamVariant");
        }
        }, param);
}
void ScriptHost::callFunc(void* obj1, const std::string& method_name, const Pars& pars) {
    py::object& obj = *static_cast<py::object*>(obj1);

    py::tuple args_tuple(pars.params.size());
    for (size_t i = 0; i < pars.params.size(); ++i) {
        args_tuple[i] = ConvertToPyObject(pars.params[i]);
    }

    obj.attr(method_name.c_str())(*args_tuple);
}

int ScriptHost::GetInt(void* inst, std::string name) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

        py::object attr = pyObj.attr(name.c_str());  // Get attribute by name
        return attr.cast<int>();                      // Cast attribute to int and return
 
}

void* ScriptHost::GetIntPtr(void* inst, const std::string& name) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

    py::object attr = pyObj.attr(name.c_str());      // Get attribute by name
    std::uint64_t ptrValue = attr.cast<std::uint64_t>(); // Cast Python int back to uintptr_t
    return reinterpret_cast<void*>(ptrValue);        // Convert integer back to void*
}

std::string ScriptHost::GetString(void* inst, const std::string& name) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

        py::object attr = pyObj.attr(name.c_str());  // Get attribute by name
    return attr.cast<std::string>();              // Cast attribute to std::string and return
}

void* ScriptHost::GetClass(void* inst, const std::string& name) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

    py::object attr = pyObj.attr(name.c_str());  // Get the class instance stored in the attribute

    // Allocate a new py::object on the heap and return as void*
    // Caller is responsible for managing this memory!
    return new py::object(attr);
}

float ScriptHost::GetFloat(void* inst, const std::string& name) {
    // Reinterpret the instance pointer as a py::object*
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

    // Get the attribute and cast it to float
    py::object attr = pyObj.attr(name.c_str());
    return attr.cast<float>();
}

bool ScriptHost::GetBool(void* inst, const std::string& name) {
    // Convert void* to py::object*
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

    // Access the attribute by name and cast it to bool
    py::object attr = pyObj.attr(name.c_str());
    return attr.cast<bool>();
}



std::unordered_map<std::string, VarType> ScriptHost::GetVarNames(void* inst) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

    std::unordered_map<std::string, VarType> result;
    py::dict attrs = py::getattr(pyObj, "__dict__");

    for (auto item : attrs) {
        std::string key = py::str(item.first);
        py::object val = py::reinterpret_borrow<py::object>(item.second);

        VarType type = VarType::T_Unknown;

        if (py::isinstance<py::bool_>(val)) {
            type = VarType::T_Bool;
        }
        else if (py::isinstance<py::int_>(val)) {
            type = VarType::T_Int;
        }
        else if (py::isinstance<py::float_>(val)) {
            type = VarType::T_Float;
        }
        else if (py::isinstance<py::str>(val)) {
            type = VarType::T_String;
        }
        else if (py::hasattr(val, "_is_pyclass_instance") && val.attr("_is_pyclass_instance").cast<bool>()) {
            type = VarType::T_Class;
        }
        else {
            type = VarType::T_Object;
        }

        result[key] = type;
    }

    return result;

}
void ScriptHost::SetInt(void* inst, const std::string& name, int value) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

    // Set the attribute by name to the int value
    pyObj.attr(name.c_str()) = value;
}

void ScriptHost::SetFloat(void* inst, const std::string& name, float value) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;
    pyObj.attr(name.c_str()) = value;
}

void ScriptHost::SetBool(void* inst, const std::string& name, bool value) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;
    pyObj.attr(name.c_str()) = value;
}

void ScriptHost::SetString(void* inst, const std::string& name, std::string value) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;
    pyObj.attr(name.c_str()) = value;
}

void ScriptHost::SetIntPtr(void* inst, const std::string& name, void* val) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

    // Convert void* pointer to uintptr_t and then cast to Python int
    uintptr_t ptrVal = reinterpret_cast<uintptr_t>(val);
    pyObj.attr(name.c_str()) = py::cast(ptrVal);
}

void ScriptHost::SetPtr(void* inst, const std::string& name, uintptr_t value) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

    // Store the uintptr_t pointer value as a Python integer attribute
    pyObj.attr(name.c_str()) = py::cast(value);
}

void ScriptHost::SetClass(void* inst, const std::string& name, void* classPtr) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    py::object& pyObj = *pyObjPtr;

    py::object* classObjPtr = static_cast<py::object*>(classPtr);
    pyObj.attr(name.c_str()) = *classObjPtr;
}