#pragma once
#include "MonoHost.h"
#include "MAsm.h"
#include "MClass.h"
#include <string>
#include <vector>

struct SharpClassInfo {
    std::string namespaceName;
    std::string className;
    std::string baseClassName;
};

class MonoLib
{
public:
    MonoLib(const std::string& dllDirectory);
    ~MonoLib(); // Good practice to have a destructor

    std::vector<SharpClassInfo> GetClasses();
    MClass* GetClass(const std::string& name,std::string ns);

    MAsm* GetAssembly() const { return m_Assembly; }
    MAsm* GetVivid() const { return m_Vivid; }

private:
    MAsm* m_Assembly = nullptr; // For game.dll
    MAsm* m_Vivid = nullptr;    // For vividengine.dll
};