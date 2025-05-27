#pragma once
#include "VAction.h"
#include "VName.h"

class VExpression;
class VVar;

class VAssignArray :
    public VAction
{
public:

    void SetTarget(VName name);
    void SetIndexExpression(VExpression* expression);
    void SetExpression(VExpression* expression);
    VVar* Exec();

private:

    VName m_Target;
    VExpression* m_IndexExpression;
    VExpression* m_Expression;


};

