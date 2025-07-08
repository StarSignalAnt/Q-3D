#include "SkyComponent.h"
#include "SceneGraph.h"

SkyComponent::SkyComponent() {

	m_Clouds.reset(new CloudsSystem(CloudsQuality::CQ_High));
	m_Sky = new SkySystem;
	m_SunLight = SceneGraph::m_Instance->GetLights()[0];
	m_Sky->m_SunLight = m_SunLight;
	m_Name = "Atmosphere";

	//m_Properties.set("ComponentName", "Atmosphere");
	m_Properties.bind("Time Of Day", &TimeOfDay);
	m_Properties.bind("Cloud Coversage", &m_Clouds->m_Coverage);
	m_Properties.bind("Animation Speed", &m_Clouds->m_AnimationSpeed);

}

void SkyComponent::OnRender(GraphNode* camera)
{

	// Fallback to old rendering method if octree isn't built.
	m_Sky->RenderSky(camera);

	m_Clouds->Render(camera);

}

void SkyComponent::OnUpdate(float dt) {
	TimeOfDay += 0.001;

	m_Sky->Update();
	m_Clouds->Update(dt);

	auto sdir = m_Sky->m_SunDir;
	float y = sdir.y;
	sdir.y = sdir.z;
	sdir.z = y;

	std::cout << "SDir:" + std::to_string(sdir.x) + " Y:" + std::to_string(sdir.y) + " Z:" + std::to_string(sdir.z) << std::endl;

	m_Clouds->SetSunDir(-sdir);
	m_Sky->m_TotalTime += dt;

	//tod = 0.75f;
	m_Sky->timeOfDay = TimeOfDay;
}