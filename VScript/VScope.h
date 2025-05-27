#pragma once
#include <vector>
#include <string>

class VContext;
class VVar;
class VClass;
class VFunction;

class VScope
{
public:

	void RegisterVar(VVar* var);
	VVar* FindVar(std::string name);
	void NoRoot() {
		m_NoRoot = true;
	}
	VFunction* FindFunc(std::string name);

	void SetClass(VClass* cls);
	void SetRoot(VScope* root) {
		m_RootScope = root;
		m_NoRoot = false;
	}
	std::vector<VVar*> GetVars() {
		return m_LocalVars;
	}
	void SetContext(VContext* context);
private:

	VScope* m_RootScope = nullptr;
	std::vector<VVar*> m_LocalVars;
	bool m_NoRoot = false;
	VContext* m_Context;
	VClass* m_Owner = nullptr;

};

