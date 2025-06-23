#pragma once
#include "LGDataNode.h"

class GraphNode;

class NodeExposeGraphNode : public LGDataNode
{
public:

	NodeExposeGraphNode();
    
    void CalculateOutputs() override;

    
};

