#include "SkySystem.h"
#include "GraphNode.h"
#include "Importer.h"
#include "MaterialSky.h"
#include "StaticRendererComponent.h"
#include "StaticMeshComponent.h"
#include "SceneGraph.h"
#include "LightComponent.h"
#include <iostream>
//#include <glm/gtc/common.hpp>
#include <algorithm> // For std::max/min
const glm::vec3 ZENITH_COLOR_NIGHT = { 0.05f, 0.05f, 0.1f };  // Dark Blue
const glm::vec3 ZENITH_COLOR_RISE = { 0.1f, 0.2f, 0.5f };   // Lighter Blue for sunrise/sunset
const glm::vec3 ZENITH_COLOR_DAY = { 0.2f, 0.4f, 0.8f };    // Bright Blue

// Horizon Colors
const glm::vec3 HORIZON_COLOR_NIGHT = { 0.1f, 0.1f, 0.2f }; // Darker Blue
const glm::vec3 HORIZON_COLOR_RISE = { 0.9f, 0.4f, 0.2f };  // Fiery Orange for sunrise/sunset
const glm::vec3 HORIZON_COLOR_DAY = { 0.5f, 0.7f, 1.0f };

SkySystem::SkySystem() {

	auto imp = new Importer;

	m_SkyDome = imp->ImportEntity("Edit/Primitives/Sphere.fbx");
	m_SkyDome->SetScale(glm::vec3(130,130,130));
	m_SkyMaterial = new MaterialSky;

	for (auto sm : m_SkyDome->GetNodes()[0]->GetComponent<StaticMeshComponent>()->GetSubMeshes()) {

		sm->m_Material = m_SkyMaterial;

	}
	m_PlanetRadius = 6371.0f; // An Earth-like radius
	m_AtmosphereRadius = m_PlanetRadius * 1.025f;
	m_SunIntensity = 22.0f;

	// RGB coefficients for Rayleigh scattering - this creates the blue sky
	m_RayleighCoeff = glm::vec3(5.802f, 9.473f, 19.644f) * 0.0001f;
	m_MieCoeff = 0.0002f;

}

GraphNode* SkySystem::GetDome() {

	return m_SkyDome;

}

void SkySystem::RenderSky(GraphNode* camera) {

	m_SkyDome->Render(camera);
   
    auto pos = camera->GetPosition();
    pos.y = 0;
    m_SkyDome->SetPosition(pos);
     
}

void SkySystem::Update() {

    if(m_SunLight==nullptr) return;
    // 1. Calculate Sun Direction and Colors
    float sunAngle = timeOfDay * 2.0f * 3.14159f;
    m_SunDir = { cos(sunAngle), 0, sin(sunAngle) };

   

    m_SkyMaterial->m_Time = m_TotalTime;
    std::cout << "Time:" << m_TotalTime << std::endl;

    // CRITICAL FIX: Normalize the sun direction!
    m_SunDir = glm::normalize(m_SunDir);

    glm::vec3 cameraPos = SceneGraph::m_Instance->GetCamera()->GetPosition();

    // A very large number to place the gizmo far away
    float distance = 140.0f;

    // Calculate a cosmetic position for the sun
    glm::vec3 sunGizmoPosition = glm::vec3(m_SunDir.x,m_SunDir.z,m_SunDir.y ) * distance;
    
    auto pos = m_SunLight->GetPosition();
    //m_SunLight->SetPosition(sunGizmoPosition);
   // m_SunLight->SetPosition(glm::vec3(0, 0, 0));
    m_SunLight->LookAt(-glm::vec3(m_SunDir.x, m_SunDir.z, m_SunDir.y));
    
    auto lc = m_SunLight->GetComponent<LightComponent>();
    lc->SetDirection(glm::vec3(m_SunDir.x, m_SunDir.z, m_SunDir.y));
    //m_SunLight->SetDirection(-glm::vec3(m_SunDir.x, m_SunDir.z, m_SunDir.y));


    float sunHeight = m_SunDir.z;

    const float baseSunIntensity = 22.0f;
    // NEW: Define the minimum brightness for the night sky
    const float minNightIntensity = 2.25f;

    // Create a factor that smoothly transitions from 1.0 (day) to 0.0 (night)
    float daylightFactor = glm::smoothstep(-0.05f, 0.15f, sunHeight);

  //  const float baseSunIntensity = 22.0f;
   // const float minNightIntensity = 0.05f; // Keeps the sky from being pure black
    m_SunIntensity = (baseSunIntensity * daylightFactor) + minNightIntensity;

    // THE FIX: Ensure the intensity never drops below our defined minimum
 

    // --- Update Directional Light ---
    if (m_SunLight != nullptr)
    {
        m_SunLight->LookAt(-m_SunDir);

        const float baseLightIntensity = 1.0f;
        float sunVisibility = glm::smoothstep(0.0f, 0.1f, sunHeight);
        m_SunLight->GetComponent<LightComponent>()->SetIntensity(baseLightIntensity * sunVisibility);
    }


  //  m_SunLight->GetComponent<LightComponent>()->SetIntensity(sunVisibility);





    //float sunHeight = m_SunDir.z;

    // 2. Interpolate Sky Colors based on the sun's height
    if (sunHeight > 0.0f) // Daytime (Sunrise -> Noon -> Sunset)
    {
        // 'noonFactor' is 0.0 at the horizon and 1.0 at the zenith (noon)
        float noonFactor = sunHeight;
        m_ZenithColor = lerp(ZENITH_COLOR_RISE, ZENITH_COLOR_DAY, noonFactor);
        m_HorizonColor = lerp(HORIZON_COLOR_RISE, HORIZON_COLOR_DAY, noonFactor);
    }
    else // Nighttime (Sunset -> Midnight -> Sunrise)
    {
        // 'midnightFactor' is 0.0 at the horizon and 1.0 at midnight
        float midnightFactor = -sunHeight;
        // We re-use the RISE colors here for the sunset-to-night transition
        m_ZenithColor = lerp(ZENITH_COLOR_RISE, ZENITH_COLOR_NIGHT, midnightFactor);
        m_HorizonColor = lerp(HORIZON_COLOR_RISE, HORIZON_COLOR_NIGHT, midnightFactor);
    }

    // 3. Send the final calculated data to the material
    m_SkyMaterial->m_SunDir = m_SunDir;
    m_SkyMaterial->m_ZenithColor = m_ZenithColor;
    m_SkyMaterial->m_HorizonColor = m_HorizonColor;

    m_SkyMaterial->m_CameraPos = glm::vec3(0, 0, 0);// SceneGraph::m_Instance->GetCamera()->GetPosition();
    m_SkyMaterial->m_AtmosRadius = m_AtmosphereRadius;
    m_SkyMaterial->m_PlanetRadius = m_PlanetRadius;
    m_SkyMaterial->m_SunIntense = m_SunIntensity;
    m_SkyMaterial->m_RayLeigh = m_RayleighCoeff;
    m_SkyMaterial->m_Mie = m_MieCoeff;
    m_SkyMaterial->m_DayLight = daylightFactor;

    //float daylightFactor = glm::smoothstep(-0.1f, 0.05f, sunHeight);
    //if (timeOfDay > 0.5) {
    
    //}

    std::cout << "TOD:" << timeOfDay << std::endl;



}

glm::vec3 SkySystem::lerp(const glm::vec3& a, const glm::vec3& b, float t)
{
	// Clamp t to the 0-1 range to ensure valid color values
	t = std::max(0.0f, std::min(1.0f, t));
	return a * (1.0f - t) + b * t;
}
