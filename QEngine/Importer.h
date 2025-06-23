#pragma once
#include <string>

class GraphNode;

class Importer
{
public:

	static GraphNode* ImportEntity(std::string path);
	static GraphNode* ImportSkeletal(std::string path);

};

