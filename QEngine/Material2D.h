#pragma once
#include "RenderMaterial.h"
class Material2D : public RenderMaterial
{
public:
    Material2D();
    void Bind(bool add) override;
    void Render() override;
    void SetColorTexture(Q3D::Engine::Texture::Texture2D* texture)
    {
        m_ColorTexture = texture;
    }

private:

    Q3D::Engine::Texture::Texture2D* m_ColorTexture;

};

