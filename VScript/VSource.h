#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

class VSource
{
public:

	VSource(std::string path);
	VSource() {};
	void SetSource(std::string src)
	{
		m_Source = src;
	}
	std::string GetCode();
	int GetNext();
	void Back();
	int Index() {
		return m_Index;
	}

private:

	std::string m_Source;
	int m_Index = 0;

};

