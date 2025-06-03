#pragma once
#include "Component.h"
#include <string>
#include <vector>

enum SVarType {
    VT_Int,
    VT_String,
    VT_Float
};

struct ScriptVar {
    std::string name;
    SVarType type;
    int ivalue = 0;
    float fvalue = 0;
    std::string svalue = "";
};

class ScriptComponent :
    public Component
{
public:

    void SetScript(std::string file, std::string name);
    void OnUpdate(float dt) override;
    std::vector<ScriptVar> GetVars();
    std::string GetName() {
        return m_Name;
    }
    int GetInt(std::string name);
    void SetInt(std::string name, int value);
    float GetFloat(std::string name);
    void SetFloat(std::string name, float value);
    void SetString(std::string name, std::string value);
    std::string GetString(std::string name);
    void Push() override;
    void Pop() override;
private:

    void* m_ComponentPy = nullptr;
    std::string m_Name = "";
    std::vector<ScriptVar> m_Pushed;
};

