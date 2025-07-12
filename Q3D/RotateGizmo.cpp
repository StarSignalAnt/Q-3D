#include "RotateGizmo.h"
#include "Importer.h"
#include "CameraComponent.h"
#include <glm/gtc/quaternion.hpp>
#include "Importer.h"
#include "CameraComponent.h"
#include "GraphNode.h"
#include "MaterialBasic3D.h"
#include "StaticMeshComponent.h"
#include "StaticRendererComponent.h"
#include "MaterialProducer.h" // For getting a default material
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
void CreateTorusGeometry(std::vector<Vertex3>& outVertices, std::vector<Tri3>& outTriangles,
	float mainRadius, float tubeRadius, int mainSegments, int tubeSegments)
{
	outVertices.clear();
	outTriangles.clear();

	// Generate vertices
	for (int i = 0; i <= mainSegments; ++i) {
		float mainAngle = 2.0f * glm::pi<float>() * i / mainSegments;
		glm::vec3 mainCirclePos(cos(mainAngle) * mainRadius, sin(mainAngle) * mainRadius, 0);

		for (int j = 0; j <= tubeSegments; ++j) {
			float tubeAngle = 2.0f * glm::pi<float>() * j / tubeSegments;

			// Calculate the position of the vertex on the tube's cross-section
			glm::vec3 tubeOffset(cos(tubeAngle), sin(tubeAngle), 0);
			tubeOffset = glm::angleAxis(mainAngle, glm::vec3(0, 0, 1)) * tubeOffset;
			tubeOffset.z = sin(tubeAngle) * tubeRadius;

			// Adjust the offset for the main circle radius
			glm::vec3 vertexPos = mainCirclePos + glm::vec3(cos(mainAngle), sin(mainAngle), 0) * cos(tubeAngle) * tubeRadius;
			vertexPos.z = sin(tubeAngle) * tubeRadius;

			Vertex3 vert;
			vert.position = vertexPos;
			// Normals could be calculated here if needed for lighting
			outVertices.push_back(vert);
		}
	}

	// Generate triangles (indices)
	for (int i = 0; i < mainSegments; ++i) {
		for (int j = 0; j < tubeSegments; ++j) {
			int p1 = i * (tubeSegments + 1) + j;
			int p2 = p1 + 1;
			int p3 = (i + 1) * (tubeSegments + 1) + j;
			int p4 = p3 + 1;

			outTriangles.push_back({ (unsigned int)p1, (unsigned int)p3, (unsigned int)p2 });
			outTriangles.push_back({ (unsigned int)p2, (unsigned int)p3, (unsigned int)p4 });
		}
	}
}


