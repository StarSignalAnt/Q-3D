#pragma once
#include "VToken.h"
#include "VName.h"

class VExpression;

class VParameter
{
public:

	VParameter();
	void SetName(VName name);
	void SetDefaultExpression(VExpression* expr);
	void SetType(TokenType type);
	TokenType GetType();
	VName GetName();
	VExpression* GetExpression();
	void SetClassType(std::string type)
	{
		m_ClassType = type;
	}
	std::string GetClassType() {
		return m_ClassType;
	}
private:

	TokenType m_Type;
	VName m_Name;
	std::string m_ClassType;
	VExpression* m_Default;


};

