#pragma once
#include "RenderMaterial.h"
class MaterialTerrainDepth :
    public RenderMaterial
{
public:
    MaterialTerrainDepth();
    void Bind(bool add) override;
    void Render() override;
//    void SetColorTexture(Texture2D* texture)
 //   {

        //m_ColorTexture = texture;
  //  }
private:

};

