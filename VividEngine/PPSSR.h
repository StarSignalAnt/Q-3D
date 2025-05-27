#pragma once
#include "PostProcess.h"

class MaterialSSR;
class Texture2D;
class TextureDepth;

class PPSSR :
    public PostProcess
{
public:

    PPSSR();
    void SetColor(Texture2D* color) {
        m_Color = color;
    }
    void SetNormals(Texture2D* normals);
    void SetDepth(TextureDepth* depth);
    void SetPositions(Texture2D* positions);
    Texture2D* Process(Texture2D* frame);
private:
   
    MaterialSSR* m_SSR;
    Texture2D* m_Color;
    TextureDepth* m_Depth;
    Texture2D* m_Normals;
    Texture2D* m_Positions;

};


