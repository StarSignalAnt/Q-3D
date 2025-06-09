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
#include "GameInput.h"
#include "GameAudio.h"
#include "SceneGraph.h"
#include "ScriptComponent.h"

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

py::object sf_GetRotationGraphNode(uintptr_t node)
{

    GraphNode* node2 = reinterpret_cast<GraphNode*>(node);

    const glm::mat4& mat = node2->GetRotation(); // assuming GetRotation returns glm::mat4

    py::module glm = py::module::import("glm");

    // glm.mat4 expects a flat list of 16 floats (column-major order)
    std::vector<float> flatMatrix;
    flatMatrix.reserve(16);

    // Flatten glm::mat4 (column-major)
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            flatMatrix.push_back(mat[col][row]);
        }
    }

    // Call glm.mat4(*flatMatrix)
    py::object pyMat4 = glm.attr("mat4")(
        flatMatrix[0], flatMatrix[1], flatMatrix[2], flatMatrix[3],
        flatMatrix[4], flatMatrix[5], flatMatrix[6], flatMatrix[7],
        flatMatrix[8], flatMatrix[9], flatMatrix[10], flatMatrix[11],
        flatMatrix[12], flatMatrix[13], flatMatrix[14], flatMatrix[15]
        );

    return pyMat4;

}

void sf_SetRotationGraphNode(uintptr_t node,py::object mat4)
{

    GraphNode* node2 = reinterpret_cast<GraphNode*>(node);

    glm::mat4 cppMat;

    // Fill C++ mat4 from Python mat4 (column-major order)
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            cppMat[col][row] = mat4.attr("__getitem__")(col).attr("__getitem__")(row).cast<float>();
        }
    }

    node2->SetRotation(cppMat);

}

bool sf_GetKeyGameInput(int key) {

    return GameInput::m_Key[key];

}

void sf_PlaySongGameAudio(const std::string& path) {

//    const std::string& path
    std::cout << "SongPath:" << path << std::endl;
    GameAudio::m_Instance->PlaySong(path);

}
void sf_StopSongGameAudio() {

    GameAudio::m_Instance->StopSong();

}

uintptr_t sf_LoadSoundGameAudio(const std::string& path) {

    auto res = GameAudio::m_Instance->LoadSound(path);
    return reinterpret_cast<std::uintptr_t>(res);

}

int sf_PlaySoundGameAudio(uintptr_t ptr)
{
    std::cout << "Playing sound.\n";

    SoLoud::Wav* sound = reinterpret_cast<SoLoud::Wav*>(ptr);
    if (sound)
    {
        auto ptr = GameAudio::m_Instance->PlaySound(sound);
        //sound->play();
        return ptr;
    }
}

void sf_StopSoundGameAudio(int ptr) {

    printf("Stopping Sound\n");
    //exit(1);

    //void* sound = reinterpret_cast<void*>(ptr);
    GameAudio::m_Instance->StopSound(ptr);
}

void sf_SetPitchGameAudio(int id,float pitch) {

    GameAudio::m_Instance->SetPitch(id, pitch);

}

void sf_SetVolumeGameAudio(int id, float volume) {

    GameAudio::m_Instance->SetVolume(id, volume);

}

uintptr_t sf_GetScene() {

    auto res = SceneGraph::m_Instance;

    return reinterpret_cast<std::uintptr_t>(res);
}

