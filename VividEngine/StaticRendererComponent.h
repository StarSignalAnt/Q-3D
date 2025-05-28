#pragma once
#include "Component.h"
class StaticRendererComponent :
    public Component
{
public:

    void OnRender(GraphNode* camera) override;

private:
};

