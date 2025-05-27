#pragma once
#include "VAction.h"
#include "VToken.h"
#include "VName.h"
class VExpression;


class VDeclareArray :
    public VAction
{
public:

    void SetType(TokenType type);
    void SetSizeExpression(VExpression* exp);
    void SetName(VName name);
    VVar* Exec();


private:

    VName m_Name;
    VExpression* m_SizeExp;
    TokenType m_Type;

};

