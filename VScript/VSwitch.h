#pragma once
#include "VAction.h"
#include <vector>
#include <string>

class VExpression;
class VCodeBody;

class VSwitch :
    public VAction
{
public:

    void SetCheck(VExpression* expr);
    void AddValue(VCodeBody* code, VExpression* expr);
    VVar* Exec();
private:

    std::vector<VCodeBody*> m_Codes;
    std::vector<VExpression*> m_Exprs;
    VExpression* m_Check;

};

