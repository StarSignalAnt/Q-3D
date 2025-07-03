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


	//if (m_Owner->GetRenderType() != NodeRenderType::RenderType_Dynamic) return;


	if (!camera->GetComponent<CameraComponent>()->InFrustum(m_Owner->GetStaticMeshBounds()))
	{
		return;
	}
	SceneGraph::Ren_Count++;



	bool add = false;
	for (auto light : SceneGraph::m_CurrentGraph->GetLights()) {


		
		for (auto& sub : mesh->GetSubMeshes()) {



			LODLevel* lod;

			if (sub->m_LODs.size() > 1) {


				lod = sub->m_LODs[0];
			}
			else {
				lod = sub->m_LODs[0];
			}


			//auto& lod = sub->m_LODs[0];

			auto mat = sub->m_Material;
			mat->SetIndexCount(lod->m_Triangles.size() * 3);
			mat->SetBuffer(lod->VertexBuffer, 0);
			mat->SetBuffer(lod->IndexBuffer, 1);
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

void StaticRendererComponent::OnRenderDirect(GraphNode* light, GraphNode* camera) {
	auto mesh = m_Owner->GetComponent<StaticMeshComponent>();
	if (!camera->GetComponent<CameraComponent>()->InFrustum(m_Owner->GetStaticMeshBounds()))
	{
		return;
	}
	for (auto& sub : mesh->GetSubMeshes()) {



		LODLevel* lod;

		if (sub->m_LODs.size() > 1) {


			lod = sub->m_LODs[0];
		}
		else {
			lod = sub->m_LODs[0];
		}


		//auto& lod = sub->m_LODs[0];

		auto mat = sub->m_Material;
		mat->SetIndexCount(lod->m_Triangles.size() * 3);
		mat->SetBuffer(lod->VertexBuffer, 0);
		mat->SetBuffer(lod->IndexBuffer, 1);
		mat->SetMatrix(glm::inverse(camera->GetWorldMatrix()), 0);
		mat->SetMatrix(m_Owner->GetWorldMatrix(), 1);
		mat->SetMatrix(camera->GetComponent<CameraComponent>()->GetProjectionMatrix(), 2);
		mat->SetCameraPosition(camera->GetPosition());
		mat->SetCameraExt(camera->GetComponent<CameraComponent>()->GetExtents());
		//	mat->SetTexture(sub.m_Material->GetColorTexture(), 0);
		mat->SetLight(light);
		mat->Bind(false);

		mat->Render();



	}

	//add = true;

}