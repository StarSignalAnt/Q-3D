#include "MAsm.h"

MAsm::MAsm(std::string name) {

	m_Assembly = mono_domain_assembly_open(MonoHost::m_Instance->GetDomain(),name.c_str());
	m_Image = mono_assembly_get_image(m_Assembly);

}

MClass* MAsm::GetClass(std::string nspace,std::string clsname) {

	MonoClass* testClass = mono_class_from_name(m_Image,nspace.c_str(),clsname.c_str());
	return new MClass(testClass);

}