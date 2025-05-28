#pragma once
#include "RenderMaterial.h"
class MaterialBasic3D :
    public RenderMaterial
{
public:

    MaterialBasic3D();
    void Bind() override;
    void Render() override;
    void SetColorTexture(Texture2D* texture)
    {
		m_ColorTexture = texture;
    }

private:

    Texture2D* m_ColorTexture;

};

