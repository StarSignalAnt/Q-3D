#include "StaticRendererComponent.h"
#include "StaticMeshComponent.h"
#include "CameraComponent.h"
#include "SceneGraph.h"
#include "GraphNode.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "QEngine.h"

void StaticRendererComponent::OnRender(GraphNode* camera) {

	auto mesh = m_Owner->GetComponent<StaticMeshComponent>();
	int b = 5;

	if (!camera->GetComponent<CameraComponent>()->InFrustum(m_Owner->GetStaticMeshBounds()))
	{
		return;
	}
	SceneGraph::Ren_Count++;

	

	bool add = false;
	for (auto light : SceneGraph::m_CurrentGraph->GetLights()) {


		
		for (auto& sub : mesh->GetSubMeshes()) {

			auto mat = sub->m_Material;
			mat->SetIndexCount(sub->m_Triangles.size() * 3);
			mat->SetBuffer(sub->VertexBuffer, 0);
			mat->SetBuffer(sub->IndexBuffer, 1);
			mat->SetMatrix(glm::inverse(camera->GetWorldMatrix()), 0);
			mat->SetMatrix(m_Owner->GetWorldMatrix(), 1);
			mat->SetMatrix(camera->GetComponent<CameraComponent>()->GetProjectionMatrix(), 2);
			mat->SetCameraPosition(camera->GetPosition());
			mat->SetCameraExt(camera->GetComponent<CameraComponent>()->GetExtents());
			//	mat->SetTexture(sub.m_Material->GetColorTexture(), 0);
			mat->SetLight(light);
			mat->Bind(add);

			mat->Render();


		}
		add = true;

	}

}