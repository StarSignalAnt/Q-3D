#include "pch.h"
#include "VTokenStream.h"

VTokenStream::VTokenStream() {

}

void VTokenStream::AddToken(VToken token) {

	m_Tokens.push_back(token);

}

VToken VTokenStream::GetNext() {

	if (m_Index >= m_Tokens.size()) {
		return VToken(T_End, "end");
	}
	auto token = m_Tokens[m_Index++];
	return token;

}

VToken VTokenStream::Peek(int delta) {

	if (m_Index + delta >= m_Tokens.size())
	{
		VToken r(T_End, "end");
		return r;
	}
	return m_Tokens[m_Index + delta];

}

std::vector<VToken> VTokenStream::GetTokens() {

	return m_Tokens;

}

bool VTokenStream::End() {

	if (m_Index >= m_Tokens.size()) return true;
	return false;

}

void VTokenStream::Back() {

	m_Index--;

}

bool VTokenStream::LineHas(TokenType type) {
	int idx = 0;
	while (true) {

		auto token = Peek(idx);
		if (token.GetType() == TokenType::T_EOL)
		{
			return false;
		}
		if (token.GetType() == type) {
			return true;
		}
		idx++;

	}
	return false;
}

bool VTokenStream::LineHas(std::string lex,std::string except) {
	int idx = 0;
	while (true) {

		auto token = Peek(idx);
		if (token.GetLex() == except)
		{
			return false;
		}
		if (token.GetType() == TokenType::T_EOL)
		{
			return false;
		}
		if (token.GetLex() == lex) {
			return true;
		}
		if (token.GetType() == T_End)
		{
			return false;
		}
		idx++;

	}
	return false;
}

bool VTokenStream::Match(std::initializer_list<TokenType> args)
{
	int idx = m_Index;

	for (const auto& v : args) {
		if (v == m_Tokens[idx].GetType())
		{

		}
		else {
			return false;
		}
		idx++;
		//	std::cout << v << std::endl;
	}
	return true;

}