#include "pch.h"
#include "VModule.h"

VModule::VModule() {


}

void VModule::SetName(VName name) {

	m_Name = name;

}

void VModule::AddClass(VClass* cls) {

	m_Classes.push_back(cls);

}

void VModule::AddEnum(VEnum* e) {

	m_Enums.push_back(e);
	
}

VName VModule::GetName() {

	return m_Name;

}

VModule* VModule::Clone() {

	VModule* clone = new VModule;
	clone->SetName(m_Name);

	for (auto cls : m_Classes)
	{
		clone->AddClass(cls->Clone());
	}

	return clone;


}

VClass* VModule::FindClass(std::string name)
{

	for (auto cls : m_Classes)
	{

		auto c_name = cls->GetName();
		if (c_name.GetNames()[0] == name)
		{
			return cls;
		}

	}
	return nullptr;
}