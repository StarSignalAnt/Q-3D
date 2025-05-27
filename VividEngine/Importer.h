#pragma once
#include <vector>
#include <string>

class Mesh3D;
class NodeEntity;
class Node;
class NodeActor;

class Importer
{
public:

	Node* ImportNode(std::string path);
	Node* ImportActor(std::string path);
	Node* ImportAnims(NodeActor* load,std::string path);

private:

};

