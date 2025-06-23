#pragma once
#include "LGLogicNode.h"
class NodeStartVideo :
    public LGLogicNode
{
public:


    NodeStartVideo();
    std::string GetCategory() const override { return "Media"; }
    void Exec() override;

};

