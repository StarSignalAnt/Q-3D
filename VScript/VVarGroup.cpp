#include "pch.h"
#include "VVarGroup.h"
#include "VContext.h"
#include "VName.h"
#include "VLambda.h"

VVarGroup::VVarGroup(TokenType type) {

	m_Type = type;

}

void VVarGroup::AddName(VName name,VExpression* expr) {

	m_Names.push_back(name);
	m_Defaults.push_back(expr);

}

VVar* VVarGroup::Exec() {

	//int a = 5;
	auto con = GetContext();

	auto top_scope = con->GetTopScope();
	
	int idx = 0;
	for (auto name : m_Names) {
		VVar* nv = new VVar;
		//nv->m_Type = TokenType::T_Class;
		nv->SetType(m_Type);
		nv->SetClassType(m_ClassType);
		nv->SetName(name.GetNames()[0]);
		nv->SetLambda(m_Lambda);
		
		if (m_Lambda != nullptr) {
			m_Lambda->SetScope(GetContext()->GetTopScope());
		}
		auto def = m_Defaults[idx];


		if (def != nullptr) {

			def->m_Context = GetContext();
			VVar* res = def->Express();
		//	nv->Transfer(res);
			nv->SetInt(res->ToInt());
			nv->SetFloat(res->ToFloat());
			nv->SetString(res->ToString());
			nv->SetArrayData(res->GetArrayData());
			nv->SetLambda(res->GetLambda());
			nv->SetClassType(res->GetClassType());
			nv->SetClassValue(res->GetClassValue());
			nv->SetC(res->ToC());
			//nv->SetName(name.GetNames()[0]);
			//nv->m_ClsValue = res->m_ClsValue;
		//	nv->m_IntValue = res->m_IntValue;
			//nv->m_FloatValue = res->m_FloatValue;
			//nv->m_ClassType = res->m_ClassType;
		//	nv->m_StrValue = res->m_StrValue;
		//	nv->m_Type = res->m_Type;
			//if (nv->m_ClsValue != nullptr) {
		//		nv->m_Type = TokenType::T_Class;
			//}
			if (nv->GetClassValue() != nullptr) {
				nv->SetType(TokenType::T_Class);
			}


		}

		top_scope->RegisterVar(nv);

		idx++;
	}

	int a = 5;

	return nullptr;
}

void VVarGroup::SetSizeExpression(VExpression* expression) {

	m_SizeExpression = expression;

}