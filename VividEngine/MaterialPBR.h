#pragma once
#include "RenderMaterial.h"
class MaterialPBR :
    public RenderMaterial
{
public:

    MaterialPBR();
    void Bind(bool add) override;
    void Render() override;
    void SetColorTexture(Texture2D* texture)
    {
        m_ColorTexture = texture;
    }
    void SetNormalTexture(Texture2D* texture)
    {
        m_NormalTexture = texture;
	}
    void SetMetallicTexture(Texture2D* texture)
    {
        m_MetallicTexture = texture;
	}
    void SetRoughnessTexture(Texture2D* texture)
    {
        m_RoughnessTexture = texture;
    }
    void SetEnvironmentMap(TextureCube* texture)
    {
        m_EnvironmentMap = texture;
	}
    void SetBRDF(Texture2D* texture)
    {
        m_BRDF = texture;
    }
    void SetIRR(TextureCube* texture)
    {
        m_IRR = texture;
	}



    Texture2D* GetColorTexture() const
    {
        return m_ColorTexture;
    }
    Texture2D* GetNormalTexture() const
    {
        return m_NormalTexture;
    }
    Texture2D* GetMetallicTexture() const
    {
        return m_MetallicTexture;
	}
 
    TextureCube* GetEnvironmentMap() const
    {
        return m_EnvironmentMap;
	}
    void SetHeightTexture(Texture2D* texture)
    {
        m_HeightTexture = texture;
	}
    void SetParallaxScale(float scale)
    {
        m_ParalaxScale = scale;
    }
    
private:



    Texture2D* m_ColorTexture = nullptr;
    Texture2D* m_NormalTexture = nullptr;
    Texture2D* m_MetallicTexture = nullptr;
    Texture2D* m_RoughnessTexture = nullptr;
    Texture2D* m_BRDF = nullptr;
    TextureCube* m_IRR = nullptr;
	TextureCube* m_EnvironmentMap = nullptr;
    Texture2D* m_HeightTexture = nullptr;
	float m_ParalaxScale = 0.05f;

};

