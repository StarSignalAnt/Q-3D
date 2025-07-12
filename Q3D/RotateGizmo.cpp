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
	case 0: base_axis = glm::vec3(1, 0, 0); break; // Z-axis ring
	case 1: base_axis = glm::vec3(0, 1, 0); break; // X-axis ring
	case 2: base_axis = glm::vec3(0, 0,1); break; // Y-axis ring
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
