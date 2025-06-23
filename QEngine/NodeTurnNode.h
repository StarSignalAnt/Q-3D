#pragma once
#include "LGLogicNode.h"

class NodeTurnNode :
    public LGLogicNode
{
public:
    NodeTurnNode();
    std::string GetCategory() const override { return "Node"; }
    void Exec() override;
};