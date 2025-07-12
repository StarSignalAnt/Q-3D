#include "TranslateGizmo.h"
#include "Importer.h"
#include "CameraComponent.h"
#include "GraphNode.h"
#include "MaterialBasic3D.h"
#include "StaticMeshComponent.h"
#include "StaticRendererComponent.h"
#include "MaterialProducer.h" // For getting a default material
#include <glm/gtc/matrix_transform.hpp>
void CreateArrowGeometry(std::vector<Vertex3>& outVertices, std::vector<Tri3>& outTriangles, float scale) {
    outVertices.clear();
    outTriangles.clear();

    // --- Define Arrow Dimensions ---
    float shaftLength = scale * 0.8f;
    float shaftWidth = scale * 0.05f; // This is the half-width
    float headLength = scale * 0.2f;
    float headWidth = scale * 0.1f;  // This is the half-width

    // --- Shaft Vertices (a simple box) ---
    // Base
    outVertices.push_back({ {-shaftWidth, -shaftWidth, 0.0f} });         // 0
    outVertices.push_back({ { shaftWidth, -shaftWidth, 0.0f} });         // 1
    outVertices.push_back({ { shaftWidth,  shaftWidth, 0.0f} });         // 2
    outVertices.push_back({ {-shaftWidth,  shaftWidth, 0.0f} });         // 3
    // Top
    outVertices.push_back({ {-shaftWidth, -shaftWidth, shaftLength} }); // 4
    outVertices.push_back({ { shaftWidth, -shaftWidth, shaftLength} }); // 5
    outVertices.push_back({ { shaftWidth,  shaftWidth, shaftLength} }); // 6
    outVertices.push_back({ {-shaftWidth,  shaftWidth, shaftLength} }); // 7

    // --- Shaft Triangles (12 triangles for 6 faces) ---
    outTriangles.insert(outTriangles.end(), {
        {0, 1, 2}, {0, 2, 3}, // Bottom face
        {4, 6, 5}, {4, 7, 6}, // Top face
        {0, 4, 5}, {0, 5, 1}, // Side face
        {1, 5, 6}, {1, 6, 2}, // Side face
        {2, 6, 7}, {2, 7, 3}, // Side face
        {3, 7, 4}, {3, 4, 0}  // Side face
        });

    unsigned int headBaseIndex = outVertices.size();

    // --- Head Vertices (a pyramid) ---
    outVertices.push_back({ {-headWidth, -headWidth, shaftLength} }); // 8
    outVertices.push_back({ { headWidth, -headWidth, shaftLength} }); // 9
    outVertices.push_back({ { headWidth,  headWidth, shaftLength} }); // 10
    outVertices.push_back({ {-headWidth,  headWidth, shaftLength} }); // 11
    outVertices.push_back({ {0.0f, 0.0f, shaftLength + headLength} });   // 12 (Apex)

    // --- Head Triangles (4 side faces, 2 base faces) ---
    outTriangles.insert(outTriangles.end(), {
        {headBaseIndex + 0, headBaseIndex + 1, headBaseIndex + 2}, {headBaseIndex + 0, headBaseIndex + 2, headBaseIndex + 3}, // Base
        {headBaseIndex + 0, headBaseIndex + 4, headBaseIndex + 1}, // Side
        {headBaseIndex + 1, headBaseIndex + 4, headBaseIndex + 2}, // Side
        {headBaseIndex + 2, headBaseIndex + 4, headBaseIndex + 3}, // Side
        {headBaseIndex + 3, headBaseIndex + 4, headBaseIndex + 0}  // Side
        });
}

GraphNode* MakeTranslateGizmo(float scale)
{
    // 1. Create temporary geometry for a single Z-up arrow
    std::vector<Vertex3> arrowVertices;
    std::vector<Tri3> arrowTriangles;
    CreateArrowGeometry(arrowVertices, arrowTriangles, scale);

    // 2. Create the GraphNode and its components
    GraphNode* gizmoNode = new GraphNode();
    gizmoNode->SetName("TranslateGizmo");

    StaticMeshComponent* meshComp = new StaticMeshComponent();
    StaticRendererComponent* rendererComp = new StaticRendererComponent();

    // --- Define transforms and colors for each axis ---
    const glm::mat4 x_transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 y_transform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 z_transform = glm::mat4(1.0f);

    const glm::vec4 red = { 1.0f, 0.0f, 0.0f, 1.0f };
    const glm::vec4 green = { 0.0f, 1.0f, 0.0f, 1.0f };
    const glm::vec4 blue = { 0.0f, 0.0f, 1.0f, 1.0f };

    // 3. Create a separate SubMesh for each arrow
    auto createSubMeshForAxis = [&](const glm::mat4& transform, const glm::vec4& color) {
        SubMesh* subMesh = new SubMesh();
        LODLevel* lod = new LODLevel();

        // Transform vertices, set color, and add to this SubMesh's LOD
        for (const auto& v : arrowVertices) {
            Vertex3 newVert = v;
            newVert.position = glm::vec3(transform * glm::vec4(v.position, 1.0f));
            newVert.color = color;
            lod->m_Vertices.push_back(newVert);
        }
        lod->m_Triangles = arrowTriangles; // Indices are local to this vertex list, so no offset needed

        subMesh->m_LODs.push_back(lod);

        // Assign a material to the submesh
        subMesh->m_Material = new MaterialBasic3D;
        subMesh->m_DepthMaterial = (RenderMaterial*)MaterialProducer::m_Instance->GetDepth();

        // Add the completed submesh to the main mesh component
        meshComp->AddSubMesh(subMesh);
        };

    // The order here is critical and must match the switch statement in Move()
    createSubMeshForAxis(x_transform, red);   // This will be SubMesh 0
    createSubMeshForAxis(y_transform, green); // This will be SubMesh 1
    createSubMeshForAxis(z_transform, blue);  // This will be SubMesh 2

    // 4. Finalize the mesh to create GPU buffers for all submeshes
    meshComp->Finalize();

    // 5. Attach components to the node
    gizmoNode->AddComponent(meshComp);
    gizmoNode->AddComponent(rendererComp);

    return gizmoNode;
}

TranslateGizmo::TranslateGizmo() {

	auto import = new Importer;

    m_Node = MakeTranslateGizmo(1.0f);//  Importer::ImportEntity("Edit/Gizmo/translate.gltf");
	m_Node->RemoveParent();

	//FixNode();
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
	case 0: base_axis = glm::vec3(1, 0, 0); break; // Z-axis handle
	case 1: base_axis = glm::vec3(0,1, 0); break; // X-axis handle
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
