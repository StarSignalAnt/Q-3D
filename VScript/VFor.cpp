#include "pch.h"
#include "VFor.h"
#include "VAssignArray.h"
#include "VExpression.h"
#include "VCodeBody.h"
#include "VVar.h"
#include "VVarAssign.h"

void VFor::SetInitial(VVarAssign* init)
{

	m_Initial = init;

}

void VFor::SetExpression(VExpression* ex)
{
	m_Exp = ex;
}

void VFor::SetInc(VVarAssign* inc)
{

	m_Inc = inc;

}

void VFor::SetCode(VCodeBody* body) {

	m_Code = body;

}

VVar* VFor::Exec() {

	m_Initial->SetContext(GetContext());
	m_Initial->Exec();

	while (true) {

		m_Exp->m_Context = GetContext();
		if (m_Exp->Express()->ToInt() == 1)
		{
			m_Code->SetContext(GetContext());
			m_Code->Exec();
			m_Inc->SetContext(GetContext());
			m_Inc->Exec();
		}
		else {
			break;
		}

	}


	return nullptr;
}