/// @brief Generates a complete rotate gizmo model with three colored rings as separate submeshes.
/// @param scale The overall radius of the rings.
/// @return A new GraphNode containing the gizmo's mesh and renderer.
GraphNode* MakeRotateGizmo(float scale)
{
	// 1. Create temporary geometry for a single torus ring
	std::vector<Vertex3> torusVertices;
	std::vector<Tri3> torusTriangles;
	// Parameters: main radius, tube radius, main segments, tube segments
	CreateTorusGeometry(torusVertices, torusTriangles, scale, scale * 0.05f, 48, 12);

	// 2. Create the GraphNode and its components
	GraphNode* gizmoNode = new GraphNode();
	gizmoNode->SetName("RotateGizmo");

	StaticMeshComponent* meshComp = new StaticMeshComponent();
	StaticRendererComponent* rendererComp = new StaticRendererComponent();

	// --- Define transforms and colors for each axis ---
	// Torus is created in XY plane, so we rotate it to align with other planes
	const glm::mat4 x_transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 y_transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 z_transform = glm::mat4(1.0f); // Already in the correct XY plane for Z-axis rotation

	const glm::vec4 red = { 1.0f, 0.0f, 0.0f, 1.0f };
	const glm::vec4 green = { 0.0f, 1.0f, 0.0f, 1.0f };
	const glm::vec4 blue = { 0.0f, 0.0f, 1.0f, 1.0f };

	// 3. Create a separate SubMesh for each ring
	auto createSubMeshForAxis = [&](const glm::mat4& transform, const glm::vec4& color) {
		SubMesh* subMesh = new SubMesh();
		LODLevel* lod = new LODLevel();

		// Transform vertices, set color, and add to this SubMesh's LOD
		for (const auto& v : torusVertices) {
			Vertex3 newVert = v;
			newVert.position = glm::vec3(transform * glm::vec4(v.position, 1.0f));
			newVert.color = color;
			lod->m_Vertices.push_back(newVert);
		}
		lod->m_Triangles = torusTriangles;

		subMesh->m_LODs.push_back(lod);

		// Assign a material that supports vertex colors
		subMesh->m_Material = new MaterialBasic3D;
		subMesh->m_DepthMaterial = (RenderMaterial*)MaterialProducer::m_Instance->GetDepth();

		meshComp->AddSubMesh(subMesh);
		};

	// The order here is critical for SelectedID to work correctly
	createSubMeshForAxis(x_transform, red);   // SubMesh 0 (X-axis)
	createSubMeshForAxis(y_transform, green); // SubMesh 1 (Y-axis)
	createSubMeshForAxis(z_transform, blue);  // SubMesh 2 (Z-axis)

	// 4. Finalize the mesh to create GPU buffers
	meshComp->Finalize();

	// 5. Attach components to the node
	gizmoNode->AddComponent(meshComp);
	gizmoNode->AddComponent(rendererComp);

	return gizmoNode;
}
// Helper function to calculate the intersection point of a ray and a plane.
bool RayPlaneIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& planePoint, const glm::vec3& planeNormal, glm::vec3& outIntersection)
{
	float denom = glm::dot(planeNormal, rayDir);
	if (abs(denom) > 1e-6) { // Ensure the ray is not parallel to the plane
		float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
		if (t >= 0) { // Intersection must be in front of the ray
			outIntersection = rayOrigin + t * rayDir;
			return true;
		}
	}
	return false;
}
RotateGizmo::RotateGizmo() {

	auto import = new Importer;

	m_Node = MakeRotateGizmo(1.0f);// Importer::ImportEntity("Edit/Gizmo/rotate1.fbx")->GetNodes()[0];

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


	auto camNode = SceneGraph::m_Instance->GetCamera();
	if (!camNode) return;
	auto cam = camNode->GetComponent<CameraComponent>();
	if (!cam) return;

	// 1. Define the base rotation axis from the selected submesh ID
	glm::vec3 base_axis;
	switch (SelectedID) {
	case 0: base_axis = glm::vec3(1.0f, 0.0f, 0.0f); break; // X-axis (Red)
	case 1: base_axis = glm::vec3(0.0f, 1.0f, 0.0f); break; // Y-axis (Green)
	case 2: base_axis = glm::vec3(0.0f, 0.0f, 1.0f); break; // Z-axis (Blue)
	default: return;
	}

	// 2. Calculate the amount of rotation from mouse input
	glm::vec3 projection_axis = (m_Space == GizmoSpace::Local) ? (glm::quat_cast(m_Selected->GetWorldRotation()) * base_axis) : base_axis;
	glm::vec3 view_dir = glm::normalize(m_Selected->GetPosition() - camNode->GetPosition());
	float alignment = abs(glm::dot(view_dir, projection_axis));
	float rotation_angle_deg = 0.0f;

	if (alignment > 0.95f) {
		float direction = glm::sign(glm::dot(view_dir, projection_axis));
		rotation_angle_deg = delta.x * direction;
	}
	else {
		glm::vec3 tangent = glm::normalize(glm::cross(projection_axis, view_dir));
		float x_contrib = glm::dot(camNode->GetRightVector(), tangent);
		float y_contrib = glm::dot(camNode->GetUpVector(), tangent);
		rotation_angle_deg = (delta.x * x_contrib) + (delta.y * y_contrib);
	}

	float sensitivity = 0.5f;
	rotation_angle_deg *= sensitivity;

	// 3. Apply the rotation based on the selected coordinate space
	if (m_Space == GizmoSpace::Local)
	{
		// --- LOCAL SPACE ROTATION ---
		glm::quat local_delta = glm::angleAxis(glm::radians(rotation_angle_deg), base_axis);
		glm::quat current_local_rot = glm::quat_cast(m_Selected->GetRotation());
		glm::quat new_local_rot = current_local_rot * local_delta;

		// --- FIX ---
		// Normalize the final quaternion to remove any accumulated scaling.
		new_local_rot = glm::normalize(new_local_rot);

		m_Selected->SetRotation(new_local_rot);
	}
	else // WORLD SPACE ROTATION
	{
		// --- WORLD SPACE ROTATION ---
		glm::quat world_delta = glm::angleAxis(glm::radians(rotation_angle_deg), base_axis);
		glm::quat current_world_rot = glm::quat_cast(m_Selected->GetWorldRotation());
		glm::quat new_world_rot = world_delta * current_world_rot;

		// It's good practice to normalize here as well for safety.
		new_world_rot = glm::normalize(new_world_rot);

		m_Selected->SetWorldRotation(new_world_rot);
	}

	AlignGizmo();
}
