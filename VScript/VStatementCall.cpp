#include "pch.h"
#include "VStatementCall.h"
#include "VContext.h"
#include "VFunction.h"

VVar* VStatementCall::Exec() {

	int b = 5;

	auto direct = GetContext()->FindCFunc(m_Name);

	if (direct != nullptr) {

		if (m_CallParams != nullptr) {
			std::vector<VVar*> vars;


			for (auto p : m_CallParams->GetParams()) {

				p->m_Context = GetContext();
				VVar* v = new VVar;
				p->m_Context = GetContext();
				auto res = p->Express();

				//v->m_IntValue = p->Express()->m_IntValue;
				//v->m_FloatValue = p->Express()->m_FloatValue;

				vars.push_back(res);
			}

			auto res = direct(vars );
			return res;
		}
		else {

			auto res = direct({});
			return res;
		}
	}
	auto context = GetContext();

	auto func = context->FindFunc(m_Name);



	auto res = func->Call(m_CallParams);

	int ba = 5;
	return res;



}

void VStatementCall::SetCallParameters(VCallParameters* parameters) {

	m_CallParams = parameters;

}

VCallParameters* VStatementCall::GetCallParameters() {

	return m_CallParams;

}