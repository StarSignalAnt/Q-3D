#include "pch.h"
#include "VEnum.h"

void VEnum::AddValue(std::string name, int value)
{

	m_ValueMap[name] = value;

}

int VEnum::GetValue(std::string name) {

	return m_ValueMap[name];

}

void VEnum::SetName(std::string name) {

	m_Name = name;

}

std::string VEnum::GetName() {

	return m_Name;

}