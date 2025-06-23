#pragma once
#include "RenderMaterial.h"

class MaterialSkeletalDepth : public RenderMaterial
{
public:

    MaterialSkeletalDepth();
    void Bind(bool add) override;
    void Render() override;

};

