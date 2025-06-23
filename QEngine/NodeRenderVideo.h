#pragma once
#include "LGLogicNode.h"
class NodeRenderVideo :
    public LGLogicNode
{
public:
    NodeRenderVideo();
    std::string GetCategory() const override { return "Media"; }
    void Exec() override;
};

