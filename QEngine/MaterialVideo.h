#pragma once
#include "Material2D.h"
class MaterialVideo :
    public RenderMaterial
{
public:
    MaterialVideo();
    void Bind(bool add) override;
    void Render() override;
    void SetColorTexture(Q3D::Engine::Texture::Texture2D* texture)
    {

    }
    Q3D::Engine::Texture::Texture2D* m_Y;
    Q3D::Engine::Texture::Texture2D* m_U;
    Q3D::Engine::Texture::Texture2D* m_V;

private:

   
};

