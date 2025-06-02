#include "SceneGizmo.h"
#include "StaticRendererComponent.h"
#include "StaticMeshComponent.h"
#include "MaterialBasic3D.h"
#include "MaterialPBR.h"

void SceneGizmo::Render(GraphNode* camera) {

	if (m_Selected == nullptr) return;
	auto renderer = m_Node->GetComponent<StaticRendererComponent>();

	renderer->OnRender(camera);

}

void SceneGizmo::FixNode() {

	auto mesh = m_Node->GetComponent<StaticMeshComponent>();
	for (auto sm : mesh->GetSubMeshes()) {

		auto tex = ((MaterialPBR*)sm->m_Material)->GetColorTexture();
		sm->m_Material = new MaterialBasic3D;
		auto b3 = (MaterialBasic3D*)sm->m_Material;
		b3->SetColorTexture(tex);


	}

	//m_Node->SetRotation(glm::vec3(180, 180, 0));

}

bool SceneGizmo::Click(int x, int y) {

	auto res = m_Graph->MousePickSelect(x, y, m_Node->GetComponent<StaticMeshComponent>());
	
	if (res.m_Hit) {

		Select(res.SubMeshIndex);

		return true;
		//	printf("Gizmo selected.\n");
	}
	else {
		//printf("Gizmo no hit.\n");
	}

}

void SceneGizmo::AlignGizmo() {

	if (m_Selected == nullptr) return;
	m_Node->SetPosition(m_Selected->GetPosition());
	if (m_Space == GizmoSpace::Local) {

		m_Node->SetRotation(m_Selected->GetRotation()*prot);
	}
	else {
		m_Node->SetRotation(prot);
	}

}

void SceneGizmo::SetSpace(GizmoSpace space) {

	m_Space = space;

}