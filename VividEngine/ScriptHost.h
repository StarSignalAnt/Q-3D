#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
#include <string>
#include "Pars.h"
#include <unordered_map>
enum class VarType {
	T_Int,
	T_Float,
	T_Bool,
	T_String,
	T_Class,
	T_CPtr,
	T_Object,  // for other python objects/classes
	T_Unknown
};

std::string VarTypeToString(VarType type);

template<typename Func>
void addFunction(const std::string& name, Func func);

// Add a function with description
template<typename Func>
void addFunction(const std::string& name, Func func, const std::string& description);

class ScriptHost
{
public:

	ScriptHost();
	static ScriptHost* m_Host;
	void Load(std::string name);
	void* CreateInstance(std::string name);
	void TestFunc(void* inst,std::string name);
	void callFunc(void* obj, const std::string& method_name, const Pars& pars);
	int GetInt(void* inst, std::string name);
	void* GetIntPtr(void* inst, const std::string& name);
	std::string GetString(void* inst, const std::string& name);
	void* GetClass(void* inst, const std::string& name);
	float GetFloat(void* inst, const std::string& name);
	bool GetBool(void* inst, const std::string& name);
	void SetInt(void* inst, const std::string& name, int value);
	void SetFloat(void* inst, const std::string& name, float value);
	void SetBool(void* inst, const std::string& name, bool value);
	void SetString(void* inst, const std::string& name, std::string value);
	void SetIntPtr(void* inst,const std::string& name,void* val);
	void SetPtr(void* inst, const std::string& name, uintptr_t value);
	void SetClass(void* inst, const std::string& name, void* classPtr);
	std::unordered_map<std::string, VarType> GetVarNames(void* inst);


private:







};

