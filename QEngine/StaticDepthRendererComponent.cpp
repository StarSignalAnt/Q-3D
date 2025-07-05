#include "StaticDepthRendererComponent.h"
#include "StaticMeshComponent.h"
#include "CameraComponent.h"
#include "SceneGraph.h"
#include "GraphNode.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void StaticDepthRendererComponent::OnRenderDepth(GraphNode* camera) {

	auto mesh = m_Owner->GetComponent<StaticMeshComponent>();
	int b = 5;
	if (!camera->GetComponent<CameraComponent>()->InFrustum(m_Owner->GetStaticMeshBounds()))
	{
	//	return;
	}
	//if (m_Owner->GetRenderType() != NodeRenderType::RenderType_Dynamic) return;


		for (auto sub : mesh->GetSubMeshes()) {

			auto& lod = sub->m_LODs[0];

			auto mat = sub->m_DepthMaterial;
			mat->SetIndexCount(lod->m_Triangles.size() * 3);
			mat->SetBuffer(lod->VertexBuffer, 0);
			mat->SetBuffer(lod->IndexBuffer, 1);
			mat->SetMatrix(glm::inverse(camera->GetWorldMatrix()), 0);
			mat->SetMatrix(m_Owner->GetWorldMatrix(), 1);
			mat->SetMatrix(camera->GetComponent<CameraComponent>()->GetProjectionMatrix(), 2);
			mat->SetCameraPosition(camera->GetPosition());
			mat->SetCameraExt(camera->GetComponent<CameraComponent>()->GetExtents());
			//	mat->SetTexture(sub.m_Material->GetColorTexture(), 0);
		
			mat->Bind(false);

			mat->Render();


		}


}