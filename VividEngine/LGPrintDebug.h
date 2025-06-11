#pragma once
#include "LGNode.h"
class LGPrintDebug :
    public LGNode
{
public:

    LGPrintDebug();
    void Exec() override;
};

