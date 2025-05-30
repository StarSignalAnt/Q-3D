#include "LightComponent.h"
#include "RenderTargetCube.h"
LightComponent::LightComponent() {

	m_ShadowMap = new RenderTargetCube(1024, 1024, true);

}

float LightComponent::GetRange() {

	return m_Range;

}

glm::vec3 LightComponent::GetColor() {
	return m_Color;
}
