#include "TranslateGizmo.h"
#include "Importer.h"

TranslateGizmo::TranslateGizmo() {

	auto import = new Importer;

	m_Node = Importer::ImportEntity("Edit/Gizmo/translate1.fbx")->GetNodes()[0];

	FixNode();

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

	if (m_Selected == nullptr) return;
	int a = 5;
	delta = delta * 0.02f;
	switch (m_Space) {
	case GizmoSpace::Local:

		if (SelectedID == 2) {
			m_Selected->Move(glm::vec3(0, -delta.y,0));
		}
		if (SelectedID == 1)
		{
			m_Selected->Move(glm::vec3(delta.x, 0, 0));
		}
		if (SelectedID == 0)
		{
			m_Selected->Move(glm::vec3(0,0,delta.y));
		}

		break;
	}
	AlignGizmo();
}
