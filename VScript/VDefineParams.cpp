#include "pch.h"
#include "VDefineParams.h"

void VDefineParams::AddParam(VParameter* param) {

	m_Pars.push_back(param);

}

std::vector<VParameter*> VDefineParams::GetParams() {

	return m_Pars;

}