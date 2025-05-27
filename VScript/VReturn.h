#pragma once
#include "VAction.h"

class VExpression;

class VReturn :
    public VAction
{
public:

    void SetExpression(VExpression* expression);
    VExpression* GetExpression();
    VVar* Exec();

private:

    VExpression* m_Expression;

};

