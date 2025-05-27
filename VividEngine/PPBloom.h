#pragma once
#include "PostProcess.h"

class MaterialBlur;
class MaterialColorLimit;
class MaterialCombine;

class PPBloom :
    public PostProcess
{
public:

    PPBloom();
    Texture2D* Process(Texture2D* frame);

private:

    MaterialColorLimit* m_ColorLimit;
    MaterialBlur* m_Blur;
    MaterialCombine* m_Combine;
};

