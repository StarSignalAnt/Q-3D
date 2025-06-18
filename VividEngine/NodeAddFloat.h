#pragma once
#include "NodeAddBase.h"
#include <string>
class NodeAddFloat :
    public NodeAddBase<float>
{
public:

    NodeAddFloat();
    std::string GetCategory() const override { return "Maths"; }

};

