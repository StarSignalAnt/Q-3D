#include "pch.h"
#include "VCodeBody.h"
#include "VExpression.h"
#include "VBreak.h"

void VCodeBody::AddCode(VAction* action) {

	m_Code.push_back(action);

}

VVar* VCodeBody::Exec() {

	for (auto code : m_Code) {

		if (dynamic_cast<VBreak*>(code)) {

			return nullptr;
		}

		VVar* res = nullptr;
		//code->SetContext(GetContext());
		if (code->GetGuard() != nullptr) {
			auto guard = code->GetGuard();
			guard->m_Context = code->GetContext();
			if (guard->Express()->ToInt() == 1) {
				res = code->Exec();
			}

		}
		else {
			res = code->Exec();

		}
		if (res != nullptr) {
			return res;
		}

	}
	return nullptr;
}

void VCodeBody::SetContext(VContext* context) {

	
	m_Context = context;
	for (auto code : m_Code) {
		
		code->SetContext(context);

	}

}