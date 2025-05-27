#include "pch.h"
#include "VVarAssign.h"
#include "VContext.h"


VVar* VVarAssign::Exec() {

	auto con = GetContext();

	m_Expr->m_Context = con;


	auto fv =con->FindVar(m_Name.GetNames()[0]);
	//%%%%%%

	switch (fv->GetType()) {
	case T_Number:
	case T_Int:
		fv->SetInt(m_Expr->Express()->ToInt());
		break;
	case T_FloatNumber:
	case T_Float:
		fv->SetFloat(m_Expr->Express()->ToFloat());
		break;
	case T_Class:
		fv->SetClassValue(m_Expr->Express()->GetClassValue());
		break;
	default:

		int bb = 5;

		break;
	}

	//fv->m_IntValue = m_Expr->Express()->m_IntValue;




	return nullptr;
}