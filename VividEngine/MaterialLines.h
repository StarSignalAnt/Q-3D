#pragma once
#include "RenderMaterial.h"
class MaterialLines :
    public RenderMaterial
{
public:

    MaterialLines();
    void Bind(bool add) override;
    void Render() override;

};

