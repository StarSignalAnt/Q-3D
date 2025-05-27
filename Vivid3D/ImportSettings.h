#pragma once
#include <string>

class Node;

class ImportSettings
{
public:

	ImportSettings();
	void SetScale(float scale);
	void SetPath(std::string path);
	void SetActor(bool actor);
	void SetName(std::string name);
	void Save();
	void Load(std::string path);
	Node* GetNode() {
		return m_Node;
	}
	bool GetActor() {
		return m_ImportAsActor;
	}
private:

	float m_Scale = 1.0f;
	std::string m_Path = "";
	bool m_ImportAsActor;
	std::string m_Name = "";
	Node* m_Node;

};

