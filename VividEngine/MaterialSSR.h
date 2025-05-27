#pragma once
#include "Material2D.h"

class TextureDepth;
class Texture2D;

class MaterialSSR :
    public Material2D
{
public:

    MaterialSSR();
    void Create();
    void Bind(bool sp) override;
    void SetColor(Texture2D* color) {
        m_Color = color;
    }
    void SetDepth(TextureDepth* depth)
    {
        m_Depth = depth;
    }
    void SetNormals(Texture2D* normals)
    {
        m_Normals = normals;
    }
    void SetPositions(Texture2D* positions) {
        m_Positions = positions;
    }

    Texture2D* m_Color;
    Texture2D* m_Normals;
    Texture2D* m_Positions;
    TextureDepth* m_Depth;
};

