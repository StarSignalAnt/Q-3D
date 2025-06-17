#pragma once
#include "LGLogicNode.h"
class NodeIf :
    public LGLogicNode
{
public:

    NodeIf();
    std::string GetCategory() const override { return "Logic"; }
    void Exec() override;
};

