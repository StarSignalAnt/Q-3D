#pragma once
#include <vector>
#include "Node.h"


class Bounds;
class MaterialBase;
class Mesh3D;

class NodeEntity : public Node
{
public:

	NodeEntity();
	void AddMesh(Mesh3D* mesh);
	int MeshesCount();
	Mesh3D* GetMesh(int index);
	std::vector<Mesh3D*> GetMeshes();
	void Render(bool sp);
	void RenderForcedMaterial(MaterialBase* material);

	void RenderDepth();
	void SetMaterial(MaterialBase* material);
	bool IsMeshIndex(Mesh3D* mesh, int index);
	void BuildGeo();
	void ForceRebuild();
	Bounds* GetBounds();
	void WriteNode(VFile* file);
	void ReadNode(VFile* file);
	void SaveFastNode(std::string path);
	void LoadFastNode(std::string path);

protected:

	std::vector<Mesh3D*> m_Meshes;



};

