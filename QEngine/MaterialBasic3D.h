#pragma once
#include "RenderMaterial.h"
class MaterialBasic3D :
    public RenderMaterial
{
public:

    MaterialBasic3D();
    void Bind(bool add) override;
    void Render() override;
    void SetColorTexture(Q3D::Engine::Texture::Texture2D* texture)
    {
		m_ColorTexture = texture;
    }

private:

    Q3D::Engine::Texture::Texture2D* m_ColorTexture;

};

