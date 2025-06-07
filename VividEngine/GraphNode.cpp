
#include "GraphNode.h"
#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "BasicMath.hpp"
#include <glm/glm.hpp>
#include "StaticMeshComponent.h"
#include "Physics.h"
#include "Vivid.h"


using namespace Diligent;
physx::PxQuat glmMat4ToPxQuat(const glm::mat4& rotationMatrix)
{
	glm::quat q = glm::quat_cast(rotationMatrix);  // Convert mat4 to glm::quat
	return physx::PxQuat(q.x, q.y, q.z, q.w);      // Convert to PxQuat
}
GraphNode::GraphNode() {

	Reset();
	m_Components = std::vector<Component*>();
	m_Name = "Node";

}

void GraphNode::Reset() {
	
	m_Position = glm::vec3(0, 0, 0);
	m_Scale = glm::vec3(1, 1, 1);
	m_Rotation = glm::mat4(1.0f);


}

void GraphNode::AddNode(GraphNode* node) {
	if (node == nullptr) return;
	m_Nodes.push_back(node);
	node->SetRootNode(this);
}


void GraphNode::AddComponent(Component* component) {
	component->OnAttach(this);
	m_Components.push_back(component);
}

void GraphNode::Render(GraphNode* camera) {

	for(auto & component : m_Components) {
		component->OnRender(camera);
	}

	for (auto sub : m_Nodes) {
		sub->Render(camera);
	}

}

void GraphNode::RenderDepth(GraphNode* camera) {

	for (auto& component : m_Components) {
		component->OnRenderDepth(camera);
	}

	for (auto sub : m_Nodes) {
		sub->RenderDepth(camera);
	}

}

glm::mat4 GraphNode::GetWorldMatrix(){

	glm::mat4 root = glm::mat4(1.0f);

	if (m_RootNode != nullptr) {
		root = m_RootNode->GetWorldMatrix();
	}

	glm::mat4 transform = glm::mat4(1.0f);
	auto pos = glm::translate(transform, m_Position);
	auto rot  = m_Rotation;
	auto scale  = glm::scale(transform, m_Scale);
	return root* (pos * rot * scale);

}

void GraphNode::SetPosition(glm::vec3 position) {

	m_Position = position;

}

void GraphNode::SetRotation(glm::vec3 rotation) {
    // Reset the rotation matrix
    m_Rotation = glm::mat4(1.0f);

    // Convert to radians and apply rotations in ZYX order
    glm::vec3 radians = glm::radians(rotation);

    m_Rotation = glm::rotate(m_Rotation, radians.z, glm::vec3(0, 0, 1)); // Roll
    m_Rotation = glm::rotate(m_Rotation, radians.y, glm::vec3(0, 1, 0)); // Yaw
    m_Rotation = glm::rotate(m_Rotation, radians.x, glm::vec3(1, 0, 0)); // Pitch
}

void GraphNode::SetScale(glm::vec3 scale) {
	m_Scale = scale;
}

void GraphNode::Move(glm::vec3 offset) {
	glm::vec3 rotatedOffset = glm::vec3(m_Rotation * glm::vec4(offset, 1.0f));
	m_Position += rotatedOffset;
	m_Updated = true;
	
	if (GetComponent<StaticMeshComponent>()!=nullptr) {

		printf("Updating!!\n");
		GetComponent<StaticMeshComponent>()->Updated();

	}

}

glm::mat4 f42g(const Diligent::float4x4& diligentMat)
{
	
	return glm::mat4(
		diligentMat._11, diligentMat._21, diligentMat._31, diligentMat._41,  // GLM column 0
		diligentMat._12, diligentMat._22, diligentMat._32, diligentMat._42,  // GLM column 1
		diligentMat._13, diligentMat._23, diligentMat._33, diligentMat._43,  // GLM column 2
		diligentMat._14, diligentMat._24, diligentMat._34, diligentMat._44   // GLM column 3
	);

}

void GraphNode::SetRotation(float4x4 rot) {

	m_Rotation = f42g(rot);

}

void GraphNode::Update(float dt) {


	for (auto& component : m_Components) {
		component->OnUpdate(dt);
	}

	for (auto sub : m_Nodes) {
		sub->Update(dt);
	}

}

void GraphNode::Turn(glm::vec3 delta, bool local) {

	auto rot = glm::mat4(1.0f);

	// Convert to radians and apply rotations in ZYX order
	glm::vec3 radians = glm::radians(delta);


	rot = glm::rotate(rot, radians.z, glm::vec3(0, 0, 1)); // Roll
	rot = glm::rotate(rot, radians.y, glm::vec3(0, 1, 0)); // Yaw
	rot = glm::rotate(rot, radians.x, glm::vec3(1, 0, 0)); // Pitch

	if (local) {
		m_Rotation = m_Rotation * rot;
	}
	else {
		m_Rotation = rot * m_Rotation;
	}
}

void GraphNode::Translate(glm::vec3 delta) {

	m_Position += delta;

}

