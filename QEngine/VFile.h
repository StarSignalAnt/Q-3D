#pragma once
#include<iostream>
#include<ostream>
#include<fstream>
#include "BasicMath.hpp"
#include <glm/glm.hpp>
#include <filesystem> 
using namespace Diligent;

//#include "Vec2.h"

enum FileMode {

	Write,Read

};

class VFile
{
public:
	static bool Exists(const char* path);

	VFile(const char* path, FileMode mode);
	void WriteInt(int v);
	void WriteFloat(float v);
	void WriteString(const char* str);
	void WriteByte(char b);
	void WriteBytes(void* data, int length);
	void WriteBool(bool b);
	void WriteVec3(glm::vec3 v);
	void WriteVec4(glm::vec4 v);
	void WriteMatrix(glm::mat4 v);
//	void WriteMatrix3x3(float3x3 v);
	glm::vec3 ReadVec3();
	glm::vec4 ReadVec4();
	void WriteLongLong(long long v);
	void WriteFileTime(const std::filesystem::file_time_type& ftime);
	//void WriteVec3(Vec3 v);
	void WriteLong(long v);
	void WriteLine(const char* str);


//	Vec3 ReadVec3();
	long long ReadLongLong();
	std::filesystem::file_time_type ReadFileTime();
	int ReadInt();
	float ReadFloat();
	char ReadByte();
	void* ReadBytes(int len);
	const char* ReadString();
	long ReadLong();
	bool ReadBool();
	glm::mat4 ReadMatrix();
	void Close();
	static long Length(const char * path);
	void Seek(long position);
	long GetPosition();

private:
	std::ifstream istream;
	std::ofstream ostream;
	FileMode mod;
	int readPos = 0;

};

