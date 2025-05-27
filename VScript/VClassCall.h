#pragma once
#include "VAction.h"
#include "VName.h"

class VCallParameters;

class VClassCall :
    public VAction
{
public:

    void SetName(VName name) {
        m_Name = name;
    }

    void SetParams(VCallParameters* params)
    {
        m_Params = params;
    }

    VVar* Exec();

private:


    VName m_Name;
    VCallParameters* m_Params;

};

