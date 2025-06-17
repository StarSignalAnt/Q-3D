#pragma once
#include "LEventNode.h"
#include <string> // Include for std::string

class TickEventNode :
    public LEventNode
{
public:
    TickEventNode();

    void FireEvent() override;
    std::string GetCategory() const override { return "Events"; }
};