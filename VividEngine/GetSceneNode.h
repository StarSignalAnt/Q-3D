#pragma once
#include "LGDataNode.h"
class GetSceneNode :
    public LGDataNode
{
public:

    GetSceneNode();
    std::string GetCategory() const override { return "Scene"; }

    // Overriding Exec to implement the logic of finding the node.
    void CalculateOutputs() override;

private:
};

