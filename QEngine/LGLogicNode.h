#pragma once
#include "LNode.h"
class LGLogicNode :
    public LNode
{
public:

    LGLogicNode() {
        m_hasExecIn = true;
        m_hasExecOut = true;
    }

    virtual void Exec() = 0;
    
private:
};

