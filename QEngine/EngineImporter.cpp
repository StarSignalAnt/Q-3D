#include "EngineImporter.h"
#include "VFile.h"

GraphNode* EngineImporter::ImportMesh(std::string path) {

	VFile* f = new VFile(path.c_str(), FileMode::Read);

	int mc = f->ReadInt();

//	StaticMeshComponent* mesh_com = new StaticMeshComponent;

	StaticMeshComponent* comp = new StaticMeshComponent;

	comp->mesh_path = path;

	for (int i = 0; i < mc; i++) {
		

		SubMesh* sm = new SubMesh;

		sm->Read(f);

		comp->AddSubMesh(sm);
		//comp->Read(f);
		//comp->Finalize();

		//int b = 5;
		
		

	}

	comp->Finalize();

	GraphNode* node = new GraphNode;
	node->AddComponent(comp);
	node->AddComponent(new StaticRendererComponent);
	node->AddComponent(new StaticDepthRendererComponent);
	

	f->Close();

	return node;

}