#include "pch.h"
#include "VToken.h"

VToken::VToken(TokenType type, std::string lexeme) {

	m_TokenType = type;
	m_Lexeme = lexeme;

}

void VToken::SetPlace(int start, int end) {

	m_SourceStart = start;
	m_SourceEnd = end;

}

std::string VToken::GetLex() {

	return m_Lexeme;

}

TokenType VToken::GetType() {

	return m_TokenType;

}