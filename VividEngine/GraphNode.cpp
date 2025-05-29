
#include "GraphNode.h"
#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
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

glm::mat4 GraphNode::GetWorldMatrix(){

	glm::mat4 root = glm::mat4(1.0f);

	if (m_RootNode != nullptr) {
		root = m_RootNode->GetWorldMatrix();
	}

	glm::mat4 transform = glm::mat4(1.0f);
	auto pos = glm::translate(transform, m_Position);
	auto rot  = m_Rotation;
	auto scale  = glm::scale(transform, m_Scale);
	return root*( pos*rot);

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
	glm::vec3 rotatedOffset = glm::vec3(m_Rotation * glm::vec4(offset, 0.0f));
	m_Position += rotatedOffset;
}