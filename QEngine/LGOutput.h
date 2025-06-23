#pragma once
#include <string>
#include <vector>
#include "DataType.h"
#include <variant>
#include <glm/glm.hpp>

// Forward declarations
class LNode;
class LGInput;
class GraphNode;
// The types of data an output pin can produce.
enum OutputType {
	Om_INT, Om_FLOAT, Om_STRING, Om_NODE, Om_VEC2, Om_VEC3, Om_VEC4, Om_MAT4
};

class LGOutput
{
public:
	// Constructor requires its parent node and its index within that node's output list.
	LGOutput(LNode* parent, int portIndex, std::string name, DataType type);

	// --- Getters ---
	std::string GetName() const { return m_Name; }
	DataType GetType() const { return m_Type; }
	LNode* getParentNode() const { return m_parentNode; }
	int getPortIndex() const { return m_portIndex; }

	// --- Connection Methods ---
	void addConnection(LGInput* input);
	const std::vector<LGInput*>& getConnections() const { return m_connections; }
	using ValueVariant = std::variant<int, float, bool, std::string, glm::vec2, glm::vec3, glm::vec4,glm::mat4, GraphNode*>;
	void removeConnection(LGInput* input); //
	template<typename T>
	void SetValue(T value) {
		m_value = value;
	}

	const ValueVariant& GetValue() const {
		return m_value;
	}
private:
	std::string m_Name;
	DataType m_Type;

	// --- Core Members ---
	LNode* m_parentNode;
	int m_portIndex;
	std::vector<LGInput*> m_connections;
	ValueVariant m_value;
};