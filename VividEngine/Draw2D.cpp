#include "pch.h"
#include "Draw2D.h"
#include "Texture2D.h"
#include "MaterialBasic3D.h"
#include "StaticMeshComponent.h"
#include "CameraComponent.h"

Draw2D::Draw2D(GraphNode* node) {

	m_Material = new MaterialBasic3D;
	m_Camera = node;

}

void Draw2D::Rect(Texture2D* image, glm::vec2 pos, glm::vec2 size,glm::vec4 color) {

	auto smesh = new StaticMeshComponent;


	Vertex3 v1, v2, v3, v4;

	v1.position = glm::vec3(pos.x, pos.y,0.1f);
	v2.position = glm::vec3(pos.x + size.x, pos.y, 0.1f);
	v3.position = glm::vec3(pos.x + size.x, pos.y + size.y, 0.1f);
	v4.position = glm::vec3(pos.x, pos.y + size.y, 0.1f);
	v1.color = color;
	v2.color = color;
	v3.color = color;
	v4.color = color;
	v1.uv = glm::vec3(0, 0, 0);
	v2.uv = glm::vec3(1, 0, 0);
	v3.uv = glm::vec3(1, 1, 0);
	v4.uv = glm::vec3(0, 1, 0);

	SubMesh* mesh = new SubMesh;

	mesh->m_Vertices.push_back(v1);
	mesh->m_Vertices.push_back(v2);
	mesh->m_Vertices.push_back(v3);
	mesh->m_Vertices.push_back(v4);

	Tri3 t1, t2;

	t1.v0 = 0;
	t1.v1 = 1;
	t1.v2 = 2;

	t2.v0 = 2;
	t2.v1 = 3;
	t2.v2 = 0;

	mesh->m_Triangles.push_back(t1);
	mesh->m_Triangles.push_back(t2);

	smesh->AddSubMesh(mesh);

	smesh->Finalize();

	RenderMaterial* mat = (RenderMaterial*)m_Material;
	if (m_Override != nullptr) {
		mat = m_Override;
	}
	mat->SetIndexCount(mesh->m_Triangles.size() * 3);
	mat->SetBuffer(mesh->VertexBuffer, 0);
	mat->SetBuffer(mesh->IndexBuffer, 1);
	mat->SetMatrix(glm::mat4(1.0f), 0);
	mat->SetMatrix(glm::mat4(1.0f), 1);
	mat->SetMatrix(m_Camera->GetComponent<CameraComponent>()->Get2DProjectionMatrix(), 2);
	mat->SetCameraPosition(m_Camera->GetPosition());
	mat->SetCameraExt(m_Camera->GetComponent<CameraComponent>()->GetExtents());
	mat->SetTexture(image, 0);
	//	mat->SetTexture(sub.m_Material->GetColorTexture(), 0);
	//mat->SetLight(light);
	//mat->SetColorTexture(image);

	mat->Bind(false);

	mat->Render();


}