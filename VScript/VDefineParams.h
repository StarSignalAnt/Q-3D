#pragma once
#include "VVarGroup.h"
#include <vector>
#include "VParameter.h"

class VDefineParams
{
public:

	void AddParam(VParameter* param);
	std::vector<VParameter*> GetParams();

private:

	std::vector<VParameter*> m_Pars;

};

