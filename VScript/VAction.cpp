#include "pch.h"
#include "VAction.h"


VContext* VAction::GetContext() {

	return m_Context;

}

void VAction::SetContext(VContext* context) {

	m_Context = context;

}