#pragma once
#include <vector>
#include <string>
#include "VToken.h"

class VTokenStream
{
public:
	VTokenStream();
	void AddToken(VToken token);
	VToken GetNext();
	VToken Peek(int delta);
	std::vector<VToken> GetTokens();
	bool End();
	void Back();
	bool LineHas(TokenType type);
	bool LineHas(std::string lex, std::string except = "");
	bool Match(std::initializer_list<TokenType> args);
	void ToNext(std::string ex) {
		int index = m_Index;
		while (true) {

			if (Peek(0).GetLex() == ex)
			{
				return;
			}
			else {
				GetNext();
				if (m_Index >= m_Tokens.size())
				{
					m_Index = index;
					return;
				}
			}

		}
	}
	void ToPrev(std::string ex) {

		int p_index = m_Index;
		while (true) {

			if (Peek(0).GetLex() == ex)
			{
				return;
			}
			else {
				Back();
				if (m_Index < 0) {
					m_Index = p_index;
					return;
				}
			}

		}

	}
	int GetIndex() {
		return m_Index;
	}

private:
	std::vector<VToken> m_Tokens;
	int m_Index = 0;
};


