#include "pch.h"
#include "VName.h"

VName::VName(std::vector<std::string> names) {

	m_Names = names;

}

bool VName::SameName(VName right)
{

	std::vector<std::string> r_names = right.GetNames();
	int idx = 0;
	for (auto name : m_Names) {

		if (name != r_names[idx]) {
			return false;
		}
		idx++;
	}
	return true;

}

void VName::Add(std::string name) {

	m_Names.push_back(name);

}

std::vector<std::string> VName::GetNames() {

	return m_Names;

}