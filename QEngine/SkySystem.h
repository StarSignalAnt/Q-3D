#pragma once
#include <glm/glm.hpp>
class GraphNode;
class MaterialSky;


class SkySystem
{
public:

	SkySystem();
	GraphNode* GetDome();
	void RenderSky(GraphNode* camera);
	void Update();
	float timeOfDay = 0.3f;

	glm::vec3 lerp(const glm::vec3& a, const glm::vec3& b, float t);

	GraphNode* m_SkyDome;
	MaterialSky* m_SkyMaterial;
	glm::vec3 m_SunDir;
	
	glm::vec3 m_ZenithColor;
	glm::vec3 m_HorizonColor;
	float m_PlanetRadius;
	float m_AtmosphereRadius;
	float m_SunIntensity;
	glm::vec3 m_RayleighCoeff;
	float m_MieCoeff;
	GraphNode* m_SunLight;
	float m_TotalTime = 0.0f;
private:
};

