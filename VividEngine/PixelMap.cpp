#include "pch.h"
#include "PixelMap.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
PixelMap::PixelMap(std::string path) {
	int w, h, bpp_in;
	// stbi_load reads an image and returns raw pixel data.
	// The last parameter '4' forces the image to be loaded with 4 channels (RGBA).
	unsigned char* loaded_data = stbi_load(path.c_str(), &w, &h, &bpp_in, 4);

	if (loaded_data == NULL) {
		// If loading fails, create a default magenta error texture.
		m_Width = 128;
		m_Height = 128;
		m_BPP = 4;
		m_Data = new float[m_Width * m_Height * m_BPP];
		m_Path = "";
		// Fill with a solid color to indicate an error
		for (int i = 0; i < m_Width * m_Height; ++i) {
			m_Data[i * 4 + 0] = 1.0f; // R
			m_Data[i * 4 + 1] = 0.0f; // G
			m_Data[i * 4 + 2] = 1.0f; // B
			m_Data[i * 4 + 3] = 1.0f; // A
		}
		return;
	}

	m_Width = w;
	m_Height = h;
	m_BPP = 4; // We requested 4 components (RGBA)
	m_Data = new float[w * h * 4];
	m_Path = path;

	// Convert the 8-bit unsigned char data (0-255) to 32-bit float (0.0-1.0)
	for (int i = 0; i < w * h * 4; i++) {
		m_Data[i] = (float)loaded_data[i] / 255.0f;
	}

	// Free the memory allocated by stbi_load
	stbi_image_free(loaded_data);
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