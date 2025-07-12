#include "ScaleGizmo.h"
#include "Importer.h"
#include "StaticMeshComponent.h"
#include "StaticRendererComponent.h"
#include "MaterialProducer.h"
#include "MaterialBasic3D.h"
#include <glm/gtc/matrix_transform.hpp>
#include "CameraComponent.h"

// Helper function to generate geometry for a single scale handle (shaft with a cube at the end).
// The geometry is created pointing along the positive Z-axis.
void CreateScaleArrowGeometry(std::vector<Vertex3>&outVertices, std::vector<Tri3>&outTriangles, float scale)
{
    outVertices.clear();
    outTriangles.clear();

    // --- Define Dimensions ---
    float shaftLength = scale * 0.85f;
    float shaftWidth = scale * 0.02f; // Half-width of the shaft
    float cubeWidth = scale * 0.1f;  // Half-width of the cube end

    // --- Shaft Vertices (a long box) ---
    outVertices.push_back({ {-shaftWidth, -shaftWidth, 0.0f} });       // 0
    outVertices.push_back({ { shaftWidth, -shaftWidth, 0.0f} });       // 1
    outVertices.push_back({ { shaftWidth,  shaftWidth, 0.0f} });       // 2
    outVertices.push_back({ {-shaftWidth,  shaftWidth, 0.0f} });       // 3
    outVertices.push_back({ {-shaftWidth, -shaftWidth, shaftLength} }); // 4
    outVertices.push_back({ { shaftWidth, -shaftWidth, shaftLength} }); // 5
    outVertices.push_back({ { shaftWidth,  shaftWidth, shaftLength} }); // 6
    outVertices.push_back({ {-shaftWidth,  shaftWidth, shaftLength} }); // 7

    // --- Shaft Triangles ---
    outTriangles.insert(outTriangles.end(), {
        {0, 1, 2}, {0, 2, 3}, {4, 6, 5}, {4, 7, 6}, {0, 4, 5}, {0, 5, 1},
        {1, 5, 6}, {1, 6, 2}, {2, 6, 7}, {2, 7, 3}, {3, 7, 4}, {3, 4, 0}
        });

    unsigned int cubeBaseIndex = outVertices.size();

    // --- Cube End Vertices ---
    outVertices.push_back({ {-cubeWidth, -cubeWidth, shaftLength} });             // 8
    outVertices.push_back({ { cubeWidth, -cubeWidth, shaftLength} });             // 9
    outVertices.push_back({ { cubeWidth,  cubeWidth, shaftLength} });             // 10
    outVertices.push_back({ {-cubeWidth,  cubeWidth, shaftLength} });             // 11
    outVertices.push_back({ {-cubeWidth, -cubeWidth, shaftLength + cubeWidth * 2} }); // 12
    outVertices.push_back({ { cubeWidth, -cubeWidth, shaftLength + cubeWidth * 2} }); // 13
    outVertices.push_back({ { cubeWidth,  cubeWidth, shaftLength + cubeWidth * 2} }); // 14
    outVertices.push_back({ {-cubeWidth,  cubeWidth, shaftLength + cubeWidth * 2} }); // 15

    // --- Cube End Triangles ---
    outTriangles.insert(outTriangles.end(), {
        {cubeBaseIndex + 0, cubeBaseIndex + 1, cubeBaseIndex + 2}, {cubeBaseIndex + 0, cubeBaseIndex + 2, cubeBaseIndex + 3},
        {cubeBaseIndex + 4, cubeBaseIndex + 5, cubeBaseIndex + 6}, {cubeBaseIndex + 4, cubeBaseIndex + 6, cubeBaseIndex + 7},
        {cubeBaseIndex + 0, cubeBaseIndex + 4, cubeBaseIndex + 7}, {cubeBaseIndex + 0, cubeBaseIndex + 7, cubeBaseIndex + 3},
        {cubeBaseIndex + 1, cubeBaseIndex + 5, cubeBaseIndex + 4}, {cubeBaseIndex + 1, cubeBaseIndex + 4, cubeBaseIndex + 0},
        {cubeBaseIndex + 2, cubeBaseIndex + 6, cubeBaseIndex + 5}, {cubeBaseIndex + 2, cubeBaseIndex + 5, cubeBaseIndex + 1},
        {cubeBaseIndex + 3, cubeBaseIndex + 7, cubeBaseIndex + 6}, {cubeBaseIndex + 3, cubeBaseIndex + 6, cubeBaseIndex + 2}
        });
}


