#include "StaticRendererComponent.h"
#include "StaticMeshComponent.h"
#include "CameraComponent.h"
#include "GraphNode.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void StaticRendererComponent::OnRender(GraphNode* camera) {

	auto mesh = m_Owner->GetComponent<StaticMeshComponent>();
	int b = 5;

	for (auto& sub : mesh->GetSubMeshes()) {

		auto mat = sub.m_Material;
		mat->SetIndexCount(sub.m_Triangles.size() * 3);
		mat->SetBuffer(sub.VertexBuffer, 0);
		mat->SetBuffer(sub.IndexBuffer, 1);
		mat->SetMatrix(glm::inverse(camera->GetWorldMatrix()), 0);
		mat->SetMatrix(m_Owner->GetWorldMatrix(), 1);
		mat->SetMatrix(camera->GetComponent<CameraComponent>()->GetProjectionMatrix(), 2);
		mat->SetCameraPosition(camera->GetPosition());	
	//	mat->SetTexture(sub.m_Material->GetColorTexture(), 0);
		mat->Bind();
		mat->Render();
	

	}

}