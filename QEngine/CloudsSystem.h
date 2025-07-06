#pragma once
#include <vector> // Required for std::vector
#include <glm/glm.hpp>

class Texture3D;
class GraphNode;
class MaterialClouds;

enum CloudsQuality {

	CQ_Ultra,CQ_High,CQ_Mid,CQ_Low

};

class CloudsSystem
{
public:

	CloudsSystem(CloudsQuality quality);
	void Update(float dt);
	void Render(GraphNode* camera);
	void CreateTestCloudMap();
	void SetMesh(GraphNode* m_Sky);
	void SetSunDir(glm::vec3 dir);
	void CreateClouds(float time);
	float m_Coverage = 0.6f;
	float m_AnimationSpeed = 0.5f;
	float m_TotalTime;
private:

	Texture3D* m_CloudMap = nullptr;
	GraphNode* m_Sky = nullptr;
	CloudsQuality m_Quality = CloudsQuality::CQ_Mid;
	float m_Width, m_Height, m_Depth;
	MaterialClouds* m_CloudMaterial = nullptr;
	glm::vec3 m_SunDir;
	glm::vec3 m_CloudVolumeStart;
	glm::vec3 m_CloudVolumeSize;

};

