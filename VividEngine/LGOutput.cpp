#include "LGOutput.h"

LGOutput::LGOutput(LNode* parent, int portIndex, std::string name, DataType type)
	: m_parentNode(parent), m_portIndex(portIndex), m_Name(name), m_Type(type)
{
	// The initializer list handles all member setup.
}

void LGOutput::addConnection(LGInput* input) {
	m_connections.push_back(input);
}