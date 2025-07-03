#pragma once
#include <string>

class GraphNode;

class Importer
{
public:

	static GraphNode* ImportEntity(std::string path,bool gen_lod = false);
	static GraphNode* ImportSkeletal(std::string path);

};

