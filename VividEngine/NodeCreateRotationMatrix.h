#pragma once
#include "LGDataNode.h"
class NodeCreateRotationMatrix :
    public LGDataNode
{
public:
    NodeCreateRotationMatrix();
    std::string GetCategory() const override { return "Maths"; }
    void CalculateOutputs() override;
};

