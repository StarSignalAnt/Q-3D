#pragma once
#include <string>
#include "MonoHost.h"
#include "MClass.h"

class MAsm
{
public:

	MAsm(std::string name,std::string path);
	MClass* GetClass(std::string nspace,std::string clsname);
private:

	MonoAssembly* m_Assembly;
	MonoImage* m_Image;
};

