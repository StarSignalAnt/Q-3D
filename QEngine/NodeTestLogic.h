#pragma once
#include "LGLogicNode.h"

class NodeTestLogic :
    public LGLogicNode
{
public:
    NodeTestLogic();
    std::string GetCategory() const override { return "Logic"; }

    void Exec() override;
};