#include "pch.h"
#include "VFile.h"
#include <filesystem>
#include <memory>
VFile::VFile(const char* path, FileMode mode) {

	//printf("Opening File:");
	//printf(path);
	//printf(":\n");

	switch (mode) {
	case FileMode::Read:
		istream.open(path, std::ios::binary | std::ios::in);
		mod = mode;
		break;
	case FileMode::Write:
		ostream.open(path, std::ios::binary | std::ios::out);
		mod = mode;
		break;
	}


}

void VFile::WriteInt(int v) {

	ostream.write(reinterpret_cast<char*>(&v), sizeof(int));

}

void VFile::WriteByte(char b) {

	ostream.write(reinterpret_cast<char*>(&b),sizeof(char));

}

void VFile::WriteFloat(float b) {

	ostream.write(reinterpret_cast<char*>(&b),sizeof(float));

}

void VFile::WriteBytes(void* dat, int len)
{
	ostream.write(reinterpret_cast<char*>(dat), len);
}

void VFile::WriteString(const char* str) {

	int len = 0;
	for (int i = 0; i < 2500; i++) {
		
		if(str[i]=="\0"[0])
		{
			break;
		}
		len++;
	}

	WriteInt(len);
	WriteBytes((void*)str, len);

}

void VFile::WriteBool(bool b) {

	if (b) {
		WriteByte(1);
	}
	else {
		WriteByte(0);
	}

}

bool VFile::ReadBool()
{

	if (ReadByte() == 1)
	{
		return true;
	}
	return false;

}

int VFile::ReadInt()
{
	int r = 0;
	istream.read(reinterpret_cast<char*>(&r) , sizeof(int));
	readPos += 4;
	return r;
}

float VFile::ReadFloat() {

	float r = 0;
	istream.read(reinterpret_cast<char*>(&r),sizeof(float));
	readPos += 4;
	return r;
}


char VFile::ReadByte() {

	char r = 0;
	istream.read(reinterpret_cast<char*>(&r),sizeof(char));
	readPos += 1;
	return r;

}

void* VFile::ReadBytes(int len) {

	char* r = (char*)malloc(len);
	istream.read(reinterpret_cast<char*>(r), len);
	readPos += len;
	return (void*)r;

}

const char* VFile::ReadString() {

	int len = ReadInt();
	

	char* nbuf = (char*)malloc(len + 1);

	char* buf2 = (char *)ReadBytes(len);

	for (int i = 0; i < len; i++) {
		nbuf[i] = buf2[i];
	}
	nbuf[len] = "\0"[0];
	return nbuf;



}

void VFile::WriteVec3(glm::vec3 v) {

	WriteFloat(v.x);
	WriteFloat(v.y);
	WriteFloat(v.z);

}

void VFile::WriteVec4(glm::vec4 v)
{

	WriteFloat(v.x);
	WriteFloat(v.y);
	WriteFloat(v.z);
	WriteFloat(v.w);

}
glm::vec4 VFile::ReadVec4() {

	float xv = ReadFloat();
	float yv = ReadFloat();
	float zv = ReadFloat();
	float wv = ReadFloat();
	return glm::vec4(xv, yv, zv, wv);

}

glm::vec3 VFile::ReadVec3() {




	float xv = ReadFloat();
	float yv = ReadFloat();
	float zv = ReadFloat();

	glm::vec3 rv(xv, yv, zv);

//glm::vec3 rv= glm::vec3(ReadFloat(), ReadFloat(), ReadFloat());


return rv;
}


bool VFile::Exists(const char* path) {

	return std::filesystem::exists(path);


}

/*
void VFile::WriteVec3(Vec3 v) {

	WriteFloat(v.X);
	WriteFloat(v.Y);
	WriteFloat(v.Z);

};

Vec3 VFile::ReadVec3() {

	Vec3 v;

	v.X = ReadFloat();
	v.Y = ReadFloat();
	v.Z = ReadFloat();

	return v;


};

*/

void VFile::Close() {

	switch (mod) {
	case FileMode::Read:
		
		istream.close();
		break;
	case FileMode::Write:
		ostream.flush();
		ostream.close();
		break;
	}

}

long GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

long VFile::Length(const char *file) {

	std::string p(file);
	return GetFileSize(p);

}


long VFile::ReadLong() {

	long r = 0;
	istream.read(reinterpret_cast<char*>(&r),sizeof(long));
	readPos += sizeof(long);
	return r;

}

void VFile::WriteLong(long v) {

	ostream.write(reinterpret_cast<char*>(&v), sizeof(long));

}

void VFile::WriteLine(const char* line) {

	
	int len = 0;
	for (int i = 0; i < 2048; i++)
	{
		if (line[i] == "\n"[0])
		{
			break;
		}
		len++;
	}
	len++;

	ostream.write(line, len);

}

void VFile::Seek(long position) {

	istream.seekg(position, std::ios::beg);

}

void VFile::WriteMatrix(glm::mat4 matrix) {

	WriteVec4(matrix[0]);
	WriteVec4(matrix[1]);
	WriteVec4(matrix[2]);
	WriteVec4(matrix[3]);

	//WriteFloat(matrix.m10);
	//WriteFloat(matrix.m11);
	//WriteFloat(matrix.m12);
	//WriteFloat(matrix.m13);
	//WriteFloat(matrix.m20);
//	WriteFloat(matrix.m21);
//	WriteFloat(matrix.m22);
//	WriteFloat(matrix.m23);
//	WriteFloat(matrix.m30);
//	WriteFloat(matrix.m31);
	//WriteFloat(matrix.m32);
	//WriteFloat(matrix.m33);

}


glm::mat4 VFile::ReadMatrix() {

	glm::mat4 r;

	r[0] = ReadVec4();
	r[1] = ReadVec4();
	r[2] = ReadVec4();
	r[3] = ReadVec4();

	return r;

}