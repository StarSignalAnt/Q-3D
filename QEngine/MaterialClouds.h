#pragma once
#include "RenderMaterial.h"

class Texture3D;

class MaterialClouds : public RenderMaterial
{
public:

    MaterialClouds();
    void Bind(bool add) override;
    void Render() override;
    Texture3D* m_Map;
    glm::vec3 m_SunDir;
    glm::vec3 m_VolStart;
    glm::vec3 m_VolSize;

private:
};

