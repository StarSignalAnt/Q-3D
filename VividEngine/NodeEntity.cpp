#include "pch.h"
#include "NodeEntity.h"
#include "MaterialBase.h"
#include "MaterialDepth.h"
#include <windows.h>
#include "Mesh3D.h"
#include "Engine.h"
#include "Bounds.h"
#include "VFile.h"
#include "Importer.h"
#include "MaterialMeshPBR.h"
NodeEntity::NodeEntity() {

	m_Static = true;
	

}

void NodeEntity::AddMesh(Mesh3D* mesh) {

	m_Meshes.push_back(mesh);
	mesh->SetOwner(this);

}

int NodeEntity::MeshesCount() {

	return m_Meshes.size();

}

Mesh3D* NodeEntity::GetMesh(int index) {

	return m_Meshes[index];

}

std::vector<Mesh3D*> NodeEntity::GetMeshes() {

	return m_Meshes;

}

void NodeEntity::RenderDepth() {

	Engine::m_Node = this;
	for (auto mesh : m_Meshes) {
	
		if (mesh->GetDepthMaterial() == nullptr) continue;
		mesh->GetDepthMaterial()->Bind(false);
		mesh->RenderDepth();

	}

	RenderDepthChildren();

}

void NodeEntity::Render(bool sp) {

//	printf("Rendering NodeEntity.\n");
	//OutputDebugStringA("Rendering NodeEntity.\n");
	
	Engine::m_Node = this;


	if (m_Enabled) {
	
			for (auto mesh : m_Meshes) {
			//	mesh->GetMaterial()->Bind(sp);
				mesh->Render(sp);
				

			}


		RenderChildren(sp);
	}
}

void NodeEntity::SetMaterial(MaterialBase* material) {

	for (auto mesh : m_Meshes) {

		mesh->SetMaterial(material);

	}

}

bool NodeEntity::IsMeshIndex(Mesh3D* mesh, int idx)
{

	int i = 0;
	for (auto me : m_Meshes) {

		if (me == mesh) {
			if (i == idx) {
				return true;
			}
		}
		i++;
	}

	return false;

}

void NodeEntity::BuildGeo() {

	



	int b = 5;
	for (auto m : m_Meshes) {

		//m->BuildGeo();
		m->NeedsRebuild();
		
	}
	for (auto n : m_Nodes) {
		n->BuildGeo();
	}

}

void NodeEntity::ForceRebuild() {

	//BuildGeo();

	for (auto m : m_Meshes) {

	
		m->BuildGeo();

	}

}

Bounds* NodeEntity::GetBounds() {

	Bounds* res = new Bounds;
	res->Min = float3(10000, 10000, 10000);
	res->Max = float3(-10000, -10000, -10000);
	res->Centre = float3(0, 0, 0);

	for (auto m : m_Meshes) {


		auto world = float4x4::Identity();

		auto tris = m->GetTris();
		auto verts = m->GetVertices();

		for (auto v : verts) {

			float3 npos = v.position * world;
			if (npos.x < res->Min.x) res->Min.x = npos.x;
			if (npos.x > res->Max.x) res->Max.x = npos.x;
			if (npos.y < res->Min.y) res->Min.y = npos.y;
			if (npos.y > res->Max.y) res->Max.y = npos.y;
			if (npos.z < res->Min.z) res->Min.z = npos.z;
			if (npos.z > res->Max.z) res->Max.z = npos.z;

		}


	}

	res->Centre.x = res->Min.x + (res->Max.x - res->Min.x) / 2.0f;
	res->Centre.y = res->Min.y + (res->Max.y - res->Min.y) / 2.0f;
	res->Centre.z = res->Min.z + (res->Max.z - res->Min.z) / 2.0f;

	return res;
}

void NodeEntity::WriteNode(VFile* file) {

	file->WriteInt(2);
	file->WriteString(m_ResourcePath.c_str());
	file->WriteVec3(m_Position);
	file->WriteMatrix(m_Rotation);
	file->WriteVec3(m_Scale);
	file->WriteString(m_Name.c_str());

	WriteScripts(file);


	file->WriteInt(m_Nodes.size());

	for (auto node : m_Nodes) {

		node->WriteNode(file);

	}

}

void NodeEntity::ReadNode(VFile* file) {

	m_ResourcePath = file->ReadString();
	m_Position = file->ReadVec3();
	m_Rotation = file->ReadMatrix();
	m_Scale = file->ReadVec3();
	m_Name = file->ReadString();

	Importer* import = new Importer;
	auto imp = (NodeEntity*)import->ImportNode(m_ResourcePath);


	for (auto mesh : imp->GetMeshes()) {

		AddMesh(mesh);

	}

}


