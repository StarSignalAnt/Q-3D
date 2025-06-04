#pragma once
#include "BasicMath.hpp"
#include <glm/glm.hpp>
#include "GraphNode.h"
using namespace Diligent;



class MaterialBasic3D;
class Texture2D;
class RenderMaterial;

class Draw2D
{
public:

	Draw2D(GraphNode* camera);
	void Rect(Texture2D* image, glm::vec2 position, glm::vec2 size, glm::vec4 color);
	void SetMaterial(MaterialBasic3D* material) {
		m_Material = material;
	}
	void SetOverride(RenderMaterial* mat) {
		m_Override = mat;
	}

private:

	MaterialBasic3D* m_Material;
	RenderMaterial* m_Override;
	GraphNode* m_Camera;
};

