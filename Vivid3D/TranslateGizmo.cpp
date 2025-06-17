#include "TranslateGizmo.h"
#include "Importer.h"
#include "CameraComponent.h"
TranslateGizmo::TranslateGizmo() {

	auto import = new Importer;

	m_Node = Importer::ImportEntity("Edit/Gizmo/translate.gltf");
	m_Node->RemoveParent();
	FixNode();
	prot = m_Node->GetRotation();
	int b = 5;

}

void TranslateGizmo::Select(int index) {

	SelectedID = index;

	//2 = Y
	//1 = X
	//0 = Z

	//printf("Select:%d\n", index);

}

void TranslateGizmo::Move(glm::vec2 delta) {

	
	
	auto cam = SceneGraph::m_Instance->GetCamera();
	auto cc = cam->GetComponent<CameraComponent>();

	if (m_Selected == nullptr) return;
	int a = 5;
	delta = delta * 0.02f;
	switch (m_Space) {
	case GizmoSpace::Local:

		if (SelectedID == 2) {
			glm::vec3 s_w = m_Selected->GetPosition();
			glm::vec2 s_p = cc->WorldToScreen(s_w);

			glm::vec3 x_w = s_w + m_Selected->TransformVector(glm::vec3(0,0,2));
			glm::vec2 x_p = cc->WorldToScreen(x_w);

			int b = 5;

			float xd = x_p.x - s_p.x;
			float yd = x_p.y - s_p.y;

			if (abs(xd) > abs(yd)) {

				float s = 1.0f;

				if (xd < 0) {
					s = -1.0f;
				}
				m_Selected->Move(glm::vec3(0,0, delta.x * 0.5f * s));


			}
			else {

				float s = 1.0f;
				if (yd < 0) {
					s = -1.0f;
				}

				m_Selected->Move(glm::vec3(0,0, delta.y * 0.5f * s));

			}



			
		}
		if (SelectedID == 1)
		{

			glm::vec3 s_w = m_Selected->GetPosition();
			glm::vec2 s_p = cc->WorldToScreen(s_w);

			glm::vec3 x_w = s_w + m_Selected->TransformVector(glm::vec3(2, 0, 0));
			glm::vec2 x_p = cc->WorldToScreen(x_w);


			float xd = x_p.x - s_p.x;
			float yd = x_p.y - s_p.y;

			if (abs(xd) > abs(yd)) {

				float s = 1.0f;

				if (xd < 0) {
					s = -1.0f;
				}
				m_Selected->Move(glm::vec3(delta.x * 0.5f * s, 0, 0));

			}
			else {

				float s = 1.0f;
				if (yd < 0) {
					s = -1.0f;
				}

				m_Selected->Move(glm::vec3(delta.y * 0.5f * s, 0, 0));

			}

			//m_Selected->Move(glm::vec3(delta.x, 0, 0));

		}
		if (SelectedID == 0)
		{
			m_Selected->Move(glm::vec3(0, -delta.y, 0));
		}

		
			//m_Selected->Move(glm::vec3(0,0,delta.y));

		

		break;
	case GizmoSpace::World:
		if (SelectedID == 1)
		{
			glm::vec3 s_w = m_Selected->GetPosition();
			glm::vec2 s_p = cc->WorldToScreen(s_w);

			glm::vec3 x_w = s_w + glm::vec3(2, 0, 0); //m_Selected->TransformVector(glm::vec3(2, 0, 0));
			glm::vec2 x_p = cc->WorldToScreen(x_w);

			float xd = x_p.x - s_p.x;
			float yd = x_p.y - s_p.y;

			if (abs(xd) > abs(yd)) {

				float s = 1.0f;

				if (xd < 0) {
					s = -1.0f;
				}
				m_Selected->Translate(glm::vec3(delta.x * 0.5f * s, 0, 0));

			}
			else {

				float s = 1.0f;
				if (yd < 0) {
					s = -1.0f;
				}

				m_Selected->Translate(glm::vec3(delta.y * 0.5f * s, 0, 0));

			}
		}

		/*
		auto cen = m_Selected->GetPosition();
		auto right = m_Selected->GetPosition()+m_Selected->TransformVector(glm::vec3(0.1, 0, 0));
		auto down = m_Selected->GetPosition()+m_Selected->TransformVector(glm::vec3(0, 0, -0.1));

		auto cpos = cc->WorldToScreen(cen);
		auto rpos = cc->WorldToScreen(right);
		auto dpos = cc->WorldToScreen(down);

		float xd = glm::distance(rpos, cpos);
		float yd = glm::distance(dpos, cpos);

		bool flip = false;

		if (xd > yd) {

			if (rpos.x < cpos.x) {
				flip = true;
			}

		}
		else {

			if (dpos.y > cpos.y) {
				flip = true;
			}

		}

		flip = false;

		int b = 5;

		*/
		
		if (SelectedID == 0) {
			m_Selected->Translate(glm::vec3(0, -delta.y, 0));
		}
		if (SelectedID == 1)
		{
			/*
			if (xd > yd) {
				if (flip) {
					m_Selected->Translate(glm::vec3(-delta.x, 0, 0));
				}
				else {
					m_Selected->Translate(glm::vec3(delta.x, 0, 0));
				}
			}
			else {
				if (flip) {
					m_Selected->Translate(glm::vec3(-delta.y, 0, 0));
				}
				else {
					m_Selected->Translate(glm::vec3(delta.y, 0, 0));
				}
			}
			*/
		}
		if (SelectedID == 2)
		{
			glm::vec3 s_w = m_Selected->GetPosition();
			glm::vec2 s_p = cc->WorldToScreen(s_w);

			glm::vec3 x_w = s_w + glm::vec3(0, 0, 2); //m_Selected->TransformVector(glm::vec3(2, 0, 0));
			glm::vec2 x_p = cc->WorldToScreen(x_w);

			float xd = x_p.x - s_p.x;
			float yd = x_p.y - s_p.y;

			if (abs(xd) > abs(yd)) {

				float s = 1.0f;

				if (xd < 0) {
					s = -1.0f;
				}
				m_Selected->Translate(glm::vec3(0, 0, delta.x * 0.5f * s));

			}
			else {

				float s = 1.0f;
				if (yd < 0) {
					s = -1.0f;
				}

				m_Selected->Translate(glm::vec3(0, 0, delta.y * 0.5f * s));

			}

		}

		break;
	}
	AlignGizmo();
}
