#pragma once
#include "LNode.h"
class LEventNode :
    public LNode
{
public:
    LEventNode() {
        m_hasExecIn = false;
        m_hasExecOut = true;
        AddExecOutput("Exec");
    }

    // Event nodes are started by the engine calling this method.
    virtual void FireEvent()
    {
        // Get the node connected to this event's "Exec Out" pin.
        ExecNext(0);
    }

    // Override Exec since events are the start of a chain.
    void Exec() override {
        FireEvent();
    }
};

