#include "pch.h"
#include "PixelMap.h"

PixelMap::PixelMap(int w, int h) {

	m_Width = w;
	m_Height = h;
	m_Data = new float[w * h * 4];
	m_BPP = 4;
	for (int l = 0; l < (w * h * 4); l++) {
		
		m_Data[l] = 0.0f;

		//auto pix = (unsigned char*)(m_Data) + l;
		//pix[0] = 0;


	}

}


PixelMap::PixelMap(int w, int h,float4 color) {

	m_Width = w;
	m_Height = h;
	m_Data = new float[w * h * 4];
	m_BPP = 4;
	for (int y = 0; y < m_Height; y++) {
		for (int x = 0; x < m_Width; x++) {

			int loc = (y * m_Width * m_BPP) + (x * m_BPP);
			m_Data[loc++] = color.x;
			m_Data[loc++] = color.y;
			m_Data[loc++] = color.z;
			m_Data[loc++] = color.w;

		}
	}

	

}

float4 PixelMap::GetColor(int x, int y) {

	if (x < 0 || x >= m_Width || y < 0 || y >= m_Height) return float4(0, 0, 0, 0);
	float4 res;

	int loc = (y * m_Width * m_BPP) + (x * m_BPP);
	res.x = m_Data[loc++];
	res.y = m_Data[loc++];
	res.z = m_Data[loc++];
	res.w = m_Data[loc++];

	return res;




}

void PixelMap::SetColor(int x, int y, float4 col) {

	if (x < 0 || x >= m_Width || y < 0 || y >= m_Height) return;
	float4 res;

	int loc = (y * m_Width * m_BPP) + (x * m_BPP);
	m_Data[loc++] = col.x;
	m_Data[loc++] = col.y;
	m_Data[loc++] = col.z;
	m_Data[loc++] = col.w;

}

void PixelMap::Write(VFile* f) {

	f->WriteInt(m_Width);
	f->WriteInt(m_Height);
	f->WriteInt(m_BPP);
	f->WriteBytes((unsigned char*)m_Data, m_Width * m_Height * m_BPP * sizeof(float));
	

}

void PixelMap::Read(VFile* f) {
	m_Width = f->ReadInt();
	m_Height = f->ReadInt();
	m_BPP = f->ReadInt();
	//m_Data = new float[m_Width * m_Height * m_BPP];
	m_Data=(float*)f->ReadBytes(m_Width * m_Height * m_BPP * sizeof(float));
}