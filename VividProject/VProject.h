#pragma once
#include <string>
#include <vector>
#include <PixelMap.h>
class VProject
{
public:

	VProject();
	void SetName(std::string name);
	void SetPath(std::string path);
	void SetEngineVersion(std::string version);
	void SetImage(std::string image);
	void SetDescription(const std::string& desc);

	// --- Added Methods ---
	void Save(const std::string& path);
	void Load(const std::string& path);

	// --- Added Getters for convenience ---
	const std::string& GetName() const { return m_Name; }
	const std::string& GetPath() const { return m_ContentPath; }
	const std::string& GetDescription() const { return m_Description; } 
	PixelMap* GetImage() const { return m_Image; }

private:

	std::string m_Name = "";
	std::string m_ContentPath = "";
	std::string m_EngineVersion = "";
	std::string m_Description = ""; 

	PixelMap* m_Image = nullptr;

};

