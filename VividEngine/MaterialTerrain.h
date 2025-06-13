#pragma once
#include "RenderMaterial.h"

class Texture2D;

class MaterialTerrain : public RenderMaterial
{
public:
    MaterialTerrain();
    void Bind(bool add) override;
    void Render() override;
    void SetColorTexture(Texture2D* texture)
    {
        m_ColorTexture = texture;
    }

private:
    RefCntAutoPtr<IBuffer> m_LightingBuffer;
    Texture2D* m_ColorTexture;
};