/// @brief Generates a complete scale gizmo model with three colored handles as separate submeshes.
/// @param scale The overall size of the gizmo.
/// @return A new GraphNode containing the gizmo's mesh and renderer.
GraphNode* MakeScaleGizmo(float scale)
{
    // 1. Create temporary geometry for a single Z-up handle
    std::vector<Vertex3> arrowVertices;
    std::vector<Tri3> arrowTriangles;
    CreateScaleArrowGeometry(arrowVertices, arrowTriangles, scale);

    // 2. Create the GraphNode and its components
    GraphNode* gizmoNode = new GraphNode();
    gizmoNode->SetName("ScaleGizmo");

    StaticMeshComponent* meshComp = new StaticMeshComponent();
    StaticRendererComponent* rendererComp = new StaticRendererComponent();

    // --- Define transforms and colors for each axis ---
    const glm::mat4 x_transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 y_transform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 z_transform = glm::mat4(1.0f);

    const glm::vec4 red = { 1.0f, 0.0f, 0.0f, 1.0f };
    const glm::vec4 green = { 0.0f, 1.0f, 0.0f, 1.0f };
    const glm::vec4 blue = { 0.0f, 0.0f, 1.0f, 1.0f };

    // 3. Create a separate SubMesh for each handle
    auto createSubMeshForAxis = [&](const glm::mat4& transform, const glm::vec4& color) {
        SubMesh* subMesh = new SubMesh();
        LODLevel* lod = new LODLevel();

        for (const auto& v : arrowVertices) {
            Vertex3 newVert = v;
            newVert.position = glm::vec3(transform * glm::vec4(v.position, 1.0f));
            newVert.color = color;
            lod->m_Vertices.push_back(newVert);
        }
        lod->m_Triangles = arrowTriangles;

        subMesh->m_LODs.push_back(lod);
        subMesh->m_Material = new MaterialBasic3D();
        subMesh->m_DepthMaterial = (RenderMaterial*)MaterialProducer::m_Instance->GetDepth();

        meshComp->AddSubMesh(subMesh);
        };

    // The creation order here maps directly to SelectedID
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
ScaleGizmo::ScaleGizmo() {

	auto import = new Importer;

    m_Node = MakeScaleGizmo(1.0f); //Importer::ImportEntity("Edit/Gizmo/scale1.fbx")->GetNodes()[0];

	FixNode();

	prot = m_Node->GetRotation();


	int b = 5;

}

void ScaleGizmo::Select(int index) {

	SelectedID = index;

	//2 = Y
	//1 = X
	//0 = Z

	printf("Select:%d\n", index);

}
glm::vec3 ClosestPointOnLineToRay3(
    glm::vec3 line_point, glm::vec3 line_dir,
    glm::vec3 ray_origin, glm::vec3 ray_dir)
{
    line_dir = glm::normalize(line_dir);
    ray_dir = glm::normalize(ray_dir);

    glm::vec3 w = line_point - ray_origin;
    float a = glm::dot(line_dir, line_dir);
    float b = glm::dot(line_dir, ray_dir);
    float c = glm::dot(ray_dir, ray_dir);
    float d = glm::dot(line_dir, w);
    float e = glm::dot(ray_dir, w);

    float denominator = a * c - b * b;
    if (glm::abs(denominator) < 0.0001f) {
        return line_point + line_dir * d;
    }
    float t = (b * e - c * d) / denominator;
    return line_point + line_dir * t;
}

void ScaleGizmo::Move(glm::vec2 delta) {

    if (m_Selected == nullptr) return;

    auto camNode = SceneGraph::m_Instance->GetCamera();
    if (!camNode) return;
    auto cc = camNode->GetComponent<CameraComponent>();
    if (!cc) return;

    // 1. Define the base axis from the selected submesh ID
    // This order must match the creation order in MakeScaleGizmo
    glm::vec3 base_axis;
    switch (SelectedID) {
    case 0: base_axis = glm::vec3(1.0f, 0.0f, 0.0f); break; // X-axis (Red)
    case 1: base_axis = glm::vec3(0.0f, 1.0f, 0.0f); break; // Y-axis (Green)
    case 2: base_axis = glm::vec3(0.0f, 0.0f, 1.0f); break; // Z-axis (Blue)
    default: return; // No valid handle selected
    }

    // 2. Determine the scaling axis in WORLD space for projection
    // Scaling is always applied locally, but we need the world-space axis for the projection math.
    glm::vec3 scale_axis_world = glm::quat_cast(m_Selected->GetWorldRotation()) * base_axis;

    // 3. Project mouse movement onto the 3D axis
    glm::vec3 ray_origin;
    glm::vec3 current_ray_dir;
    glm::vec3 prev_ray_dir;

    cc->ScreenToWorldRay(cc->GetMouseCoords(), ray_origin, current_ray_dir);
    cc->ScreenToWorldRay(cc->GetMouseCoords() - delta, ray_origin, prev_ray_dir);

    glm::vec3 line_point = m_Selected->GetPosition();
    glm::vec3 line_dir = scale_axis_world;

    glm::vec3 current_closest_point = ClosestPointOnLineToRay3(line_point, line_dir, ray_origin, current_ray_dir);
    glm::vec3 prev_closest_point = ClosestPointOnLineToRay3(line_point, line_dir, ray_origin, prev_ray_dir);

    // 4. Calculate the scalar change from the projected movement
    glm::vec3 world_space_delta = current_closest_point - prev_closest_point;

    // The magnitude of the movement is the amount we want to scale by
    float scale_change = glm::length(world_space_delta);

    // Check if we moved along or against the axis to determine if we should scale up or down
    if (glm::dot(world_space_delta, scale_axis_world) < 0.0f) {
        scale_change = -scale_change;
    }

    // Apply a sensitivity factor to get the right "feel" for the interaction
    float sensitivity = 1.5f; // You can tune this value
    scale_change *= sensitivity;

    // 5. Apply the calculated change to the correct LOCAL scale axis
    glm::vec3 scale_delta_vector(0.0f);
    switch (SelectedID) {
    case 0: scale_delta_vector.x = scale_change; break;
    case 1: scale_delta_vector.y = scale_change; break;
    case 2: scale_delta_vector.z = scale_change; break;
    }

    m_Selected->SetScale(m_Selected->GetScale() + scale_delta_vector);

    // 6. Keep the gizmo visuals aligned with the object
    AlignGizmo();
}
