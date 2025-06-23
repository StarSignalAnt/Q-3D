#pragma once
#include "Component.h"

class SkeletalDepthRendererComponent : public Component
{
public:

	void OnRenderDepth(GraphNode* camera) override;


private:
};

