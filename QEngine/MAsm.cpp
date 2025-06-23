#include "MAsm.h"
#include <iostream>

// The constructor now takes the specific domain it should load the assembly into.
MAsm::MAsm(MonoDomain* domain, const std::string& assemblyPath)
    : m_Assembly(nullptr), m_Image(nullptr),m_Domain(domain)
{
    if (!domain) {
        std::cerr << "FATAL: MAsm created with a null domain for assembly: " << assemblyPath << std::endl;
        return;
    }

    // This is the core of the class. It loads the specified assembly
    // directly into the provided domain.
    m_Assembly = mono_domain_assembly_open(domain, assemblyPath.c_str());

    if (!m_Assembly) {
        std::cerr << "Error: Failed to load assembly: " << assemblyPath << std::endl;
        return;
    }

    m_Image = mono_assembly_get_image(m_Assembly);
}

MClass* MAsm::GetClass(const std::string& nspace, const std::string& clsname) {
    if (!m_Image) {
        return nullptr;
    }
    MonoClass* testClass = mono_class_from_name(m_Image, nspace.c_str(), clsname.c_str());
    if (!testClass) {
        std::cerr << "Error: Class not found - Namespace: " << nspace << ", Class: " << clsname << std::endl;
        return nullptr;
    }
    // Assumes MClass constructor is MClass(MonoClass*)
    return new MClass(m_Domain, testClass);
}