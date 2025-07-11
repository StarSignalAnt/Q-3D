#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // Functions like glm::translate, glm::rotate, glm::perspective
#include <glm/gtc/type_ptr.hpp> 
#include <vector>
#include <string>
#include "BasicMath.hpp"
#include <glm/glm.hpp>
#include "PxPhysicsAPI.h"
#include "VFile.h"
#include "Properties.h"
// ADDED: Include for nlohmann/json
#include "json.hpp" // Assumes nlohmann/json.hpp is in your project include paths


using json = nlohmann::json;
using namespace physx;
using namespace Diligent;
class Component;


#include "json.hpp" // Assumes nlohmann/json.hpp is in your project include paths
using json = nlohmann::json;
// END ADDED

// --- HELPER FUNCTIONS for glm serialization ---
// By specializing the adl_serializer struct inside the nlohmann namespace,
// we provide a robust way for the library to find these conversion functions.
namespace nlohmann {
	template <>
	struct adl_serializer<glm::vec3> {
		static void to_json(json& j, const glm::vec3& v) {
			j = { v.x, v.y, v.z };
		}

		static void from_json(const json& j, glm::vec3& v) {
			j.at(0).get_to(v.x);
			j.at(1).get_to(v.y);
			j.at(2).get_to(v.z);
		}
	};

	template <>
	struct adl_serializer<glm::mat4> {
		static void to_json(json& j, const glm::mat4& m) {
			// Flatten the matrix into a 1D array for serialization
			j = {
				m[0][0], m[0][1], m[0][2], m[0][3],
				m[1][0], m[1][1], m[1][2], m[1][3],
				m[2][0], m[2][1], m[2][2], m[2][3],
				m[3][0], m[3][1], m[3][2], m[3][3]
			};
		}

		static void from_json(const json& j, glm::mat4& m) {
			for (int col = 0; col < 4; ++col) {
				for (int row = 0; row < 4; ++row) {
					j.at(col * 4 + row).get_to(m[col][row]);
				}
			}
		}
	};
}

enum ResourceType {
	Static,Skeletal,Light,Camera,SubData
};

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
	void Debug() {

		auto size = max - min;

		std::cout << "W:" << size.x << " H:" << size.y << " D:" << size.z << std::endl;

	}
};

enum NodeRenderType {
	RenderType_Static,RenderType_Dynamic
};

class GraphNode
{
public:

	Properties& GetProperties() {
		return m_Properties;
	}
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
	std::vector<Component*> GetAllComponents() {
		std::vector<Component*> res;
		
		for (auto c : m_Components) {
			res.push_back(c);
		}

		return res;

	}
	std::string GetLongName() {

		std::string root = "";
		if (m_RootNode != nullptr) {
			root = m_RootNode->GetName();
		}

		return root + "." + m_Name;

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
	std::string& GetName() {
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
	glm::vec3 TransformVector(glm::vec3 transform) {

		return m_Rotation * glm::vec4(transform, 1.0);

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
	void Write(VFile* f);
	void Read(VFile* f);
	void SetResource(std::string path, ResourceType type) {
		m_ResourcePath = path;
		m_ResourceType = type;
	}
	std::string GetResourcePath() {
		return m_ResourcePath;
	}
	ResourceType GetResourceType() {
		return m_ResourceType;
	}
	void WriteScripts(VFile* f);
	void ReadScripts(VFile* f);
	void RemoveParent();
	GraphNode* FindNode(std::string name);
	void JWrite(json& j) ;
	void JRead(const json& j);
	void JWriteScripts(json& j);
	void JReadScripts(const json& j);
	void SetRenderType(NodeRenderType type,bool include_children=true) {
		m_RenderType = type;
		if (include_children) {
			for (auto node : m_Nodes) {
				node->SetRenderType(type, true);
			}
		}
	}
	NodeRenderType GetRenderType() {
		return m_RenderType;
	}
	void LookAt(glm::vec3 pos);
private:

	Bounds* m_Bounds = nullptr;
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::mat4 m_Rotation;
	glm::vec3 m_PositionPush;
	glm::vec3 m_ScalePush;
	glm::mat4 m_RotationPush;

	void MarkBoundsAsDirty();

	bool m_bBoundsAreDirty = true;
	Bounds m_CachedBounds;


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
	std::string m_ResourcePath = "";
	ResourceType m_ResourceType = ResourceType::SubData;
	NodeRenderType m_RenderType = NodeRenderType::RenderType_Dynamic;

	Properties m_Properties;
	
};

