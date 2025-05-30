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


private:


    Texture2D* m_ColorTexture = nullptr;
    Texture2D* m_NormalTexture = nullptr;
    Texture2D* m_MetallicTexture = nullptr;
    Texture2D* m_RoughnessTexture = nullptr;
	TextureCube* m_EnvironmentMap = nullptr;

};