void NodeEntity::SaveFastNode(std::string path) {

	VFile* file = new VFile(path.c_str(), FileMode::Write);

	file->WriteString(m_Name.c_str());
	file->WriteInt(m_Meshes.size());

	for (auto v : m_Meshes)
	{
		
		auto verts = v->GetVertexData();
		auto tris = v->GetTriData();

		int vsize = v->GetVertices().size() * 27 * 4;
		int tsize = v->GetTris().size() * 3 * 4;

		file->WriteInt(v->GetVertices().size());
		file->WriteInt(vsize);
		file->WriteBytes((void*)verts, vsize);
		file->WriteInt(v->GetTris().size());
		file->WriteInt(tsize);
		file->WriteBytes((void*)tris, tsize);

		auto mat = v->GetMaterial();
		file->WriteString(mat->GetPath().c_str());



	}

	file->Close();

}

void NodeEntity::LoadFastNode(std::string path) {


	VFile* file = new VFile(path.c_str(), FileMode::Read);

	m_Name = file->ReadString();

	int mc = file->ReadInt();

	for (int i = 0; i < mc; i++) {

		Mesh3D* mesh = new Mesh3D;

		int num_verts = file->ReadInt();
		int vsize = file->ReadInt();
		float* vdata = (float*)file->ReadBytes(vsize);
		int num_tris = file->ReadInt();
		int tsize = file->ReadInt();
		Uint32* tdata = (Uint32*)file->ReadBytes(tsize);
		std::string mat = file->ReadString();


		int vi = 0;
		for (int v = 0; v < num_verts; v++) {

			Vertex nv;
			nv.position.x = vdata[vi++];
			nv.position.y = vdata[vi++];
			nv.position.z = vdata[vi++];
			nv.color.x = vdata[vi++];
			nv.color.y = vdata[vi++];
			nv.color.z = vdata[vi++];
			nv.color.w = vdata[vi++];
			nv.texture.x = vdata[vi++];
			nv.texture.y = vdata[vi++];
			nv.texture.z = vdata[vi++];
			nv.normal.x = vdata[vi++];
			nv.normal.y = vdata[vi++];
			nv.normal.z = vdata[vi++];
			nv.binormal.x = vdata[vi++];
			nv.binormal.y = vdata[vi++];
			nv.binormal.z = vdata[vi++];
			nv.tangent.x = vdata[vi++];
			nv.tangent.y = vdata[vi++];
			nv.tangent.z = vdata[vi++];
			nv.bone_ids.x = vdata[vi++];
			nv.bone_ids.y = vdata[vi++];
			nv.bone_ids.z = vdata[vi++];
			nv.bone_ids.w = vdata[vi++];
			nv.bone_weights.x = vdata[vi++];
			nv.bone_weights.y = vdata[vi++];
			nv.bone_weights.z = vdata[vi++];
			nv.bone_weights.w = vdata[vi++];
			mesh->AddVertex(nv,false);


		}
		vi = 0;
		for (int t = 0; t < num_tris; t++) {
			Triangle nt;
			nt.v0 = tdata[vi++];
			nt.v1 = tdata[vi++];
			nt.v2 = tdata[vi++];
			mesh->AddTri(nt);
		}
	
		//mesh->BuildFast(vdata, tdata, num_verts, num_tris);

		mesh->SetDepthMaterial(new MaterialDepth);
		mesh->Build();
		AddMesh(mesh);

		auto lmat = Engine::FindActiveMaterial(mat);

		if (lmat != nullptr) {
			//materials.push_back(lmat);
			mesh->SetMaterial(lmat);
			continue;
		}

		if (VFile::Exists(mat.c_str())) {


			auto v_mat = (MaterialMeshPBR*)MaterialBase::LoadMaterial(mat);
			//materials.push_back(v_mat);

			Engine::m_ActiveMaterials.push_back(v_mat);
			mesh->SetMaterial(v_mat);
			continue;

		}


		//Engine::m_ActiveMaterials.push_back(v_mat);
		mesh->SetMaterial(MaterialBase::LoadMaterial(mat));
		
		Engine::m_ActiveMaterials.push_back(mesh->GetMaterial());

		//mesh->Build();
		

	}

	file->Close();

}

void NodeEntity::RenderForcedMaterial(MaterialBase* material) {
	Engine::m_Node = this;
	int b = 5;
	for (auto mesh : m_Meshes) {

		auto tmp = mesh->GetMaterial();
		material->CopyMaps(mesh->GetMaterial());
		mesh->SetMaterial(material);
		material->Bind(false);
		mesh->Render(false);
		mesh->SetMaterial(tmp);



	}

}