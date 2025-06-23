#include "pch.h"
#include "PixelMap.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

PixelMap::PixelMap(std::string path, PixelMapDataType type) {
	m_Path = path;
	m_Type = type;

	int w, h, bpp_in;
	unsigned char* loaded_data = stbi_load(path.c_str(), &w, &h, &bpp_in, 4);

	if (loaded_data == NULL) {
		m_Width = 128;
		m_Height = 128;
		m_BPP = 4;
		m_Type = PixelMapDataType::UINT8; // Fallback to 8-bit for error texture
		m_Data = new unsigned char[m_Width * m_Height * m_BPP];
		for (int i = 0; i < m_Width * m_Height; ++i) {
			static_cast<unsigned char*>(m_Data)[i * 4 + 0] = 255;
			static_cast<unsigned char*>(m_Data)[i * 4 + 1] = 0;
			static_cast<unsigned char*>(m_Data)[i * 4 + 2] = 255;
			static_cast<unsigned char*>(m_Data)[i * 4 + 3] = 255;
		}
		return;
	}

	m_Width = w;
	m_Height = h;
	m_BPP = 4;

	if (m_Type == PixelMapDataType::FLOAT32) {
		// Convert 8-bit data to 32-bit float data
		m_Data = new float[w * h * 4];
		for (int i = 0; i < w * h * 4; i++) {
			static_cast<float*>(m_Data)[i] = (float)loaded_data[i] / 255.0f;
		}
		// Free the original 8-bit data
		stbi_image_free(loaded_data);
	}
	else {
		// Use the 8-bit data directly
		m_Data = loaded_data;
	}
}



// Constructor for a blank map of a specific type
PixelMap::PixelMap(int w, int h, PixelMapDataType type) {
	m_Width = w;
	m_Height = h;
	m_BPP = 4;
	m_Type = type;
	if (type == PixelMapDataType::FLOAT32) {
		m_Data = new float[w * h * 4];
		memset(m_Data, 0, w * h * 4 * sizeof(float));
	}
	else {
		m_Data = new unsigned char[w * h * 4];
		memset(m_Data, 0, w * h * 4);
	}
}

float4 PixelMap::GetColor(int x, int y) {
	if (x < 0 || x >= m_Width || y < 0 || y >= m_Height || m_Data == nullptr) return float4(0, 0, 0, 0);

	int loc = (y * m_Width + x) * m_BPP;
	float4 res;

	if (m_Type == PixelMapDataType::FLOAT32) {
		float* floatData = static_cast<float*>(m_Data);
		res.x = floatData[loc + 0];
		res.y = floatData[loc + 1];
		res.z = floatData[loc + 2];
		res.w = floatData[loc + 3];
	}
	else {
		unsigned char* byteData = static_cast<unsigned char*>(m_Data);
		res.x = byteData[loc + 0];// / 255.0f;
		res.y = byteData[loc + 1]; // 255.0f;
		res.z = byteData[loc + 2];// / 255.0f;
		res.w = byteData[loc + 3];// / 255.0f;
	}
	return res;
}


int4 PixelMap::GetIntColor(int x, int y) {
	
	int loc = (y * m_Width + x) * m_BPP;
	float4 res;

	if (m_Type == PixelMapDataType::FLOAT32) {
		float* floatData = static_cast<float*>(m_Data);
		res.x = floatData[loc + 0];
		res.y = floatData[loc + 1];
		res.z = floatData[loc + 2];
		res.w = floatData[loc + 3];
	}
	else {
		unsigned char* byteData = static_cast<unsigned char*>(m_Data);
		res.x = byteData[loc + 0];// / 255.0f;
		res.y = byteData[loc + 1]; // 255.0f;
		res.z = byteData[loc + 2];// / 255.0f;
		res.w = byteData[loc + 3];// / 255.0f;
	}
	unsigned char* byteData = static_cast<unsigned char*>(m_Data);
	return int4(byteData[loc], byteData[loc + 1], byteData[loc + 2], 0);
}


void PixelMap::SetColor(int x, int y, float4 col) {
	if (x < 0 || x >= m_Width || y < 0 || y >= m_Height || m_Data == nullptr) return;

	int loc = (y * m_Width + x) * m_BPP;

	if (m_Type == PixelMapDataType::FLOAT32) {
		float* floatData = static_cast<float*>(m_Data);
		floatData[loc + 0] = col.x;
		floatData[loc + 1] = col.y;
		floatData[loc + 2] = col.z;
		floatData[loc + 3] = col.w;
	}
	else {
		unsigned char* byteData = static_cast<unsigned char*>(m_Data);
		byteData[loc + 0] = static_cast<unsigned char>(col.x * 255.0f);
		byteData[loc + 1] = static_cast<unsigned char>(col.y * 255.0f);
		byteData[loc + 2] = static_cast<unsigned char>(col.z * 255.0f);
		byteData[loc + 3] = static_cast<unsigned char>(col.w * 255.0f);
	}
}


//void PixelMap::Write(VFile* f) {

	//f->WriteInt(m_Width);
//	f->WriteInt(m_Height);
//	f->WriteInt(m_BPP);
//	f->WriteBytes((unsigned char*)m_Data, m_Width * m_Height * m_BPP * sizeof(float));

	

//}

//void PixelMap::Read(VFile* f) {

	//m_Width = f->ReadInt();
//	m_Height = f->ReadInt();
//	m_BPP = f->ReadInt();
	//m_Data = new float[m_Width * m_Height * m_BPP];
//	m_Data=(float*)f->ReadBytes(m_Width * m_Height * m_BPP * sizeof(float));
//}


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
	m_Data = (float*)f->ReadBytes(m_Width * m_Height * m_BPP * sizeof(float));
}