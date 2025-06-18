#pragma once
#include "LGLogicNode.h"
class NodeSetPositionNode :
    public LGLogicNode
{
public:
    NodeSetPositionNode();
    std::string GetCategory() const override { return "Node"; }
    void Exec() override;
};

