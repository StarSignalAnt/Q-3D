
#include "GraphNode.h"
#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "BasicMath.hpp"
#include <glm/glm.hpp>
#include "StaticMeshComponent.h"

using namespace Diligent;

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

}

void GraphNode::Stop() {

	for (auto comp : m_Components) {

		comp->OnStop();

	}

	for (auto node : m_Nodes) {

		node->Stop();

	}

}