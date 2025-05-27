#include "pch.h"
#include "VFunction.h"
#include "VCodeBody.h"
#include "VClass.h"
#include "VContext.h"
#include "VDefineParams.h"
#include "VCallParameters.h"
#include "VExpression.h"

VFunction::VFunction() {

}

void VFunction::SetName(VName name) {

	m_Name = name;

}

VName VFunction::GetName() {

	return m_Name;

}

void VFunction::SetParams(VDefineParams* params) {

	m_Params = params;

}

VDefineParams* VFunction::GetParams() {

	return m_Params;

}

void VFunction::SetContext(VContext* context) {

	m_Context = context;
	m_Code->SetContext(m_Context);

}

VFunction* VFunction::Clone() {

	VFunction* clone = new VFunction;
	clone->SetName(m_Name);
	clone->SetParams(m_Params);
	clone->SetBody(m_Code);
	clone->SetContext(m_Context);
	clone->SetGuard(m_Guard);
	return clone;

}


VVar* VFunction::Call(const std::vector<VVar*>& args)
{
	m_Context->PushScope(m_ClassOwner->GetScope());

	if (args.size()>0) {

		auto params = new VCallParameters;

		for (auto arg : args) {

			VExpression* e = new VExpression;
			ExpElement ele;
			ele.EleType = arg->GetType();
			
			VName name;
			name.Add(arg->GetName());
			ele.VarName = name;
			ele.FloatValue = arg->ToFloat();

			e->Elements.push_back(ele);
			
			params->AddParam(e);

		}


		m_Context->PushScope(GetScope(), params);
	}
	else {
		m_Context->PushScope(GetScope());
	}

	m_Code->SetContext(m_Context);
	VVar* res = nullptr;
	if (m_Guard != nullptr) {
		m_Guard->m_Context = m_Context;
		if (m_Guard->Express()->ToInt() == 1) {
			res = m_Code->Exec();
		}
	}
	else {
		res = m_Code->Exec();
	}


	m_Context->PopScope();

	m_Context->PopScope();

	return res;

}

VVar* VFunction::Call(VCallParameters* params) {

	bool pushed = false;

	if (!m_Context->HasScope(m_ClassOwner->GetScope())) {
		m_Context->PushScope(m_ClassOwner->GetScope());
		pushed = true;
	}

	if (params != nullptr) {
		m_Context->PushScope(GetScope(), params);
	}
	else {
		m_Context->PushScope(GetScope());
	}
	
	m_Code->SetContext(m_Context);
	VVar* res = nullptr;
	if (m_Guard != nullptr) {
		m_Guard->m_Context = m_Context;
		if (m_Guard->Express()->ToInt() == 1) {
			res = m_Code->Exec();
		}
	}
	else {
		res = m_Code->Exec();
	}


	m_Context->PopScope();

	if (pushed) {
		m_Context->PopScope();
	}
	return res;
}

void VFunction::SetBody(VCodeBody* code) {

	m_Code = code;

}

void VFunction::SetOwner(VClass* cls) {

	m_ClassOwner = cls;

}

VScope* VFunction::GetScope() {

	auto r = new VScope;

	auto params = GetParams();

	if (params != nullptr)
	{

		if (params->GetParams().size() > 0) {

			auto pp = params->GetParams();

			for (auto p : pp) {

				VVar* f_var = new VVar;
				f_var->SetName(p->GetName().GetNames()[0]);
				f_var->SetType(p->GetType());
				f_var->SetClassType(p->GetClassType());
				auto pv = m_Context->FindVar(p->GetName().GetNames()[0]);
				f_var->SetClassValue(nullptr);
				if (pv != nullptr) {
					f_var->SetClassValue(pv->GetClassValue());
				}

				r->RegisterVar(f_var);

			}
		}
	}

	r->SetClass(m_ClassOwner);
	return r;
	


}

void VFunction::SetGuard(VExpression* expression) {

	m_Guard = expression;

}