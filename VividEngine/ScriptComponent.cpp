#include "ScriptComponent.h"
#include "ScriptHost.h"
#include <iostream>

void ScriptComponent::SetScript(std::string path, std::string name) {

	ScriptHost::m_Host->Load(path);
	m_ComponentPy = ScriptHost::m_Host->CreateInstance(name);
	

	void* o = (void*)m_Owner;
	//PyClass pyclass_obj(reinterpret_cast<uintptr_t>(o));
	Pars p(reinterpret_cast<uintptr_t>(o));


	ScriptHost::m_Host->callFunc(m_ComponentPy, "SetNode",p);

	m_Name = ScriptHost::m_Host->GetString(m_ComponentPy, "name");

	//ScriptHost::m_Host->callFunc(m_ComponentPy, "Update", Pars(0.1f));


	int b = 5;
	
	/*
	* 
	* 
	void* mem = malloc(512);

	int* m2 = (int*)mem;
	m2[0] = 24;
	m2[1] = 25;

	uintptr_t ptr_int = reinterpret_cast<uintptr_t>(mem);

	ScriptHost::m_Host->callFunc(m_ComponentPy, "SetClass", Pars());

	auto p2 = ScriptHost::m_Host->CreateInstance(name);

	auto vals = ScriptHost::m_Host->GetVarNames(p2);
	for (const auto& [name, type] : vals) {
		std::cout << "Variable: " << name << " | Type: " << VarTypeToString(type) << std::endl;
	}


//	ScriptHost::m_Host->callFunc(p2, "SetClass", Pars());

//	auto cls = ScriptHost::m_Host->GetClass(m_ComponentPy, "c1");


//	ScriptHost::m_Host->SetInt(cls, "other", 55);

//	ScriptHost::m_Host->SetClass(p2, "c1", cls);
//	ScriptHost::m_Host->callFunc(p2, "Check", Pars());

	//ScriptHost::m_Host->callFunc(m_ComponentPy, "Setup", Pars(1.5f,false,200,ptr_int));

	//void* nmem = (void*)ScriptHost::m_Host->GetIntPtr(m_ComponentPy, "cptr");
	//std::string s1 = ScriptHost::m_Host->GetString(m_ComponentPy,"test");
	//void* oc = ScriptHost::m_Host->GetClass(m_ComponentPy, "check");
	//int ov = ScriptHost::m_Host->GetInt(oc, "other");

	//Pars p(PyClass(reinterpret_cast<uintptr_t>(oc)));

//	ScriptHost::m_Host->SetFloat(m_ComponentPy, "intC", 500.5);
//	ScriptHost::m_Host->callFunc(m_ComponentPy, "SetCls",p);
//	ScriptHost::m_Host->SetPtr(m_ComponentPy, "vval", ptr_int);

//	void* m5 = (void*)ScriptHost::m_Host->GetIntPtr(m_ComponentPy, "vval");


//	float val = ScriptHost::m_Host->GetFloat(m_ComponentPy, "fv");
//	bool b1 = ScriptHost::m_Host->GetBool(m_ComponentPy, "bv");

//	auto names = ScriptHost::m_Host->GetVarNames(m_ComponentPy);


	//int* m3 = (int*)m5;
//	printf("M1=%d\n", m3[0]);
//	printf("M2=%d\n", m3[1]);
	int a = 5;
	*/


}

void ScriptComponent::OnUpdate(float dt) {

	//printf("Updating SC");
	ScriptHost::m_Host->callFunc(m_ComponentPy, "Update", Pars(dt));

}

std::vector<ScriptVar> ScriptComponent::GetVars() {

	auto vals = ScriptHost::m_Host->GetVarNames(m_ComponentPy);

	std::vector<ScriptVar> res;

	for (const auto& [name, type] : vals) {
		//std::cout << "Variable: " << name << " | Type: " << VarTypeToString(type) << std::endl;
		ScriptVar var;
		var.name = name;
		switch (type) {
		case VarType::T_Int:
			var.type = SVarType::VT_Int;
			break;
		case VarType::T_Float:
			var.type = SVarType::VT_Float;
			break;
		case VarType::T_String:
			var.type = SVarType::VT_String;
			break;
		}
		res.push_back(var);
	}

	return res;


}

int ScriptComponent::GetInt(std::string name) {

	return ScriptHost::m_Host->GetInt(m_ComponentPy,name);

}

float ScriptComponent::GetFloat(std::string name) {

	return ScriptHost::m_Host->GetFloat(m_ComponentPy, name);

}

void ScriptComponent::SetFloat(std::string name, float value) {

	ScriptHost::m_Host->SetFloat(m_ComponentPy, name, value);

}

std::string ScriptComponent::GetString(std::string name) {

	return ScriptHost::m_Host->GetString(m_ComponentPy,name);

}

void ScriptComponent::SetString(std::string name, std::string value) {

	ScriptHost::m_Host->SetString(m_ComponentPy, name, value);

}