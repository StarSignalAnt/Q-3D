#include "pch.h"
#include "VClass.h"
#include "VFunction.h"
#include "VScope.h"
#include "VVar.h"
#include "VName.h"
#include "VExpression.h"
#include "VDefineParams.h"
#include "VParameter.h"
#include "VContext.h"
#include "VScope.h"


void VClass::SetName(VName name) {

	m_Name = name;

}

void VClass::AddVarGroup(VVarGroup* group) {

	m_Groups.push_back(group);

}

void VClass::AddFunction(VFunction* func)
{

	m_Funcs.push_back(func);
	func->SetOwner(this);

}



VFunction* VClass::FindFunction(VName name) {

	for (auto func : m_Funcs) {

		if (func->GetName().SameName(name)) {
			return func;
		}

	}
	return nullptr;

}

void VClass::SetContext(VContext* context) {

	m_Context = context;

	for (auto func : m_Funcs) {

		func->SetContext(context);

	}

}

VClass* VClass::Clone() {

	VClass* clone = new VClass;
	clone->SetName(m_Name);
	clone->SetSubClass(m_SubClass);
	for (auto func : m_Funcs) {

		auto f2 = func->Clone();
		f2->SetOwner(clone);
		clone->AddFunction(f2);

	}

	for (auto func : m_StaticFuncs) {
		auto f3 = func->Clone();
		f3->SetOwner(clone);
		clone->AddStaticFunction(f3);
	}

	for (auto g : m_Groups)
	{
		clone->AddVarGroup(g);
	}
	for (auto sg : m_StaticGroups) {
		clone->AddStaticVarGroup(sg);
	}

	return clone;

}

VName VClass::GetName() {

	return m_Name;

}

VFunction* VClass::FindFunction(std::string name) {

	for (auto func : m_Funcs) {

		if (func->GetName().GetNames()[0] == name)
		{
			return func;
		}

	}

	return nullptr;

}

void VClass::CreateScope() {

	m_InstanceScope = new VScope;
	m_InstanceScope->NoRoot();

	std::vector<VVarGroup*> groups = m_Groups;

	if (m_Static) {
		groups = m_StaticGroups;
	}

	for (auto g : groups) {

		if (g->GetArray()) {
			auto con = m_Context;

		//	auto top_scope = con->GetTopScope();

			auto nv = new VVar;
			//	nv->m_Type = m_Type;
			//	nv->m_Name = m_Name.GetNames()[0];
			nv->SetType(g->GetType());
			
			auto names = g->GetNames()[0];

			nv->SetName(names.GetNames()[0]);



			int size = g->GetSizeExpression()->Express()->ToInt();

			nv->CreateArray(size);

			for (int i = 0; i < size; i++) {
				VVar* ave = new VVar;
				//	ave->m_Type = m_Type;
				ave->SetType(g->GetType());
				nv->SetArray(i, ave);

				//nv->m_ArrValues.push_back(ave);

			}

			 m_InstanceScope->RegisterVar(nv);
			return;
			int b = 5;
		}

		int ii = 0;
		if (g->GetType() != T_Ident) {
			for (auto v : g->GetNames())
			{
				auto nv = new VVar;

				nv->SetName(v.GetNames()[0]);

				//nv->m_Name = v.GetNames()[0];

				if (g->GetExpressions()[ii] != nullptr) {
					g->GetExpressions()[ii]->m_Context = this->m_Context;
					auto res = g->GetExpressions()[ii]->Express();

					nv->SetC(res->ToC());

					switch (g->GetType()) {
					case T_Number:
					case T_Int:
						
						nv->SetInt(res->ToInt());
						//nv->m_IntValue = res->ToInt();
						break;
					case T_Float:
					case T_FloatNumber:
						nv->SetFloat(res->ToFloat());
						//nv->m_FloatValue = res->ToFloat();
						break;
					case T_Class:


						int c = 5;
						break;
					}

				}
				//nv->m_Type = g->GetType();

				nv->SetType(g->GetType());
				m_InstanceScope->RegisterVar(nv);
				ii++;

				//nv->m_IntValue = 
			}
		}
		else {
			for (auto v : g->GetNames())
			{
				auto nv = new VVar;
				//nv->m_Name = v.GetNames()[0];

				nv->SetName(v.GetNames()[0]);

			//	if (g->GetExpressions()[ii] != nullptr) {
			//		g->GetExpressions()[ii]->m_Context = this->m_Context;
			//		nv->m_IntValue = g->GetExpressions()[ii]->Express()->m_IntValue;
			//	}

				nv->SetType(T_Class);
				nv->SetClassType(g->GetClassType());

				//nv->m_Type = T_Class;
				//nv->m_ClassType = g->GetClassType();
				if (g->GetExpressions()[ii] != nullptr) {
					g->GetExpressions()[ii]->m_Context = this->m_Context;
					nv->SetClassValue(g->GetExpressions()[ii]->Express()->GetClassValue());

				}
				m_InstanceScope->RegisterVar(nv);

				ii++;

				//nv->m_IntValue = 
			}
		}

	}


	m_InstanceScope->SetClass(this);
	m_InstanceScope->NoRoot();

}

VVar* VClass::FindVar(std::string name) {

	
	for (auto v : m_InstanceScope->GetVars()) {
		if (v->IsName(name)) {
			return v;
		}
	}
	return nullptr;
}

VFunction* VClass::FindFunctionBySig2(std::string name, std::vector<std::string> sig)
{
	for (auto func : m_Funcs) {

		if (func->GetName().GetNames()[0] == name)
		{

			if (sig.size() == 0 && func->GetParams() == nullptr) {
				return func;
			}
			auto params = func->GetParams();
			auto pl = params->GetParams();

			bool right = true;
			int si = 0;
			for (auto p : pl) {

				if (p->GetClassType() == sig[si])
				{
				//	if (sig[si] == T_Number)
					{

					}
					//else {
					//	return nullptr;
				//	}
				}
				else {
					right = false;
					break;
				}

			}

			if (right) {
				return func;
			}
			int b = 5;
			//		return func;
		}

	}
	return nullptr;
}

VFunction* VClass::FindFunctionBySig(std::string name,std::vector<TokenType> sig) {

	
	for (auto func : m_Funcs) {

		if (func->GetName().GetNames()[0] == name)
		{

			if (sig.size() == 0 && func->GetParams() == nullptr) {
				return func;
			}
			auto params = func->GetParams();
			auto pl = params->GetParams();

			int si = 0;
			for (auto p : pl) {

				if (p->GetType() == T_Int)
				{
					if (sig[si] == T_Number)
					{

					}
					else {
						return nullptr;
					}
				}

			}
			
			return func;
			int b = 5;
			//		return func;
		}

	}
	return nullptr;
}

void VClass::AddStaticVarGroup(VVarGroup* group) {

	m_StaticGroups.push_back(group);

}

void VClass::SetSubClass(std::string cls) {

	m_SubClass = cls;

}

std::string VClass::GetSubClass() {

	return m_SubClass;

}

std::vector<VFunction*> VClass::GetFunctions() {

	return m_Funcs;

}

void VClass::WriteClass(VFile* file) {


//	file->WriteInt(m_InstanceScope->GetVars().size());

	for (auto v : m_InstanceScope->GetVars()) {



	}

}