void GraphNode::Push() {

	m_PositionPush = m_Position;
	m_ScalePush = m_Scale;
	m_RotationPush = m_Rotation;

	for (auto comp : m_Components) {

		comp->Push();

	}

	for (auto node : m_Nodes) {
		node->Push();
	}
	

}

void GraphNode::Pop() {

	m_Position = m_PositionPush;
	m_Scale = m_ScalePush;
	m_Rotation = m_RotationPush;

	for (auto comp : m_Components) {

		comp->Pop();

	}

	for (auto node : m_Nodes) {
		node->Pop();
	}
}

glm::vec3 GraphNode::GetWorldPosition() {

	glm::vec3 root = glm::vec3(0, 0, 0);

	if (m_RootNode) {

		root = m_RootNode->GetWorldPosition();

	}

	return root + m_Position;

}

glm::mat4 GraphNode::GetWorldRotation() {

	glm::mat4 root = glm::mat4(1.0f);

	if (m_RootNode) {
		root = m_RootNode->GetWorldRotation();
		
	}

	return root * m_Rotation;

}

void GraphNode::Play() {

	for (auto comp : m_Components) {

		comp->OnPlay();

	}

	for (auto node : m_Nodes) {

		node->Play();

	}

	switch (m_BodyType) {
	case T_Box:
		if (m_RB != nullptr) {
			Vivid::m_Physics->RemoveActor(m_RB);
		}
		//SetBody(BodyType::T_Box);
		CreateRB();
		CreateBody();

		break;
	case T_TriMesh:

		if (m_RS != nullptr) {
			Vivid::m_Physics->RemoveStatic(m_RS);
		}


		CreateBody();

		break;
	case T_ConvexHull:
		if (m_RB != nullptr) {
			Vivid::m_Physics->RemoveActor(m_RB);
		}
		CreateBody();
		break;
	}

}

void GraphNode::Stop() {

	for (auto comp : m_Components) {

		comp->OnStop();

	}

	for (auto node : m_Nodes) {

		node->Stop();

	}

}

Bounds GraphNode::GetStaticMeshBounds(bool includeChildren)  {
	Bounds totalBounds;
	bool foundAnyVertices = false;

	// Get all StaticMeshComponent instances from this node
	auto staticMeshComponents = GetComponents<StaticMeshComponent>();

	for (auto meshComp : staticMeshComponents) {
		const auto& subMeshes = meshComp->GetSubMeshes();

		for (const auto& subMesh : subMeshes) {
			if (subMesh->m_Vertices.empty()) continue;

			// Get world matrix for this submesh's owner
			glm::mat4 worldMatrix = subMesh->m_Owner ? subMesh->m_Owner->GetWorldMatrix() : glm::mat4(1.0f);

			// Process each vertex
			for (const auto& vertex : subMesh->m_Vertices) {
				// Transform vertex to world space
				glm::vec4 worldPos = glm::vec4(vertex.position, 1.0f);
				glm::vec3 worldPos3 = glm::vec3(worldPos);

				if (!foundAnyVertices) {
					// First vertex - initialize bounds
					totalBounds.min = worldPos3;
					totalBounds.max = worldPos3;
					foundAnyVertices = true;
				}
				else {
					// Expand bounds
					totalBounds.min = glm::min(totalBounds.min, worldPos3);
					totalBounds.max = glm::max(totalBounds.max, worldPos3);
				}
			}
		}
	}

	// Recursively include children if requested
	if (includeChildren) {
		for (const auto& childNode : m_Nodes) {
			Bounds childBounds = childNode->GetStaticMeshBounds(true);

			if (childBounds.IsValid()) {
				if (!foundAnyVertices) {
					totalBounds = childBounds;
					foundAnyVertices = true;
				}
				else {
					totalBounds.min = glm::min(totalBounds.min, childBounds.min);
					totalBounds.max = glm::max(totalBounds.max, childBounds.max);
				}
			}
		}
	}

	// Calculate derived values (size and center)
	totalBounds.CalculateDerivedValues();

	return totalBounds;
}

void GraphNode::SetBody(BodyType type) {

	m_BodyType = type;

}

glm::mat4 PxQuatToGlmMat4(const physx::PxQuat& quat) {
	// Convert PxQuat to glm::quat (note: glm::quat constructor order is w, x, y, z)
	glm::quat glmQuat(quat.w, quat.x, quat.y, quat.z);

	// Normalize just in case
	glmQuat = glm::normalize(glmQuat);

	// Convert glm quaternion to 4x4 rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(glmQuat);

	return rotationMatrix;
}

