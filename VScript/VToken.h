#pragma once
#include<string>

enum TokenType {
	T_Ident, T_Number, T_If, T_Else, T_End, T_Then, T_While, T_For, T_Func, T_Meth, T_Operator, T_None, T_Class, T_Module, T_Access,
	T_Int, T_Float, T_String, T_Bool, T_EOL,
	T_Plus,T_Minus,T_Times,T_Divide,T_FloatNumber,T_New,T_Equal,T_LeftPara,T_RightPara,T_Return,T_LeftBracket,T_RightBracket,
	T_ElseIf,T_GreaterThan,T_LessThan,T_Lambda,T_Invoke,T_Static,T_CObject,T_List,T_ListAdd,T_ListRemove,T_ForEach,T_Enum,T_Switch,T_Break
};

class VToken
{
public:

	VToken(TokenType type, std::string lexeme);
	void SetPlace(int start, int end);
	std::string GetLex();
	TokenType GetType();
	int GetPlace() {
		return m_SourceStart;
	}
	int GetEnd() {
		return m_SourceEnd;
	}
private:

	TokenType m_TokenType = TokenType::T_None;
	std::string m_Lexeme = "";
	int m_SourceStart = 0;
	int m_SourceEnd = 0;

};


