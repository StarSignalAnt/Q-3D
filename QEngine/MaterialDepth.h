#pragma once
#include "RenderMaterial.h"
class MaterialDepth :
    public RenderMaterial
{
public:

    MaterialDepth();
    void Bind(bool add) override;
    void Render() override;

};

