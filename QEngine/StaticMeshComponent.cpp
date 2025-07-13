#include "StaticMeshComponent.h"
#include "QEngine.h"
#include "MaterialProducer.h"
#include "EngineImporter.h"

void StaticMeshComponent::Finalize() {


	

	for (auto sub : m_SubMeshes) {


		for (auto& lod : sub->m_LODs) {


			float* data = new float[lod->m_Vertices.size() * 19];

			int i = 0;
			for (auto v : lod->m_Vertices) {

				data[i++] = v.position.x;
				data[i++] = v.position.y;
				data[i++] = v.position.z;
				data[i++] = v.color.x;
				data[i++] = v.color.y;
				data[i++] = v.color.z;
				data[i++] = v.color.w;
				data[i++] = v.uv.x;
				data[i++] = v.uv.y;
				data[i++] = v.uv.z;
				data[i++] = v.normal.x;
				data[i++] = v.normal.y;
				data[i++] = v.normal.z;
				data[i++] = v.binormal.x;
				data[i++] = v.binormal.y;
				data[i++] = v.binormal.z;
				data[i++] = v.tangent.x;
				data[i++] = v.tangent.y;
				data[i++] = v.tangent.z;

			}

			BufferDesc vbuf;
			vbuf.Name = "Mesh Buffer VB";
			vbuf.Usage = USAGE_IMMUTABLE;
			vbuf.BindFlags = BIND_VERTEX_BUFFER;
			vbuf.Size = (Uint64)(sizeof(Vertex3) * lod->m_Vertices.size());

			BufferData vdata;
			vdata.DataSize = vbuf.Size;
			vdata.pData = data;

			Q3D::Engine::QEngine::GetDevice()->CreateBuffer(vbuf, &vdata, &lod->VertexBuffer);

			BufferDesc ibuf;

			ibuf.Name = "Mesh Buffer IB";
			ibuf.Usage = USAGE_IMMUTABLE;
			ibuf.BindFlags = BIND_INDEX_BUFFER;
			ibuf.Size = (Uint64)(sizeof(Uint32) * lod->m_Triangles.size() * 3);

			Uint32* indices = new Uint32[lod->m_Triangles.size() * 3];

			i = 0;
			for (auto t : lod->m_Triangles)
			{
				indices[i++] = t.v0;
				indices[i++] = t.v1;
				indices[i++] = t.v2;
			}


			BufferData idata;
			idata.DataSize = ibuf.Size;
			idata.pData = indices;

			Q3D::Engine::QEngine::GetDevice()->CreateBuffer(ibuf, &idata, &lod->IndexBuffer);

		}
	}
}



void StaticMeshComponent::Initialize() {

	auto node = EngineImporter::ImportMesh(mesh_path);
	auto mesh = node->GetComponent<StaticMeshComponent>();

	for (auto sub : mesh->GetSubMeshes()) {
		AddSubMesh(sub);
		
	}






	int b = 5;

}