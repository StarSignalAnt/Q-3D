#pragma once
#include "VName.h"
#include <string>
#include <vector>

class VDefineParams;
class VContext;
class VVar;
class VCodeBody;
class VClass;
class VScope;
class VCallParameters;
class VExpression;

class VFunction
{
public:

	VFunction();
	void SetName(VName name);
	VName GetName();
	void SetParams(VDefineParams* params);
	VDefineParams* GetParams();
	void SetContext(VContext* context);
	VFunction* Clone();
	VVar* Call(const std::vector<VVar*>& args);
	VVar* Call(VCallParameters* params);

	void SetBody(VCodeBody* code);
	void SetOwner(VClass* cls);
	VScope* GetScope();
	void SetGuard(VExpression* expression);
	VCodeBody* GetCode() {
		return m_Code;
	}
private:

	VName m_Name;
	VDefineParams* m_Params;
	VContext* m_Context;
	VCodeBody* m_Code;
	VClass* m_ClassOwner = nullptr;
	VExpression* m_Guard = nullptr;

};

