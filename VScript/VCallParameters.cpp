#include "pch.h"
#include "VCallParameters.h"
#include "VExpression.h"

void VCallParameters::AddParam(VExpression* expression)
{

	m_Params.push_back(expression);

}

std::vector<VExpression*> VCallParameters::GetParams() {

	return m_Params;

}