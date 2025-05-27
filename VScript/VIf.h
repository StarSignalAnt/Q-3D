#pragma once
#include "VAction.h"

class VExpression;
class VCodeBody;

class VIf :
    public VAction
{
public:

    void SetIfExp(VExpression* expression);
    void SetBody(VCodeBody* body);
    void SetElseBody(VCodeBody* body);
    void SetElseIf(VIf* eif);
    VVar* Exec();

private:

    VExpression* m_IfExp;
    VCodeBody* m_Code;
    VCodeBody* m_ElseCode;
    VIf* m_ElseIf;

};

