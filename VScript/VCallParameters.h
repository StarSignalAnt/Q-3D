#pragma once
#include <vector>
#include <string>

class VExpression;

class VCallParameters
{
public:

	void AddParam(VExpression* expression);
	std::vector<VExpression*> GetParams();

private:

	std::vector<VExpression*> m_Params;

};

