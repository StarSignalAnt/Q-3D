#include "RotateGizmo.h"
#include "Importer.h"
#include "CameraComponent.h"
#include <glm/gtc/quaternion.hpp>
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
	
	delta.x = -delta.x;
	delta.y = -delta.y;
	
	if (m_Selected == nullptr) return;

	auto camNode = SceneGraph::m_Instance->GetCamera();
	// It's safer to assume the camera node might not have the component
	if (!camNode) return;
	auto cam = camNode->GetComponent<CameraComponent>();
	if (!cam) return;


	// 1. Define the base rotation axis from the selected gizmo part
	glm::vec3 base_axis;
	switch (SelectedID) {
	case 0: base_axis = glm::vec3(0, 0, 1); break; // Z-axis ring
	case 1: base_axis = glm::vec3(0, 1, 0); break; // X-axis ring
	case 2: base_axis = glm::vec3(1, 0, 0); break; // Y-axis ring
	default: return; // No valid ring selected
	}
	//if (SelectedID != 2) return;


	// 2. Determine the rotation axis in WORLD space
	glm::vec3 rotation_axis_world = base_axis;
	if (m_Space == GizmoSpace::Local) {
		// If in local space, transform the axis by the object's current world rotation
		// Note: Assumes a function GetWorldRotation() exists on your Node class
		rotation_axis_world = glm::quat_cast(m_Selected->GetWorldRotation()) * base_axis;

	}

	// 3. Map 2D mouse movement to a 3D rotation
	// This is the core of the dynamic behavior.
	glm::vec3 object_pos = m_Selected->GetPosition();
	glm::vec3 camera_pos = camNode->GetPosition();
	glm::vec3 view_dir = glm::normalize(object_pos - camera_pos);

	// Create a 3D tangent on the rotation plane that is perpendicular to the view direction.
	// This vector represents the "side-to-side" motion on the gizmo ring from the camera's perspective.
	glm::vec3 screen_tangent_3d = glm::normalize(glm::cross(rotation_axis_world, view_dir));

	// If the camera is looking perfectly along the rotation axis, the cross product is zero.
	// We use the camera's right vector as a fallback for a stable tangent.
	if (glm::length(screen_tangent_3d) < 0.1f) {
		screen_tangent_3d = camNode->GetRightVector(); // Assumes GetRightVector() exists
	}

	// Project the mouse delta onto the screen tangent to find rotation amount.
	// We do this by seeing how the camera's own right/up vectors align with the tangent.
	float x_contrib = delta.x * glm::dot(camNode->GetRightVector(), screen_tangent_3d);
	// Mouse-Y is typically inverted, so we use -GetUpVector()
	float y_contrib = delta.y * glm::dot(-camNode->GetUpVector(), screen_tangent_3d);

	// The total rotation is the sum, with a sensitivity factor for control.
	float sensitivity = 0.5f;
	float total_rotation_angle_deg = (x_contrib + y_contrib) * sensitivity;

	// 4. Apply the rotation using a quaternion for stability
	glm::quat rotation_delta = glm::angleAxis(glm::radians(total_rotation_angle_deg), rotation_axis_world);

	// Apply the delta to the object's current world rotation
	// Note: Assumes GetWorldRotation() and SetWorldRotation() exist
	m_Selected->SetWorldRotation(rotation_delta * glm::quat_cast(m_Selected->GetWorldRotation()));


	// Keep the gizmo aligned with the object
	AlignGizmo();
	
	
	
	
	
	
	
	
	return;
	/*
	int a = 5;
	auto cam = SceneGraph::m_Instance->GetCamera();
	auto cc = cam->GetComponent<CameraComponent>();
	delta = delta * 0.48f;
	switch (m_Space) {
	case GizmoSpace::Local:

		if (SelectedID == 2) {

			glm::vec3 s_w = m_Selected->GetPosition();
			glm::vec2 s_p = cc->WorldToScreen(s_w);

			glm::vec3 x_w = s_w + m_Selected->TransformVector(glm::vec3(0, 0, 5));
			glm::vec2 x_p = cc->WorldToScreen(x_w);

			int b = 5;

			float xd = x_p.x - s_p.x;
			float yd = x_p.y - s_p.y;

			if (abs(xd) > abs(yd)) {

				float s = 1.0f;

				if (xd < 0) {
					s = -1.0f;
				}
				//m_Selected->Move(glm::vec3(0, 0, delta.x * 0.5f * s));


				m_Selected->Turn(glm::vec3(-delta.x*s, 0, 0), true);

			}
			else {

				float s = 1.0f;
				if (yd < 0) {
					s = -1.0f;
				}

				//m_Selected->Move(glm::vec3(0, 0, delta.y * 0.5f * s));


				m_Selected->Turn(glm::vec3(delta.y*s, 0, 0), true);
			}

		}
		if (SelectedID == 1)
		{
		
			//	m_Selected->Turn(glm::vec3(0,delta.x , 0), true);
		
		m_Selected->Turn(glm::vec3(0, delta.x,0),true);
		}
		if (SelectedID == 0)
		{
			
			return;
			glm::vec3 s_w = m_Selected->GetPosition();
			glm::vec2 s_p = cc->WorldToScreen(s_w);

			glm::vec3 x_w = s_w + m_Selected->TransformVectorGlobal(glm::vec3(0, 0, 2));
			glm::vec2 x_p = cc->WorldToScreen(x_w);

			int b = 5;

			float xd = x_p.x - s_p.x;
			float yd = x_p.y - s_p.y;

			if (abs(xd) > abs(yd)) {

				float s = 1.0f;

				if (xd < 0) {
					s = -1.0f;
				}
				//m_Selected->Move(glm::vec3(0, 0, delta.x * 0.5f * s));


				m_Selected->Turn(glm::vec3(0,0,delta.x*s), true);

			}
			else {

				float s = 1.0f;
				if (yd < 0) {
					s = -1.0f;
				}

				//m_Selected->Move(glm::vec3(0, 0, delta.y * 0.5f * s));



				m_Selected->Turn(glm::vec3(0,0,delta.y*s), true);
			}
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
*/
	AlignGizmo();
}
