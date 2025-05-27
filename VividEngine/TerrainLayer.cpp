#include "pch.h"
#include "TerrainLayer.h"
#include "Texture2D.h"
#include "TextureCube.h"
#include "PixelMap.h"

TerrainLayer::TerrainLayer() {

}


void TerrainLayer::SetPixels(PixelMap* pixels) {
    m_LayerPixMap = pixels;
    m_LayerMap = new Texture2D(pixels->GetWidth(), pixels->GetHeight(), pixels->GetData(), 4);
}


void TerrainLayer::FillBrush(float4 value) {

    auto pix = new PixelMap(m_LayerPixMap->GetWidth(), m_LayerPixMap->GetHeight(),value);
    if (m_LayerMap != nullptr) {
        m_LayerMap->Update(pix->GetData());
    }
    else {
        m_LayerMap = new Texture2D(pix->GetWidth(),pix->GetHeight(),pix->GetData(), 4);
    }

}

void TerrainLayer::PlotBrush(float x, float y, int w, int h, float strength)
{
    int px = (int)(x * ((float)m_LayerPixMap->GetWidth()));
    int py = (int)(y * ((float)m_LayerPixMap->GetHeight()));

    for (int ax = -w; ax < w; ax++)
    {
        for (int ay = -h; ay < h; ay++)
        {
            if (ax == 0 && ay == 0)
            {
                int bb = 5;
            }
            float s = 0;

            float xd, yd;
            float dis = (float)sqrt(ax * ax + ay * ay);

            s = dis / w;
            if (s > 1.0)
            {
                s = 1.0f;
            }

            s = 1.0f - s;
            if (s < 0) s = 0;
            //s = s * strength;



            s = s * strength;
            
            if (s > 0.25) {
                int bb = 5;
            }
            //s = s / 2.0f;
            s = s * 255.0f;
            //if (s > 0.1) {

            //}

            int pv = (int)(m_LayerPixMap->GetColor(px + ax, py + ay).r * 255.0f);
            pv = (int)s + pv;
            if (pv > 255)
            {
                pv = 255;
            }
            if (pv < 0) pv = 0;
            
            m_LayerPixMap->SetColor(px + ax, py + ay, float4(((float)(pv)/255.0f), ((float)(pv)/255.0f), ((float)(pv)/255.0f),1.0));
        }
    
    }
    //LayerMap = new Texture2D(LayerPixMap.Width, LayerPixMap.Height, LayerPixMap.Data, 4);
  
    if (m_LayerMap != nullptr) {
        m_LayerMap->Update(m_LayerPixMap->GetData());
    }
    else {
        m_LayerMap = new Texture2D(m_LayerPixMap->GetWidth(), m_LayerPixMap->GetHeight(), m_LayerPixMap->GetData(), 4);
    }
}