#pragma once
#include "VAction.h"

class VVarAssign;
class VExpression;
class VVarAssign;
class VVar;
class VCodeBody;

class VFor :
    public VAction
{
public:

    void SetInitial(VVarAssign* init);
    void SetExpression(VExpression* exp);
    void SetInc(VVarAssign* ass);
    void SetCode(VCodeBody* code);
    VVar* Exec();

private:

    VVarAssign* m_Initial;
    VExpression* m_Exp;
    VVarAssign* m_Inc;
    VCodeBody* m_Code;


};

