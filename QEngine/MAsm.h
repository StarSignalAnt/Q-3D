#pragma once
#include <string>
#include "MonoHost.h" // For Mono types
#include "MClass.h"

class MAsm
{
public:
    // CONSTRUCTOR CHANGED: It now requires a domain to load into.
    MAsm(MonoDomain* domain, const std::string& assemblyPath);

    MClass* GetClass(const std::string& nspace, const std::string& clsname);

    MonoAssembly* GetAsm() const {
        return m_Assembly;
    }

private:
    MonoDomain* m_Domain;
    MonoAssembly* m_Assembly;
    MonoImage* m_Image;
};