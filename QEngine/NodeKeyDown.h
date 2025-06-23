#pragma once
#include "LGDataNode.h"
class NodeKeyDown :
    public LGDataNode
{
public:

    NodeKeyDown();
    std::string GetCategory() const override { return "Input"; }
    void CalculateOutputs() override;
private:

};

