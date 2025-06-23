#pragma once
#include "LEventNode.h"
class RenderEventNode :
    public LEventNode
{
public:
    RenderEventNode();

    void FireEvent() override;
    std::string GetCategory() const override { return "Events"; }
};

