#include "MAsm.h"
static MonoAssembly* assembly_preload_hook(MonoAssemblyName* aname, char** assemblies_path, void* user_data)
{
    const char* name = mono_assembly_name_get_name(aname);

    std::string fullPath = std::string("C:\\Vivid3D\\Vivid3D\\CS\\") + name + ".dll";

    MonoAssembly* loaded = mono_domain_assembly_open(mono_domain_get(), fullPath.c_str());

    if (!loaded) {
        std::cerr << "Failed to resolve referenced assembly: " << fullPath << std::endl;
    }

    return loaded;
}
MAsm::MAsm(std::string name,std::string path) {
	mono_set_assemblies_path(path.c_str());
    mono_install_assembly_preload_hook(assembly_preload_hook, nullptr);
	m_Assembly = mono_domain_assembly_open(MonoHost::m_Instance->GetDomain(),name.c_str());
	m_Image = mono_assembly_get_image(m_Assembly);

}

MClass* MAsm::GetClass(std::string nspace,std::string clsname) {

	MonoClass* testClass = mono_class_from_name(m_Image,nspace.c_str(),clsname.c_str());
	return new MClass(testClass);

}