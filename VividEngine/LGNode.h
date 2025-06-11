#pragma once
#include <vector>
#include <string>
#include "GraphNode.h"
#include <glm/glm.hpp>

class LGNode
{
public:

	//template<typename T>
	//T GetValue();
	virtual int GetInt();
	virtual std::string GetString();
	virtual GraphNode* GetNode();
	virtual glm::vec3 GetVec3();
	virtual float GetFloat();


	virtual void Exec();
	virtual void ExecNext();
	virtual LGNode* GetInput(std::string name);
	void AddInput(std::string name) {

	}
	void AddOutput(std::string name);

	void SetName(std::string name) {
		m_Name = name;
	}
	std::string GetName() {
		return m_Name;
	}
protected:

	std::string m_Name = "";
	std::unordered_map<std::string, LGNode*> m_Inputs;
	std::unordered_map<std::string, LGNode*> m_Outputs;

};

