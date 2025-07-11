#pragma once
#include "Component.h"
#include "CloudsSystem.h"
#include "SkySystem.h"

class SkyComponent : public Component
{
public:

	SkyComponent();
	Component* CreateInstance() override {
		return new SkyComponent;

	}
	void OnUpdate(float dt) override;
	void OnRender(GraphNode* camera) override;
	
private:

	std::unique_ptr<CloudsSystem> m_Clouds;
	SkySystem* m_Sky;
	float TimeOfDay = 0.0f;
	GraphNode* m_SunLight;
};

