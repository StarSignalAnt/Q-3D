
#include "VAssignArray.h"
#include "VExpression.h"
#include "VName.h"
#include "VVar.h"
#include "VContext.h"

void VAssignArray::SetTarget(VName name) {

	m_Target = name;

}

void VAssignArray::SetIndexExpression(VExpression* expression)
{

	m_IndexExpression = expression;

}

void VAssignArray::SetExpression(VExpression* expression) {

	m_Expression = expression;

}

VVar* VAssignArray::Exec() {

	auto con = GetContext();

	m_IndexExpression->m_Context = con;
	m_Expression->m_Context = con;

	int index = m_IndexExpression->Express()->ToInt();

	auto fv = con->FindVar(m_Target.GetNames()[0]);

	fv->SetArray(index, m_Expression->Express());


	int b = 5;

	return nullptr;

}