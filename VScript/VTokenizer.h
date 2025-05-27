#pragma once
#include "VSource.h"
#include "VTokenStream.h"

class VTokenizer
{
public:

	VTokenizer();
	VTokenStream Tokenize(VSource* source);
	void TokenizeWord();
	void TokenizeNumber();
	void TokenizeString();

private:

	VSource* m_Source;
	VTokenStream m_TokenStream;

};

