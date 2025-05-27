#pragma once
#include <vector>
#include <string>
#include "VName.h"
#include "VVarGroup.h"

class VFunction;
class VContext;
class VScope;
class VFile;

class VClass
{
public:

	void SetName(VName name);
	void AddVarGroup(VVarGroup* group);
	void AddStaticVarGroup(VVarGroup* group);
	void AddFunction(VFunction* func);
	void AddStaticFunction(VFunction* func)
	{
		m_StaticFuncs.push_back(func);
	}
	VFunction* FindFunction(VName name);
	void SetContext(VContext* context);
	VClass* Clone();
	VName GetName();
	VFunction* FindFunction(std::string name);
	VFunction* FindFunctionBySig(std::string name,std::vector<TokenType> sig);
	VFunction* FindFunctionBySig2(std::string name, std::vector<std::string> sig);
	VVar* FindVar(std::string name);
	VScope* GetScope() {
		return m_InstanceScope;
	}
	std::vector<VFunction*> GetFunctions();
	void SetScope(VScope* scope) {
		m_InstanceScope = scope;
	}
	void CreateScope();
	void SetStatic(bool stat) {
		m_Static = stat;
	}
	std::vector<VFunction*> GetStaticFuncs() {
		return m_StaticFuncs;
	}
	void SetSubClass(std::string name);
	std::string GetSubClass();
	void SetDataName(std::string name) {
		m_DataName = name;
	}
	std::string GetDataName() {
		return m_DataName;
	}
	void WriteClass(VFile* file);
	void ReadClass(VFile* file);
	std::vector<VVarGroup*> GetGroups() {

		return m_Groups;

	}
	void SetStart(int s) {
		m_Start = s;
	}
	void SetEnd(int e) {
		m_End = e;
	}
	int GetStart() {
		return m_Start;
	}
	int GetEnd() {
		return m_End;
	}
private:
	int m_Start = 0;
	int m_End = 0;
	bool m_Static = false;
	VContext* m_Context;
	VName m_Name;
	std::vector<VVarGroup*> m_StaticGroups;
	std::vector<VFunction*> m_StaticFuncs;
	std::vector<VVarGroup*> m_Groups;
	std::vector<VFunction*> m_Funcs;
	VScope* m_InstanceScope = nullptr;
	std::string m_SubClass = "";
	std::string m_DataName = "";
};

