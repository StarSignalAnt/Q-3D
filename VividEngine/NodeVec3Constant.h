#pragma once
#include "LGDataNode.h"
class NodeVec3Constant :
    public LGDataNode
{
public:
    NodeVec3Constant();
    std::string GetCategory() const override { return "Constants"; }
};

