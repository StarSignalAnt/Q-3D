#pragma once
#include "RenderMaterial.h"
class MaterialOutline :
    public RenderMaterial
{
public:
    MaterialOutline();
    void Bind(bool add) override;
    void Render() override;
    void SetColorTexture(Texture2D* texture)
    {
        m_ColorTexture = texture;
    }

private:

    Texture2D* m_ColorTexture;

};

