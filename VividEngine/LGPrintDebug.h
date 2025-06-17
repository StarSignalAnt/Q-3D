#pragma once
#include "LNode.h"
class LGPrintDebug :
    public LNode
{
public:

    LGPrintDebug();
    void Exec() override;
};

