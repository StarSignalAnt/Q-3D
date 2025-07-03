#pragma once
#include "Component.h"
class StaticDepthRendererComponent :
    public Component
{
public:


    void OnRenderDepth(GraphNode* camera) override;
    //void OnRenderDirect(GraphNode* camera)

private:

};

