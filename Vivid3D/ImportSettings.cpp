#include "ImportSettings.h"
#include "VFile.h"
#include "Importer.h"
#include "Node.h"

ImportSettings::ImportSettings()
{



}

void ImportSettings::SetScale(float scale) {

	m_Scale = scale;

}

void ImportSettings::SetPath(std::string path) {

	m_Path = path;

}

void ImportSettings::SetActor(bool actor) {

	m_ImportAsActor = actor;

}

void ImportSettings::SetName(std::string name) {

	m_Name = name;

}

void ImportSettings::Save() {

	auto path = m_Path + ".import";

	VFile* file = new VFile(path.c_str(),FileMode::Write);

	file->WriteString(m_Path.c_str());
	file->WriteString(m_Name.c_str());
	file->WriteFloat(m_Scale);
	file->WriteBool(m_ImportAsActor);

	file->Close();



}

void ImportSettings::Load(std::string path) {


	VFile* file = new VFile(path.c_str(), FileMode::Read);


	m_Path = file->ReadString();
	m_Name = file->ReadString();
	m_Scale = file->ReadFloat();
	m_ImportAsActor = file->ReadBool();

	file->Close();

	Importer* imp = new Importer;

	if (m_ImportAsActor) {

		m_Node = imp->ImportActor(m_Path);

	}
	else {

		m_Node = imp->ImportNode(m_Path);


	}

	m_Node->SetName(m_Name);
	m_Node->SetScale(float3(m_Scale, m_Scale, m_Scale));

}