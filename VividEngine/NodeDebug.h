#pragma once
#include "LGLogicNode.h"
class NodeDebug :
    public LGLogicNode
{
public:
    NodeDebug();
    std::string GetCategory() const override { return "Debug"; }
    void Exec() override;
};

