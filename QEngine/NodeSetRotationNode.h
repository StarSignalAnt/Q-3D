#pragma once
#include "LGLogicNode.h"
class NodeSetRotationNode :
    public LGLogicNode
{
public:
    NodeSetRotationNode();
    std::string GetCategory() const override { return "Node"; }
    void Exec() override;
};

