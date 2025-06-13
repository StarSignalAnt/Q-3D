#pragma once
#include <string>
#include "BasicMath.hpp"
#include "VFile.h"
using namespace Diligent;



class PixelMap
{
public:

	PixelMap(int w, int h);
	PixelMap(int w, int h, float4 color);
	PixelMap(std::string path);
	float4 GetColor(int x, int y);
	void SetColor(int x, int y, float4 color);
	int GetWidth() {
		return m_Width;
	}
	int GetHeight() {
		return m_Height;
	}
	float* GetData() {
		return m_Data;
	}
	int GetBPP() {
		return m_BPP;
	}
	void Write(VFile* f);
	void Read(VFile* f);

private:

	int m_Width;
	int m_Height;
	int m_BPP;
	float* m_Data;
	std::string m_Path;

};

