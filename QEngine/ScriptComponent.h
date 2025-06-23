#pragma once
#include "Component.h"
#include <string>
#include <vector>

enum SVarType {
    VT_Int,
    VT_String,
    VT_Float,
    VT_Object,
    VT_Unknown
};



struct ScriptVar {
    std::string name;
    SVarType type;
    int ivalue = 0;
    std::string cls;
    float fvalue = 0;
    std::string svalue = "";
};

class ScriptComponent :
    public Component
{
public:

    void OnPlay() override;
    void OnStop() override;
    void SetScript(std::string file, std::string name);
    std::string GetPath() {
        return m_Path;
    }
    std::string GetName() {
        return m_Name;
    }
    void ReInit();
    void* GetInstance() {
        return m_ComponentPy;
    }
    void OnUpdate(float dt) override;
    std::vector<ScriptVar> GetVars();
    std::vector<ScriptVar> GetPythonVars();
 
    int GetInt(std::string name);
    void SetInt(std::string name, int value);
    float GetFloat(std::string name);
    void SetFloat(std::string name, float value);
    void SetString(std::string name, std::string value);
    void SetNode(std::string name, GraphNode* node);
    GraphNode* GetNode(std::string name);
    std::string GetString(std::string name);
    void Push() override;
    void Pop() override;
private:

    void* m_ComponentPy = nullptr;
    std::string m_Name = "";
    std::vector<ScriptVar> m_Pushed;
    std::string m_Path = "";
};

