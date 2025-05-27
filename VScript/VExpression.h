#pragma once
#include <vector>
#include "VClass.h"
#include "VVar.h"

class VExpression;
class VCallParameters;
class VStatementCall;
class VClassCall;

struct ExpElement {

	float FloatValue = 0;
	float IntValue = 0;
	std::string StringValue = "";
	VClass* ClassValue = nullptr;
	VVar* VarElement;
	TokenType EleType;
	VName VarName;
	TokenType OpType;
	bool IsSubExpr = false;
	VExpression* SubExpr;
	std::string ClassType = "";
	bool IsNew = false;
	VCallParameters* NewParams;
	VStatementCall* Statement = nullptr;
	VClassCall* ClassCall = nullptr;
	VExpression* IndexExpression = nullptr;
	bool IsArray = false;

};

class VExpression
{
public:

	std::vector<ExpElement> Elements;
	VContext* m_Context;
	bool Is_Float();
	bool Is_Int();
	bool Is_Object();
	bool Is_String();
	std::vector<std::string> ToVector();
	std::vector<std::string> ToOpVector();
	VVar* Express();
};

