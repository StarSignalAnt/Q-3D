#include "pch.h"
#include "VToken.h"
#include "VTokenizer.h"
#include <unordered_set>
#include <map>
std::string intToCharString(int value) {
	if (value < 0 || value > 255) {
		throw std::out_of_range("Value must be in the range 0-255");
	}
	// Convert the integer to a char
	char ch = static_cast<char>(value);
	// Construct a std::string from the char
	return std::string(1, ch);
}

bool IsOperator(char ch) {
	// Define a set of characters that are considered operators in C++/C#
	static const std::unordered_set<char> operators = {
		'(', ')', '{', '}', '[', ']', ';', ':', '?', ',', '.', '!', '@', '#',
		'%', '^', '&', '*', '-', '+', '=', '|', '~', '<', '>', '/', '\\','='
	};

	// Check if the character is in the set of operators
	return operators.find(ch) != operators.end();
}

VTokenizer::VTokenizer() {

}

VTokenStream VTokenizer::Tokenize(VSource* source) {

	std::map<std::string, TokenType> toke_map;

	toke_map["class"] = TokenType::T_Class;
	toke_map["if"] = TokenType::T_If;
	toke_map["else"] = TokenType::T_Else;
	toke_map["end"] = TokenType::T_End;
	toke_map["module"] = TokenType::T_Module;
	toke_map["."] = TokenType::T_Access;
	toke_map["int"] = TokenType::T_Int;
	toke_map["float"] = TokenType::T_Float;
	toke_map["string"] = TokenType::T_String;
	toke_map["bool"] = TokenType::T_Bool;
	toke_map[";"] = TokenType::T_EOL;
	toke_map["func"] = TokenType::T_Func;
	toke_map["function"] = TokenType::T_Func;
	toke_map["fn"] = TokenType::T_Func;
	toke_map["new"] = TokenType::T_New;
	toke_map["("] = TokenType::T_LeftPara;
	toke_map[")"] = TokenType::T_RightPara;
	toke_map["return"] = TokenType::T_Return;
	toke_map["ret"] = TokenType::T_Return;
	toke_map["["] = TokenType::T_LeftBracket;
	toke_map["]"] = TokenType::T_RightBracket;
	toke_map["else"] = TokenType::T_Else;
	toke_map["elseif"] = TokenType::T_ElseIf;
	toke_map["while"] = TokenType::T_While;
	toke_map["for"] = TokenType::T_For;
	toke_map["lambda"] = TokenType::T_Lambda;
	toke_map["invoke"] = TokenType::T_Invoke;
	toke_map["static"] = TokenType::T_Static;
	toke_map["cobject"] = TokenType::T_CObject;
	toke_map["CObject"] = TokenType::T_CObject;
	toke_map["list"] = TokenType::T_List;
	toke_map["foreach"] = TokenType::T_ForEach;
	toke_map["enum"] = TokenType::T_Enum;
	toke_map["switch"] = TokenType::T_Switch;
	toke_map["break"] = TokenType::T_Break;

	m_Source = source;

	while (true) {

		int ch = m_Source->GetNext();

		if (ch == -1) break;

		if (std::isspace(ch)) {
			
			continue;
		}

		std::string test = "";

		test += ch;

		if (test == "\"")
		{
			TokenizeString();
		}

		if (std::isalpha(ch)) {

			m_Source->Back();
			TokenizeWord();
			continue;


		}

		if (std::isdigit(ch)) {

			m_Source->Back();
			TokenizeNumber();
			continue;

		}

		if (IsOperator(ch)) {

			std::string op = intToCharString((int)ch);

			
			VToken token(TokenType::T_Operator,op);
			token.SetPlace(m_Source->Index(),0);
			m_TokenStream.AddToken(token);
			continue;
		}

		std::string str_c = intToCharString(ch);

		int b = 5;

	}

	VTokenStream new_s;

	for (auto toke : m_TokenStream.GetTokens()) {

		auto it = toke_map.find(toke.GetLex());
		if (it != toke_map.end()) {
			// Key is present, access the value using the iterator
			int value = (int)it->second;
			VToken new_token((TokenType)value, toke.GetLex());
			new_token.SetPlace(toke.GetPlace(), toke.GetEnd());
			new_s.AddToken(new_token);
			//			std::cout << "Key: " << key << ", Value: " << value << std::endl;
		}
		else {
			new_s.AddToken(toke);
			// Key is not present
			//std::cout << "Key: " << key << " not found in the map." << std::endl;
		}

	}

	return new_s;
	

}

void VTokenizer::TokenizeString() {

	std::string res = "";

	while (true) {
		auto ch = m_Source->GetNext();
		if (ch == -1) return;

		std::string test = "";
		test += ch;
		if (test == "\"")
		{
			VToken token(TokenType::T_String, res);

			m_TokenStream.AddToken(token);

			//m_Source->Back();
			return;
		}
		res += ch;

	}

}

void VTokenizer::TokenizeWord() {

	std::string word = "";
	int start = m_Source->Index();

	while (true) {

		auto ch = m_Source->GetNext();

		if (std::isalpha(ch)) {
			word += ch;
			continue;
		}
		if (std::isdigit(ch)) {
			word += ch;
			continue;
		}

		VToken token(TokenType::T_Ident, word);

		token.SetPlace(start, m_Source->Index());
		m_TokenStream.AddToken(token);

		m_Source->Back();

		return;

	}

}

void VTokenizer::TokenizeNumber() {

	std::string number = "";
	bool is_float = false;
	int start = m_Source->Index();

	while (true) {

		auto ch = m_Source->GetNext();

		if (std::isspace(ch)) {

			continue;
		}
		if (std::isdigit(ch)) {
			number += ch;
			continue;
		}
		std::string base = "";
		base += ch;
		
		if (intToCharString((int)ch) == ".")
		{
			number += ch;
			is_float = true;
			continue;
		}

		TokenType n_type = TokenType::T_Number;

		if (is_float) {
			n_type = TokenType::T_FloatNumber;
		}

		VToken token(n_type, number);
		token.SetPlace(start, m_Source->Index());
		m_TokenStream.AddToken(token);

		m_Source->Back();

		return;


	}
}