#pragma once
#include "VAction.h"
#include "VName.h"

class VExpression;
class VVar;

class VClassAssign :
    public VAction
{
public:

    void SetName(VName name);
    void SetExpression(VExpression* expression);
    VVar* Exec();


private:

    VName m_Name;
    VExpression* m_Expression;

};

