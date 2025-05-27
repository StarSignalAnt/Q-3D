#include "pch.h"
#include "VClassCall.h"
#include "VVar.h"
#include "VCallParameters.h"
#include "VContext.h"
#include "VFunction.h"

VVar* VClassCall::Exec() {

	int a = 5;
	auto cv = GetContext()->FindVar(m_Name.GetNames());

	for (auto p : m_Params->GetParams()) {

		p->m_Context = GetContext();

	}

	if (cv->GetClassType() == "List")
	{
		std::string meth = m_Name.GetNames()[1];

		if (meth == "Add")
		{

			auto val = m_Params->GetParams()[0];

			cv->ListAdd(val->Express());

			return nullptr;

		}

		int b = 5;
	}

	auto func = cv->GetClassValue()->FindFunction(m_Name.GetNames()[m_Name.GetNames().size() - 1]);
	
	if (func == nullptr) {
		printf("Func:");
		printf(m_Name.GetNames()[m_Name.GetNames().size() - 1].c_str());
		printf(" not found.\n");
		exit(-1);
	}


	func->SetContext(GetContext());

	return func->Call(m_Params);

	return nullptr;

}