#include "SkeletalRendererComponent.h"
#include "SkeletalMeshComponent.h"
#include "CameraComponent.h"
#include "SceneGraph.h"
#include "GraphNode.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 DiligentToGLM(const Diligent::float4x4& diligentMatrix)
{
	// Diligent Engine uses row-major order by default
	// GLM uses column-major order
	// We need to transpose during conversion

	return glm::mat4(
		diligentMatrix._11, diligentMatrix._21, diligentMatrix._31, diligentMatrix._41,  // Column 0
		diligentMatrix._12, diligentMatrix._22, diligentMatrix._32, diligentMatrix._42,  // Column 1
		diligentMatrix._13, diligentMatrix._23, diligentMatrix._33, diligentMatrix._43,  // Column 2
		diligentMatrix._14, diligentMatrix._24, diligentMatrix._34, diligentMatrix._44   // Column 3
	);
}

void SkeletalRendererComponent::OnRender(GraphNode* camera) {

	auto mesh = m_Owner->GetComponent<SkeletalMeshComponent>();
	int b = 5;

	auto bones = mesh->GetBones();





	bool add = false;
	for (auto light : SceneGraph::m_CurrentGraph->GetLights()) {



		for (auto& sub : mesh->GetSubMeshes()) {

			auto mat = sub.m_Material;
			mat->SetIndexCount(sub.m_Triangles.size() * 3);
			mat->SetBuffer(sub.VertexBuffer, 0);
			mat->SetBuffer(sub.IndexBuffer, 1);
			mat->SetMatrix(glm::inverse(camera->GetWorldMatrix()), 0);
			mat->SetMatrix(m_Owner->GetWorldMatrix(), 1);
			mat->SetMatrix(camera->GetComponent<CameraComponent>()->GetProjectionMatrix(), 2);
			mat->SetCameraPosition(camera->GetPosition());
			mat->SetCameraExt(camera->GetComponent<CameraComponent>()->GetExtents());
			//	mat->SetTexture(sub.m_Material->GetColorTexture(), 0);
			mat->SetLight(light);
			for (int i = 0;i < 100;i++) {

				mat->SetBone( DiligentToGLM(bones[i]), i);

			}
			mat->Bind(add);

			mat->Render();


		}
		add = true;

	}

}