#pragma once
#include "StaticMeshComponent.h"
#include "StaticRendererComponent.h"
#include "StaticDepthRendererComponent.h" 

class EngineImporter
{
public:

	static GraphNode* ImportMesh(std::string path);

};

