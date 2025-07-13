#pragma once
#include "RenderMaterial.h"

class MaterialSky : public RenderMaterial
{
public:
    MaterialSky();
    void Bind(bool add) override;
    void Render() override;
    void SetColorTexture(Q3D::Engine::Texture::Texture2D* texture)
    {
//        m_ColorTexture = texture;
    }



    glm::vec3 m_SunDir;
    glm::vec3 m_ZenithColor;
    glm::vec3 m_HorizonColor;

    glm::vec3 m_CameraPos;
    float m_AtmosRadius;
    float m_PlanetRadius;
    float m_SunIntense;
    glm::vec3 m_RayLeigh;
    float m_Mie;
    float m_DayLight = 0;
    float m_Time = 0;

};

