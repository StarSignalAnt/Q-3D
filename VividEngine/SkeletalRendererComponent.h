#pragma once
#include "Component.h"
class SkeletalRendererComponent :
    public Component
{
public:

    void OnRender(GraphNode* camera) override;
};

