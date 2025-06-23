#include "LinesRendererComponent.h"
#include "QEngine.h"
#include "CameraComponent.h"

LinesRendererComponent::LinesRendererComponent() {

	m_Material = new MaterialLines;

}

void LinesRendererComponent::AddVertex(LineVertex vertex) {

	m_Vertices.push_back(vertex);

}

void LinesRendererComponent::AddLine(Line line) {

	m_Lines.push_back(line);

}

void LinesRendererComponent::Finalize() {



		float* data = new float[m_Vertices.size() * 7];

		int i = 0;
		for (auto v : m_Vertices) {

			data[i++] = v.position.x;
			data[i++] = v.position.y;
			data[i++] = v.position.z;
			data[i++] = v.color.x;
			data[i++] = v.color.y;
			data[i++] = v.color.z;
			data[i++] = v.color.w;


		}

		BufferDesc vbuf;
		vbuf.Name = "Lines Buffer VB";
		vbuf.Usage = USAGE_IMMUTABLE;
		vbuf.BindFlags = BIND_VERTEX_BUFFER;
		vbuf.Size = (Uint64)(sizeof(LineVertex) * m_Vertices.size());

		BufferData vdata;
		vdata.DataSize = vbuf.Size;
		vdata.pData = data;

		QEngine::m_pDevice->CreateBuffer(vbuf, &vdata, &VertexBuffer);

		BufferDesc ibuf;

		ibuf.Name = "Lines Index Buffer IB";
		ibuf.Usage = USAGE_IMMUTABLE;
		ibuf.BindFlags = BIND_INDEX_BUFFER;
		ibuf.Size = (Uint64)(sizeof(Uint32) * m_Lines.size() * 2);

		Uint32* indices = new Uint32[m_Lines.size() * 2];

		i = 0;
		for (auto t : m_Lines)
		{
			indices[i++] = t.v0;
			indices[i++] = t.v1;
	
		}


		BufferData idata;
		idata.DataSize = ibuf.Size;
		idata.pData = indices;

		QEngine::m_pDevice->CreateBuffer(ibuf, &idata, &IndexBuffer);


}

void LinesRendererComponent::OnRender(GraphNode* camera) {



	//	bool add = false;
		//for (auto light : SceneGraph::m_CurrentGraph->GetLights()) {



			//for (auto& sub : mesh->GetSubMeshes()) {

	auto mat = m_Material;
	mat->SetIndexCount(m_Lines.size() * 2);
	mat->SetBuffer(VertexBuffer, 0);
	mat->SetBuffer(IndexBuffer, 1);
	mat->SetMatrix(glm::inverse(camera->GetWorldMatrix()), 0);
	mat->SetMatrix(m_Owner->GetWorldMatrix(), 1);
	mat->SetMatrix(camera->GetComponent<CameraComponent>()->GetProjectionMatrix(), 2);
	mat->SetCameraPosition(camera->GetPosition());
	mat->SetCameraExt(camera->GetComponent<CameraComponent>()->GetExtents());
	//	mat->SetTexture(sub.m_Material->GetColorTexture(), 0);
	//mat->SetLight(light);
	mat->Bind(false);
	mat->Render();



	//add = true;

//}

}