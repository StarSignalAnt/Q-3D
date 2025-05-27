#pragma once
#include "VAction.h"
#include "VVar.h"
#include "VExpression.h"

class VVarAssign : public VAction
{
public:

	void SetTarget(VName name) {
		m_Name = name;
	}

	void SetExpression(VExpression* expr) {
		m_Expr = expr;
	}

	VVar* Exec();

private:

	VName m_Name;
	VExpression* m_Expr;

};

