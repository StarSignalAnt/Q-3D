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


glm::vec3 ClosestPointOnLineToRay(
	glm::vec3 line_point, glm::vec3 line_dir,
	glm::vec3 ray_origin, glm::vec3 ray_dir)
{
	// Ensure directions are unit vectors for stable calculations
	line_dir = glm::normalize(line_dir);
	ray_dir = glm::normalize(ray_dir);

	glm::vec3 w = line_point - ray_origin;
	float a = glm::dot(line_dir, line_dir); // Should be 1.0
	float b = glm::dot(line_dir, ray_dir);
	float c = glm::dot(ray_dir, ray_dir); // Should be 1.0
	float d = glm::dot(line_dir, w);
	float e = glm::dot(ray_dir, w);

	float denominator = a * c - b * b;

	// If the lines are parallel, the denominator will be close to zero.
	// In this case, we can use a simpler projection.
	if (glm::abs(denominator) < 0.0001f) {
		return line_point + line_dir * d;
	}

	// Solve for t, the parameter along the line to the closest point
	float t = (b * e - c * d) / denominator;

	// Return the closest point on the line
	return line_point + line_dir * t;
}

void TranslateGizmo::Move(glm::vec2 delta) {

	if (m_Selected == nullptr) return;

	auto camNode = SceneGraph::m_Instance->GetCamera();
	if (!camNode) return;
	auto cc = camNode->GetComponent<CameraComponent>();
	if (!cc) return;

	// 1. Define the base translation axis from the selected gizmo handle
	glm::vec3 base_axis;
	switch (SelectedID) {
	case 0: base_axis = glm::vec3(0, 1, 0); break; // Z-axis handle
	case 1: base_axis = glm::vec3(1, 0, 0); break; // X-axis handle
	case 2: base_axis = glm::vec3(0, 0, 1); break; // Y-axis handle
	default: return; // No valid handle selected
	}

	// 2. Determine the translation axis in WORLD space
	glm::vec3 translation_axis_world = base_axis;
	if (m_Space == GizmoSpace::Local) {
		// If in local space, rotate the axis by the object's world rotation
		translation_axis_world = glm::quat_cast(m_Selected->GetWorldRotation()) * base_axis;
	}

	// 3. Project mouse movement onto the 3D axis
	// We need to find where the mouse rays (current and previous) intersect the 3D line of the axis.

	// Get the 3D rays from the camera for the current and previous mouse positions
	// Note: Assumes a function ScreenToWorldRay() exists on your CameraComponent
	glm::vec3 ray_origin;
	glm::vec3 current_ray_dir;
	glm::vec3 prev_ray_dir;

	cc->ScreenToWorldRay(cc->GetMouseCoords(), ray_origin, current_ray_dir);
	cc->ScreenToWorldRay(cc->GetMouseCoords() - delta, ray_origin, prev_ray_dir);

	// The translation axis is a 3D line defined by the object's position and the axis direction
	glm::vec3 line_point = m_Selected->GetPosition();
	glm::vec3 line_dir = translation_axis_world;

	// Find the closest points on the axis line to each of the mouse rays
	// Note: Assumes a helper function ClosestPointOnLineToRay() exists
	glm::vec3 current_closest_point = ClosestPointOnLineToRay(line_point, line_dir, ray_origin, current_ray_dir);
	glm::vec3 prev_closest_point = ClosestPointOnLineToRay(line_point, line_dir, ray_origin, prev_ray_dir);

	// 4. Calculate the delta and apply it
	glm::vec3 world_space_delta = current_closest_point - prev_closest_point;

	// The Translate function should apply movement in world space
	m_Selected->Translate(world_space_delta);

	// Keep the gizmo aligned with the object
	AlignGizmo();
	

	return;

}
