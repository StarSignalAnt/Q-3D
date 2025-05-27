#include "pch.h"
#include "VExpression.h"
#include "VReturn.h"
#include "VVar.h"

void VReturn::SetExpression(VExpression* expression) {

	m_Expression = expression;

}

VExpression* VReturn::GetExpression() {

	return m_Expression;

}

VVar* VReturn::Exec() {

	if (m_Expression == nullptr) {

		auto v = new VVar;
		v->SetType(T_Int);
		v->SetInt(0);
		return v;

	}
	m_Expression->m_Context = GetContext();
	return m_Expression->Express();

	//return nullptr;

}