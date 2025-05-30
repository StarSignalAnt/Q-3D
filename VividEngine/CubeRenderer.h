#pragma once
#include "BasicMath.hpp"
#include <glm/glm.hpp>

using namespace Diligent;
class SceneGraph;
class RenderTargetCube;

class CubeRenderer
{
public:

	CubeRenderer(SceneGraph* graph, RenderTargetCube* target);
	void RenderDepth(glm::vec3 position, float maxZ,RenderTargetCube* m_Target);
	void RenderEnvironment(glm::vec3 position);
	RenderTargetCube* GetTarget() {
		return m_Target;
	}

private:

	SceneGraph* m_Graph;
	RenderTargetCube* m_Target;


};

