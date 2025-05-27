#include "pch.h"
#include "VWhile.h"
#include "VExpression.h"
#include "VCodeBody.h"

void VWhile::SetExpression(VExpression* expression) {

	m_Expression = expression;

}

void VWhile::SetCode(VCodeBody* code) {

	m_Code = code;

}

VVar* VWhile::Exec() {

	m_Expression->m_Context = GetContext();
	while (true) {
		if (m_Expression->Express()->ToInt() == 1)
		{
			m_Code->SetContext(GetContext());
			m_Code->Exec();

		}
		else {
			break;
		}
	}

	return nullptr;
}