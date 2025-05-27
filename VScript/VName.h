#pragma once
#include <string>
#include <vector>

class VName
{
public:

	VName() {};
	VName(std::vector<std::string> names);
	void Add(std::string);
	bool SameName(VName right);
	std::vector<std::string> GetNames();

private:

	std::vector<std::string> m_Names;

};


