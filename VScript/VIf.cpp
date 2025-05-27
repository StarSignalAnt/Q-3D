#include "pch.h"
#include "VIf.h"
#include "VExpression.h"
#include "VCodeBody.h"

void VIf::SetIfExp(VExpression* expression) {

	m_IfExp = expression;

}

void VIf::SetBody(VCodeBody* body) {

	m_Code = body;

}

void VIf::SetElseBody(VCodeBody* else_body) {

	m_ElseCode = else_body;

}

void VIf::SetElseIf(VIf* else_if)
{

	m_ElseIf = else_if;
}

VVar* VIf::Exec() {

	m_IfExp->m_Context = GetContext();

	if (m_IfExp->Express()->ToInt() == 1)
	{
		m_Code->SetContext(GetContext());
		auto res = m_Code->Exec();
		if (res != nullptr) {
			return res;
		}


	}
	else {

		if (m_ElseIf != nullptr) {


			m_ElseIf->SetContext(GetContext());
			auto res = m_ElseIf->Exec();
			if (res != nullptr) {
				return res;
			}


		}
		else {

			if (m_ElseCode != nullptr) {
				m_ElseCode->SetContext(GetContext());
				auto res = m_ElseCode->Exec();
				if (res != nullptr) {
					return res;
				}
			}
		}

	}

	return nullptr;
}