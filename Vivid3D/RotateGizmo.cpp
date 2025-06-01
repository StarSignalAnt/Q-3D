#include "RotateGizmo.h"
#include "Importer.h"

RotateGizmo::RotateGizmo() {

	auto import = new Importer;

	m_Node = Importer::ImportEntity("Edit/Gizmo/rotate1.fbx")->GetNodes()[0];

	FixNode();

	prot = m_Node->GetRotation();


	int b = 5;

}

void RotateGizmo::Select(int index) {

	SelectedID = index;

	//2 = Y
	//1 = X
	//0 = Z

	printf("Select:%d\n", index);

}

void RotateGizmo::Move(glm::vec2 delta) {

	if (m_Selected == nullptr) return;
	int a = 5;
	delta = delta * 0.08f;
	switch (m_Space) {
	case GizmoSpace::Local:

		if (SelectedID == 2) {
			m_Selected->Turn(glm::vec3(delta.x,0, 0),true);
		}
		if (SelectedID == 1)
		{
			m_Selected->Turn(glm::vec3(0, 0, delta.x),true);
		}
		if (SelectedID == 0)
		{
			m_Selected->Turn(glm::vec3(0, delta.x,0),true);
		}

		break;
	case GizmoSpace::World:
		if (SelectedID == 2) {
			m_Selected->Turn(glm::vec3(delta.x, 0, 0), false);
		}
		if (SelectedID == 1)
		{
			m_Selected->Turn(glm::vec3(0, 0, delta.x), false);
		}
		if (SelectedID == 0)
		{
			m_Selected->Turn(glm::vec3(0, delta.x, 0), false);
		}
		break;
	}
	AlignGizmo();
}