py::object sf_RayCast(py::object position, py::object dir)
{

    float x = position.attr("x").cast<float>();
    float y = position.attr("y").cast<float>();
    float z = position.attr("z").cast<float>();

    float dx = dir.attr("x").cast<float>();
    float dy = dir.attr("y").cast<float>();
    float dz = dir.attr("z").cast<float>();

    std::cout << "Pos X:" << x << " Y:" << y << " Z:" << z << std::endl;
    std::cout << "Dir X:" << dx << " Y:" << dy << " Z:" << dz << std::endl;


    auto res = SceneGraph::m_CurrentGraph->RayCast(glm::vec3(x, y, z), glm::vec3(x+dx, y+dy, z+dz));


    py::object glm = py::module_::import("glm");
    py::object castResultClass = py::module_::import("__main__").attr("CastResult"); // Replace with actual module name

    // Create result instance
    py::object result = castResultClass();

    if (res.m_Hit == false) {

        result.attr("hit") = false;
        result.attr("point") = glm.attr("vec3")(0.0f,0.0f,0.0f);
        return result;

    }

    // Set fields
    result.attr("hit") = res.m_Hit;
    result.attr("distance") = res.m_Distance;
    result.attr("point") = glm.attr("vec3")(res.m_Point.x,res.m_Point.y,res.m_Point.z);
    result.attr("normal") = glm.attr("vec3")(0,0,0);
    //result.attr("hitNode") = py::none(); // or some actual node object
    py::object nodeResult = py::module_::import("__main__").attr("GraphNode"); // Replace with actual module name


    // Create result instance
    py::object node = nodeResult("");
    
    node.attr("cpp") = reinterpret_cast<std::uintptr_t>(res.m_Node);
    
    result.attr("hitNode") = node;

    
    return result;


}   

void sf_Stop() {

    exit(1);
}


py::object sf_GetRootNode(uintptr_t scene) {


    SceneGraph* graph = reinterpret_cast<SceneGraph*>(scene);
    int b = 5;
    py::object nodeClass = py::module_::import("__main__").attr("GraphNode"); // Replace with actual module name
    auto root = graph->GetRootNode();



    py::object res = nodeClass(root->GetName());
    res.attr("cpp") = reinterpret_cast<std::uintptr_t>(root);


    // Pass by value (copy of py::object)
    ScriptHost::m_Host->callFunc(&res, "Populate", Pars());

    return res;
}

int sf_GetNodeCount(py::object node)
{
    //GraphNode* onode = reinterpret_cast<GraphNode*>(node);
    
    uintptr_t p = (uintptr_t)node.attr("cpp").cast<uintptr_t>();

    GraphNode* onode = reinterpret_cast<GraphNode*>(p);
    int a = 5;

    //return onode->GetNodes().size();
    return onode->GetNodes().size();

}

py::object sf_GetSubNode(py::object node, int index)
{
    uintptr_t p = (uintptr_t)node.attr("cpp").cast<uintptr_t>();

    GraphNode* onode = reinterpret_cast<GraphNode*>(p);
    
    py::object nodeClass = py::module_::import("__main__").attr("GraphNode"); // Replace with actual module name
    auto root = onode->GetNodes()[index];

    py::object res = nodeClass(root->GetName());

    res.attr("cpp") = reinterpret_cast<std::uintptr_t>(root);

 

    
    return res;

}


void sf_SetBodyType(py::object node, int type)
{
    uintptr_t p = (uintptr_t)node.attr("cpp").cast<uintptr_t>();

    GraphNode* onode = reinterpret_cast<GraphNode*>(p);

    onode->SetBody((BodyType)type);

}

py::object sf_GetComponent(uintptr_t node,std::string comp)
{
 
    GraphNode* onode = reinterpret_cast<GraphNode*>(node);
    int b = 5;
    for (auto com : onode->GetComponents<ScriptComponent>())
    {
        if (com->GetName() == comp) {

            auto inst = com->GetInstance();
            py::object* inst1 = static_cast<py::object*>(inst);

            py::object ii = *inst1;
            return ii;


        }
    }
}

