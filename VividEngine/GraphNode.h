#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // Functions like glm::translate, glm::rotate, glm::perspective
#include <glm/gtc/type_ptr.hpp> 
#include <vector>
#include <string>

class Component;

class GraphNode
{
public:

	GraphNode();
	void Reset();
	void AddNode(GraphNode* node);	
	std::vector<GraphNode*> GetNodes() const { return m_Nodes; }
	void SetName(std::string name) { m_Name = name; }
	void SetRootNode(GraphNode* root) { m_RootNode = root; }
	GraphNode* GetRootNode() const { return m_RootNode; }
	void AddComponent(Component* component);
	template <typename T>
	T* GetComponent() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		for (Component* comp : m_Components) {
			if (T* casted = dynamic_cast<T*>(comp)) {
				return casted;
			}
		}
		return nullptr;
	}
	void Render(GraphNode* camera);
	glm::mat4 GetWorldMatrix();

	void SetPosition(glm::vec3 position);
	void SetRotation(glm::vec3 rotation);
	void SetScale(glm::vec3 scale);
	glm::vec3 GetPosition() { return m_Position; }
	void Move(glm::vec3 vec);
private:

	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::mat4 m_Rotation;

	GraphNode* m_RootNode = nullptr;

	std::vector<GraphNode*> m_Nodes;
	std::string m_Name;
	std::vector<Component*> m_Components;

};

