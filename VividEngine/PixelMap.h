#pragma once
#include <string>
#include "BasicMath.hpp"
#include "VFile.h"
using namespace Diligent;

enum class PixelMapDataType
{
	UINT8,
	FLOAT32
};


class PixelMap
{
public:

	// Main constructor for loading from a file, with a flag to specify data type
	PixelMap(std::string path, PixelMapDataType type = PixelMapDataType::UINT8);

	// Constructors for creating blank maps
	PixelMap(int w, int h, PixelMapDataType type = PixelMapDataType::UINT8);
	//PixelMap(int w, int h, float4 color); // This will always create a float map

	

	// Public methods
	float4 GetColor(int x, int y);
	int4 GetIntColor(int x, int y);

	void SetColor(int x, int y, float4 color);
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int GetBPP() const { return m_BPP; }
	PixelMapDataType GetDataType() const { return m_Type; }

	// Typed data accessors
	void* GetData() { return m_Data; }
	const void* GetData() const { return m_Data; }


private:
	int m_Width = 0;
	int m_Height = 0;
	int m_BPP = 0;
	void* m_Data = nullptr; // Generic data pointer
	PixelMapDataType m_Type; // Stores whether data is 8-bit or 32-bit
	std::string m_Path;

};
