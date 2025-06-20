#pragma once
#include "LEventNode.h"

class PlayEventNode : public LEventNode
{
public:
        PlayEventNode();

        void FireEvent() override;
        std::string GetCategory() const override { return "Events"; }
};

