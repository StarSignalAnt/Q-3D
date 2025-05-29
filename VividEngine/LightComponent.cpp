#include "LightComponent.h"

LightComponent::LightComponent() {

}

float LightComponent::GetRange() {

	return m_Range;

}

glm::vec3 LightComponent::GetColor() {
	return m_Color;
}
