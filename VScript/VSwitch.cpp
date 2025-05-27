#include "pch.h"
#include "VSwitch.h"
#include "VExpression.h"
#include "VVar.h"
#include "VCodeBody.h"
void VSwitch::SetCheck(VExpression* expr)
{

	m_Check = expr;

}

void VSwitch::AddValue(VCodeBody* code, VExpression* expr)
{

	m_Codes.push_back(code);
	m_Exprs.push_back(expr);

}

VVar* VSwitch::Exec() {

	int b = 5;
	m_Check->m_Context = GetContext();

	int check = m_Check->Express()->ToInt();

	int i = 0;
	for (auto ch : m_Exprs) {

		ch->m_Context = GetContext();
		auto rv = ch->Express()->ToInt();

		if (rv == check) {

			auto code = m_Codes[i];
			code->SetContext(GetContext());
			auto res = code->Exec();

			return res;

		}
		i++;
	}

	return nullptr;
}