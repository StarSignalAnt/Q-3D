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
    float GetFloat(std::string name);
    void SetFloat(std::string name, float value);
    void SetString(std::string name, std::string value);
    std::string GetString(std::string name);

private:

    void* m_ComponentPy = nullptr;
    std::string m_Name = "";
};

