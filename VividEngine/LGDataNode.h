#pragma once
#include "LNode.h"

// Base class for all data-only nodes (e.g., Float, Vector, Get Actor Location, etc.)
class LGDataNode : public LNode
{
public:
    LGDataNode();
    void Exec() override;
};