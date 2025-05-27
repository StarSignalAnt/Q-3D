#pragma once
#include "VAction.h"

class VExpression;
class VVar;
class VCodeBody;

class VWhile : public VAction
{
public:

	void SetExpression(VExpression* expression);
	void SetCode(VCodeBody* code);
	VVar* Exec();

private:

	VExpression* m_Expression;
	VCodeBody* m_Code;

};

