#pragma once
#include <string>
#include <vector>
#include <functional>
#include "BasicMath.hpp"

using namespace Diligent;

class VContext;
class VClass;
class VVar;
class VModule;
class VFile;


class ScriptHost
{
public:

	ScriptHost();
	VModule* LoadModule(std::string path);
	VClass* CreateInstance(std::string name);
	void AddCFunction(std::string name, std::function<VVar* (const std::vector<VVar*>& args)> func);
	VVar* CRVec3(float x, float y, float z);
	float3 GetVec3(VClass* cls);
	void WriteContext(VFile* file);
	void ReadContext(VFile* file);
	static ScriptHost* m_This;
	VContext* GetContext() {
		return m_Context;
	}
private:

	VContext* m_Context;
	VModule* m_Vec3, * m_Vec4;
	VModule* m_Matrix;
	VModule* m_Node;
	VModule* m_GS;
	VModule* m_Input;
	VModule* m_Maths;
	std::vector<std::string> m_ModPaths;

};



