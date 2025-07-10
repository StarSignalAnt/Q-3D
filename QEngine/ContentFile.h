#pragma once
#include <string>

class VFile;

class ContentFile
{
public:

	ContentFile(std::string id, std::string path, long size);
	ContentFile(long start, long size, long compressed_size, bool compressed);
	void SetContentArchive(VFile* file)
	{
		m_ContentArchive = file;
	}
	ContentFile(std::string id, std::string path, long start, long size, long compressed_size, bool compressed);

	void SetArchiveData(long start,long size, long compressedSize, bool is_compressed);
	const std::string& GetID() const { return m_ID; }
	const std::string& GetFullPath() const { return m_FullPath; }
	const std::string& GetExtension() const { return m_Extension; }
	const long GetSize() const { return m_Size; }
	const long GetStart() const { return m_Start; }
	const long GetCompressedSize() const { return m_CompressedSize; }
	const bool IsCompressed() const { return m_Compressed; }
	void* GetData() const { return m_Data; }
	void* LoadResource();

private:

	long m_Start = 0;
	long m_Size = 0;
	std::string m_ID;
	std::string m_FullPath;
	std::string m_Extension;
	long m_CompressedSize = 0;
	bool m_InMemory = false;
	bool m_Compressed = false;
	VFile* m_ContentArchive = nullptr;
	void* m_Data = nullptr; // Pointer to the loaded resource data
};

