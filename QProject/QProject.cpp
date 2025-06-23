#include "pch.h"
#include "QProject.h"
#include <VFile.h>
#include <PixelMap.h>
QProject::QProject() {

}

void QProject::SetName(std::string name) {

	m_Name = name;

}

void QProject::SetPath(std::string path) {

	m_ContentPath = path;
	
}

void QProject::SetEngineVersion(std::string version) {

	m_EngineVersion = version;

}

void QProject::SetImage(std::string image) {

	m_Image = new PixelMap(image);

}
void QProject::SetDescription(const std::string& desc)
{
    m_Description = desc;
}

void QProject::Save(const std::string& path)
{
    VFile file(path.c_str(), FileMode::Write);
    file.WriteString(m_Name.c_str());
    file.WriteString(m_ContentPath.c_str());
    file.WriteString(m_EngineVersion.c_str());
    file.WriteString(m_Description.c_str());

    if (m_Image != nullptr)
    {
        file.WriteBool(true);
        m_Image->Write(&file);
    }
    else
    {
        file.WriteBool(false);
    }
}

void QProject::Load(const std::string& path)
{
    // NOTE: This assumes your VFile class can be constructed for reading
    // and has methods for reading strings and bools.
    VFile file(path.c_str(), FileMode::Read);

    m_Name = file.ReadString();
    m_ContentPath = file.ReadString();
    m_EngineVersion = file.ReadString();
    m_Description = file.ReadString();
    bool hasImage = file.ReadBool();
    if (hasImage)
    {
        if (m_Image != nullptr)
        {
            delete m_Image;
        }
        // Create an empty PixelMap to load the data into.
        // The Read method will populate its internal members.
        m_Image = new PixelMap(0, 0);
        m_Image->Read(&file);
    }
    else
    {
        if (m_Image != nullptr)
        {
            delete m_Image;
            m_Image = nullptr;
        }
    }
}