#pragma once
#include "RenderMaterial.h"
class MaterialDepth :
    public RenderMaterial
{
public:

    MaterialDepth();
    MaterialDepth(bool clone);
    void Bind(bool add) override;
    void Render() override;

};

