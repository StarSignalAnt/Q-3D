#pragma once
#include "VAction.h"
#include "VName.h"

class VVar;

class VInvoke :

    public VAction
{
public:

    void SetTarget(VName name) {
        m_Target = name;
    }

    VVar* Exec();

private:

    VName m_Target;

};

