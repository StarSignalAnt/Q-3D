#pragma once
#include "LGLogicNode.h"
class NodeSceneRayCast :
    public LGLogicNode
{
public:
    NodeSceneRayCast();
    std::string GetCategory() const override { return "Interaction"; }
    void Exec() override;
    void CalculateOutputs() override;
private:
};