void GraphNode::CreateBody() {
	PxMaterial* material = Vivid::m_Physics->CreateMaterial(); // static friction, dynamic friction, restitution


	switch (m_BodyType)
	{
	case BodyType::T_Box:
	{
		auto bounds = GetStaticMeshBounds(true);

		auto size = bounds.max - bounds.min;

		m_BoxBody = PxBoxGeometry(size.x/2.0, size.y/2.0, size.z/2.0);
		m_Shape = PxRigidActorExt::createExclusiveShape(*m_RB, m_BoxBody, *material);

		Vivid::m_Physics->AddActor(m_RB);
	}
		break;
	case BodyType::T_TriMesh:
	{
		std::vector<PxVec3> vertices;   // Your mesh's vertex positions
		std::vector<PxU32> indices;
		PxU32 base = 0;
		for (auto comp : GetComponents<StaticMeshComponent>())
		{



			for (auto mesh : comp->GetSubMeshes()) {

				for (auto v : mesh->m_Vertices) {

					auto p = GetWorldMatrix() * glm::vec4(v.position, 1.0);

					PxVec3 pos = PxVec3(p.x,p.y,p.z);
					vertices.push_back(pos);

				}


				for (auto t : mesh->m_Triangles) {

					indices.push_back(base + (PxU32)t.v0);
					indices.push_back(base + (PxU32)t.v1);
					indices.push_back(base + (PxU32)t.v2);
					indices.push_back(base + (PxU32)t.v0);
					indices.push_back(base + (PxU32)t.v2);
					indices.push_back(base + (PxU32)t.v1);


				}

				base += mesh->m_Vertices.size();



			}

		}
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = static_cast<PxU32>(vertices.size());
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = vertices.data();

		meshDesc.triangles.count = static_cast<PxU32>(indices.size()) / 3;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = indices.data();

		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;
		bool success = Vivid::m_Physics->GetCooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
		if (!success || result != PxTriangleMeshCookingResult::eSUCCESS)
		{
			throw std::runtime_error("Failed to cook triangle mesh!");
		}

		// 3. Create triangle mesh from cooked data
		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxTriangleMesh* triMesh = Vivid::m_Physics->GetPhysics()->createTriangleMesh(readBuffer);

		// 4. Create triangle mesh geometry
		PxTriangleMeshGeometry triGeom(triMesh);

		// 5. Create a static actor
		PxTransform pose(PxVec3(0.0f, 0.0f, 0.0f));  // Position in world
		PxRigidStatic* staticActor = Vivid::m_Physics->GetPhysics()->createRigidStatic(pose);

		// 6. Attach shape to static actor
		physx::PxShape* shape = PxRigidActorExt::createExclusiveShape(*staticActor, triGeom, *material);
		// 7. Add actor to scene

		Vivid::m_Physics->AddStatic(staticActor);
		m_RS = staticActor;
	}
	

		break;
	case T_ConvexHull:
		std::vector<PxVec3> vertices;
		for (auto comp : GetComponents<StaticMeshComponent>())
		{
			for (auto mesh : comp->GetSubMeshes())
			{
				for (auto& v : mesh->m_Vertices)
				{
					glm::vec4 worldPos = glm::vec4(v.position, 1.0f);
					vertices.emplace_back(worldPos.x, worldPos.y, worldPos.z);
				}
			}
		}

		// 1. Convex mesh descriptor
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = static_cast<PxU32>(vertices.size());
		convexDesc.points.stride = sizeof(PxVec3);
		convexDesc.points.data = vertices.data();
		convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		// 2. Cook convex mesh
		PxDefaultMemoryOutputStream writeBuffer;
		PxConvexMeshCookingResult::Enum result;
		bool success = Vivid::m_Physics->GetCooking()->cookConvexMesh(convexDesc, writeBuffer, &result);
		if (!success || result != PxConvexMeshCookingResult::eSUCCESS)
		{
			throw std::runtime_error("Failed to cook convex hull!");
		}

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxConvexMesh* convexMesh = Vivid::m_Physics->GetPhysics()->createConvexMesh(readBuffer);

		// 3. Create convex geometry
		PxConvexMeshGeometry convexGeom(convexMesh);

		// 4. Create dynamic rigid body
		PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f));  // Or set based on your object
		PxRigidDynamic* dynamicActor = Vivid::m_Physics->GetPhysics()->createRigidDynamic(transform);

		// 5. Create shape and attach to actor
		m_Shape = PxRigidActorExt::createExclusiveShape(*dynamicActor, convexGeom, *material);
		PxTransform t;
		t.p = PxVec3(m_Position.x, m_Position.y, m_Position.z);
		t.q = glmMat4ToPxQuat(m_Rotation);
		dynamicActor->setGlobalPose(t, true);

		// 6. Add actor to scene
		Vivid::m_Physics->AddActor(dynamicActor);
		m_RB = dynamicActor;  // Store handle to the rigid body
		break;
	}



}

void GraphNode::CreateRB() {

	m_RB = Vivid::m_Physics->CreateRB();
	
	PxTransform t;
	t.p = PxVec3(m_Position.x, m_Position.y, m_Position.z);
	t.q = glmMat4ToPxQuat(m_Rotation);
	m_RB->setGlobalPose(t, true);



}

void GraphNode::UpdatePhysics() {

	if (m_RB) {
		PxTransform t = m_RB->getGlobalPose();
		m_Position = glm::vec3(t.p.x, t.p.y, t.p.z);
		m_Rotation = PxQuatToGlmMat4(t.q);
	}

	for (auto node : m_Nodes) {
		node->UpdatePhysics();
	}

}