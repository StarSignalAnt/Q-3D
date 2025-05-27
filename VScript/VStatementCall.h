#pragma once
#include "VAction.h"
#include <string>

class VVar;
class VCallParameters;

class VStatementCall :
    public VAction
{
public:

    void SetName(std::string name) {
        m_Name = name;
    }
    void SetCallParameters(VCallParameters* params);
    VCallParameters* GetCallParameters();

    VVar* Exec();

private:

    std::string m_Name;
    VCallParameters* m_CallParams;
   

};

