#include "pch.h"
#include "VContext.h"
#include "VScope.h"
#include "VFunction.h"
#include "VEnum.h"

VContext::VContext() {

	VScope* static_scope = new VScope;

	m_StaticScope = static_scope;

}

void VContext::AddModule(VModule* module) {

	auto mod = module->Clone();

	m_Modules.push_back(mod);
	for (auto cls : mod->GetClasses()) {

		VVar* sv = new VVar;
		cls->SetStatic(true);
		sv->SetName(cls->GetName().GetNames()[0]);
		sv->SetClassValue(cls);
		cls->SetContext(this);
		cls->CreateScope();
		m_StaticScope->RegisterVar(sv);
		for (auto func : cls->GetStaticFuncs()) {
			cls->AddFunction(func);
		}


	}

	for (auto en : module->GetEnums()) {
		m_Enums.push_back(en);
	}

	//PushScope(m_StaticScope);




}

VClass* VContext::CreateInstance(std::string name) {

	for (auto mod : m_Modules) {

		auto c_class = mod->FindClass(name);
		if (c_class != nullptr) {
			auto i_class = c_class->Clone();
			i_class->SetContext(this);
			i_class->CreateScope();
			if (i_class->GetSubClass() != "")
			{

				auto ih = CreateInstance(i_class->GetSubClass());
				auto ic = ih->GetScope();
				auto vars = ic->GetVars();
				for (auto v : vars) {
					i_class->GetScope()->RegisterVar(v);
				}
				for (auto f : ih->GetFunctions()) {
					i_class->AddFunction(f);
				}

			}
			i_class->FindFunction(name)->Call(nullptr);
			return i_class;
		}

	}

	return nullptr;

}

VVar* VContext::FindVar(std::vector<std::string> names) {

	if (this == nullptr) {
		printf("Context not assigned.\n");
		exit(1);
	}
	if (names.size() == 1)
	{
		return FindVar(names[0]);
	}
	else {

		int b = 5;

		auto en = FindEnum(names[0]);
	
		if (en != nullptr) {

			auto val = en->GetValue(names[1]);

			auto var = new VVar;
			var->SetName(names[0]);
			var->SetType(T_Int);
			var->SetInt(val);

			return var;



		}
		

		auto check = FindVar(names[0]);

		if (check == nullptr) {

			check = m_StaticScope->FindVar(names[0]);

		}

		if (check->GetClassType() == "List"){
			return check;
		}

		VVar* cur = check;
		if (cur->GetClassValue() == nullptr) {
			printf("Runtime error:");
			//printf(names[0].c_str());
			for (int i = 0; i < names.size();i++) {
				printf(names[i].c_str());
				printf(".");
			}
			printf(" is null.\n");
			
			exit(1);

		}
		int ii = 1;
		while (true) {
			
			if (cur->GetClassValue() != nullptr) {
				auto ncur = cur->GetClassValue()->FindVar(names[ii]);
				if (ncur != nullptr) {
					cur = ncur;
				}
				else {
					return cur;
				}

			}

			ii++;
			if (ii >= names.size()) {
				return cur;
			}
		}

	}

}

VVar* VContext::FindVar(std::string name) {

	m_ScopeStack.top()->SetContext(this);
	auto var = m_ScopeStack.top()->FindVar(name);
	if (var != nullptr) {
		return var;
	}
	return nullptr;
}

VEnum* VContext::FindEnum(std::string name) {

	for (auto en : m_Enums)
	{
		if (en->GetName() == name) {
			return en;
		}
	}
	return nullptr;

}