void InitNodeScript() {

    addFunction("updateGraphNode", sf_UpdateGraphNode);
    addFunction("turnGraphNode", sf_TurnGraphNode);
    addFunction("getPositionGraphNode", sf_GetPositionGraphNode);
    addFunction("getRotationGraphNode", sf_GetRotationGraphNode);
    addFunction("setRotationGraphNode", sf_SetRotationGraphNode);
    addFunction("getKeyGameInput", sf_GetKeyGameInput);
    addFunction("playSongGameAudio", sf_PlaySongGameAudio);
    addFunction("stopSongGameAudio", sf_StopSongGameAudio);
    addFunction("loadSoundGameAudio", sf_LoadSoundGameAudio);
    addFunction("playSoundGameAudio", sf_PlaySoundGameAudio);
    addFunction("stopSoundGameAudio", sf_StopSoundGameAudio);
    addFunction("setPitchGameAudio", sf_SetPitchGameAudio);
    addFunction("setVolumeGameAudio", sf_SetVolumeGameAudio);
    addFunction("getScene", sf_GetScene);
    addFunction("rayCast", sf_RayCast);
    addFunction("stop", sf_Stop);
    addFunction("getRootNode", sf_GetRootNode);
    addFunction("getNodeCount", sf_GetNodeCount);
    addFunction("getSubNode", sf_GetSubNode);
    addFunction("setBodyType", sf_SetBodyType);
    addFunction("getComponent", sf_GetComponent);
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
    ScanAndLoadPY("Engine/Py/Audio");
    ScanAndLoadPY("Engine/PY/Input");
   ScanAndLoadPY("Engine/PY/Node");
   ScanAndLoadPY("Engine/PY/Component");
   ScanAndLoadPY("Engine/PY/Scene");



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
void* ScriptHost::CreateComponentInstance(std::string name, void* cp) {

    py::object my_custom_node_class = py::globals()[name.c_str()];
    auto ii = reinterpret_cast<uintptr_t>(cp);

    py::object* my_custom_node_instance = new py::object(my_custom_node_class(ii,name));


    //*my_custom_node_class.attr() = reinterpret_cast<uintptr_t>(cp);



    return static_cast<void*>(my_custom_node_instance);
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

GraphNode* ScriptHost::GetGraphNode(void* inst, const std::string& name) {

    py::object* inst1 = static_cast<py::object*>(inst);

    py::object ii = *inst1;

    

    uintptr_t node = ii.attr(name.c_str()).attr("cpp").cast<uintptr_t>();

    if (node == 0) {
        return nullptr;
    }

    GraphNode* node2 = reinterpret_cast<GraphNode*>(node);


    return node2;
}

void ScriptHost::SetGraphNode(void* inst, const std::string& name, GraphNode* node) {

    py::object nodeClass = py::module_::import("__main__").attr("GraphNode"); // Replace with actual module name
    auto root = node;

    py::object res = nodeClass(root->GetName());

    res.attr("cpp") = reinterpret_cast<std::uintptr_t>(root);

    py::object* inst1 = static_cast<py::object*>(inst);

    py::object ii = *inst1;
    
    ii.attr(name.c_str()) = res;
        

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


std::vector<PythonVar> ScriptHost::GetVarDetails(void* inst) {
    py::object* pyObjPtr = static_cast<py::object*>(inst);
    if (!pyObjPtr) {
        return {};
    }
    py::object& pyObj = *pyObjPtr;

    std::vector<PythonVar> result;

    if (!py::hasattr(pyObj, "__dict__")) {
        return result;
    }

    py::dict attrs = py::getattr(pyObj, "__dict__");

    for (auto item : attrs) {
        std::string name = py::str(item.first);
        py::object val = py::reinterpret_borrow<py::object>(item.second);

        // Get raw class name from Python
        std::string py_class = py::str(val.attr("__class__").attr("__name__"));

        // Map common Python types to friendly engine types
        std::string type_str;

        if (py::isinstance<py::int_>(val)) {
            type_str = "Int";
        }
        else if (py::isinstance<py::float_>(val)) {
            type_str = "Float";
        }
        else if (py::isinstance<py::str>(val)) {
            type_str = "String";
        }
        else if (py::isinstance<py::bool_>(val)) {
            type_str = "Bool";
        }
        else if (py::isinstance<py::list>(val)) {
            type_str = "List";
        }
        else if (py::isinstance<py::dict>(val)) {
            type_str = "Dict";
        }
        else if (py::isinstance<py::tuple>(val)) {
            type_str = "Tuple";
        }
        else {
            // Use class name for unknown or user-defined types
            type_str = py_class;
        }

        result.push_back({ name, type_str });
    }

    return result;
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