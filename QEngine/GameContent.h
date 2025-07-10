#pragma once
#include <string>
#include <vector>

class ContentFile;
class VFile;

class GameContent
{
public:

	GameContent(std::string path);
	void ScanContent();
	void ReadArchive();
	void BuildArchive();
	void Debug_ListFiles();
	void SetContentArchive(VFile* file) {
		m_ArchiveFile = file;
	}
	ContentFile* FindContent(const std::string& fullPath);

private:

	std::string m_ContentPath;
	std::vector<ContentFile*> m_Contents;
	VFile* m_ArchiveFile = nullptr;
};

