#include "pch.h"
#include "VScope.h"
#include "VVar.h"
#include "VClass.h"
#include "VFunction.h"

void VScope::RegisterVar(VVar* var) {

	m_LocalVars.push_back(var);

}

VVar* VScope::FindVar(std::string name) {

	for (auto v : m_LocalVars) {

		if (v->IsName(name)) {
			return v;
		}

	}

	if (m_NoRoot) {
		return nullptr;
	}

	if (m_RootScope != nullptr) {

		return m_RootScope->FindVar(name);

	}

}

void VScope::SetClass(VClass* cls) {
	
	m_Owner = cls;

}

VFunction* VScope::FindFunc(std::string name) {

	auto func = m_Owner->FindFunction(name);
	if (func == nullptr) {
		if (m_NoRoot == false)
		{
			func = m_RootScope->FindFunc(name);
		}
	}
	return func;
	//return nullptr;
}

void VScope::SetContext(VContext* context) {

	m_Context = context;

}