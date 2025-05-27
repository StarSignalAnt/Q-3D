#pragma once
#include "PostProcess.h"

class MaterialEmissive;

class MaterialBlur;
class MaterialColorLimit;
class MaterialCombine;
class PPEmissive :
    public PostProcess
{
public:

    PPEmissive();
    Texture2D* Process(Texture2D* frame);

private:

    MaterialEmissive* m_Emissive = nullptr;
    MaterialBlur* m_Blur;
    MaterialCombine* m_Combine;
};

