#include "LightComponent.h"
#include "RenderTargetCube.h"
LightComponent::LightComponent() {

	m_ShadowMap = new RenderTargetCube(2048,2048, true);

}

float LightComponent::GetRange() {

	return m_Range;

}

glm::vec3 LightComponent::GetColor() {
	return m_Color;
}
