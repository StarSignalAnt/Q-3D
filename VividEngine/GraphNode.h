#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // Functions like glm::translate, glm::rotate, glm::perspective
#include <glm/gtc/type_ptr.hpp> 
#include <vector>
#include <string>
#include "BasicMath.hpp"
#include <glm/glm.hpp>
#include "PxPhysicsAPI.h"
using namespace physx;
using namespace Diligent;
class Component;


enum BodyType {
	T_Box,T_Sphere,T_ConvexHull,T_TriMesh,T_TerrainMesh,T_None
};

// Add this struct to GraphNode.h (or create a separate header)
struct Bounds {
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 size;
	glm::vec3 center;

	Bounds() : min(FLT_MAX), max(-FLT_MAX), size(0), center(0) {}

	bool IsValid() const {
		return min.x <= max.x && min.y <= max.y && min.z <= max.z;
	}

	void CalculateDerivedValues() {
		if (IsValid()) {
			size = max - min;
			center = (min + max) * 0.5f;
		}
		else {
			size = glm::vec3(0);
			center = glm::vec3(0);
		}
	}
};

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
	template <typename T>
	std::vector<T*> GetComponents() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		std::vector<T*> res;
		for (Component* comp : m_Components) {
			if (T* casted = dynamic_cast<T*>(comp)) {
				//return casted;
				res.push_back(casted);
			}
		}
		return res;
	}
	void Render(GraphNode* camera);
	void RenderDepth(GraphNode* camera);
	void Update(float dt);
	glm::mat4 GetWorldMatrix();


	void SetPosition(glm::vec3 position);
	void SetRotation(glm::vec3 rotation);
	void SetScale(glm::vec3 scale);
	glm::vec3 GetPosition() { return m_Position; }
	void Move(glm::vec3 vec);
	void Translate(glm::vec3 delta);
	void SetRotation(float4x4 rotation); //converts from de.
	void SetRotation(glm::mat4 rotation) {
		m_Rotation = rotation;
	}
	std::string GetName() {
		return m_Name;
	}
	glm::vec3 GetEularRotation() const {
		return glm::degrees(glm::eulerAngles(glm::quat_cast(m_Rotation)));
	}
	glm::vec3 GetScale() const { return m_Scale; }
	void Turn(glm::vec3 delta, bool local);
	
	glm::mat4 GetRotation() {
		return m_Rotation;
	}
	void Push();
	void Pop();
	glm::vec3 GetWorldPosition();
	glm::mat4 GetWorldRotation();
	void RemoveNode(GraphNode* node) {
		if (!node) return;

		auto it = std::find(m_Nodes.begin(), m_Nodes.end(), node);
		if (it != m_Nodes.end()) {
			m_Nodes.erase(it);
			node->SetRootNode(nullptr); // Clear the parent reference
		}
	}

	// Also add this method to get a non-const reference (alternative solution)
	std::vector<GraphNode*>& GetNodesRef() { return m_Nodes; }
	void Play();
	void Stop();
	Bounds GetStaticMeshBounds(bool includeChildren = true);
	void SetBody(BodyType type);
	void CreateBody();
	void CreateRB();
	void UpdatePhysics();
	BodyType GetBodyType() {
		return m_BodyType;
	}

	void SetHideFromEditor(bool hide) {
		m_HideFromEditor = hide;
	}

	bool GetHideFromEditor() {
		return m_HideFromEditor;
	}

private:

	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::mat4 m_Rotation;
	glm::vec3 m_PositionPush;
	glm::vec3 m_ScalePush;
	glm::mat4 m_RotationPush;


	GraphNode* m_RootNode = nullptr;

	std::vector<GraphNode*> m_Nodes;
	std::string m_Name;
	std::vector<Component*> m_Components;
	bool m_Updated = true;
	BodyType m_BodyType = BodyType::T_None;
	PxBoxGeometry m_BoxBody;
	PxRigidDynamic* m_RB = nullptr;
	PxRigidStatic* m_RS = nullptr;
	PxShape* m_Shape;
	bool m_HideFromEditor = false;
};

