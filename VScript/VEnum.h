#pragma once
#include <vector>
#include <map>
#include <string>

class VEnum
{
public:

	void AddValue(std::string name, int value);
	int GetValue(std::string name);
	void SetName(std::string name);
	std::string GetName();

private:

	std::map<std::string, int> m_ValueMap;
	std::string m_Name;

};

