#pragma once
#include "RenderMaterial.h"
class MaterialSkeletal :
    public RenderMaterial
{
public:


    MaterialSkeletal();
    void Bind(bool add) override;
    void Render() override;
    void SetColorTexture(Q3D::Engine::Texture::Texture2D* texture)
    {
        m_ColorTexture = texture;
    }
    void SetNormalTexture(Q3D::Engine::Texture::Texture2D* texture)
    {
        m_NormalTexture = texture;
    }
    void SetMetallicTexture(Q3D::Engine::Texture::Texture2D* texture)
    {
        m_MetallicTexture = texture;
    }
    void SetRoughnessTexture(Q3D::Engine::Texture::Texture2D* texture)
    {
        m_RoughnessTexture = texture;
    }
    void SetEnvironmentMap(TextureCube* texture)
    {
        m_EnvironmentMap = texture;
    }
    void SetBRDF(Q3D::Engine::Texture::Texture2D* texture)
    {
        m_BRDF = texture;
    }
    void SetIRR(TextureCube* texture)
    {
        m_IRR = texture;
    }



    Q3D::Engine::Texture::Texture2D* GetColorTexture() const
    {
        return m_ColorTexture;
    }
    Q3D::Engine::Texture::Texture2D* GetNormalTexture() const
    {
        return m_NormalTexture;
    }
    Q3D::Engine::Texture::Texture2D* GetMetallicTexture() const
    {
        return m_MetallicTexture;
    }

    TextureCube* GetEnvironmentMap() const
    {
        return m_EnvironmentMap;
    }
    void SetHeightTexture(Q3D::Engine::Texture::Texture2D* texture)
    {
        m_HeightTexture = texture;
    }
    void SetParallaxScale(float scale)
    {
        m_ParalaxScale = scale;
    }

private:



    Q3D::Engine::Texture::Texture2D* m_ColorTexture = nullptr;
    Q3D::Engine::Texture::Texture2D* m_NormalTexture = nullptr;
    Q3D::Engine::Texture::Texture2D* m_MetallicTexture = nullptr;
    Q3D::Engine::Texture::Texture2D* m_RoughnessTexture = nullptr;
    Q3D::Engine::Texture::Texture2D* m_BRDF = nullptr;
    TextureCube* m_IRR = nullptr;
    TextureCube* m_EnvironmentMap = nullptr;
    Q3D::Engine::Texture::Texture2D* m_HeightTexture = nullptr;
    float m_ParalaxScale = 0.05f;
};

