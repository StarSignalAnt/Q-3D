#pragma once

#include "MonoHost.h"
#include "MAsm.h"
#include "MClass.h"
#include <string>
#include <vector>


struct SharpClassInfo {
	std::string namespaceName;
	std::string className;
	std::string baseClassName; // new
};

class MonoLib
{
public:

	MonoLib(std::string dll);
	std::vector<SharpClassInfo> GetClasses();
	MClass* GetClass(std::string name);
	MAsm* GetAssembly() const { return m_Assembly; }
	MAsm* GetVivid() const { return m_Vivid; }
private:
	MAsm* m_Assembly = nullptr;
	MAsm* m_Vivid = nullptr;
};

