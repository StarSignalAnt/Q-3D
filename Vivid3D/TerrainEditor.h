#pragma once
#include <GraphNode.h>
#include <glm/glm.hpp>
#include "Texture2D.h"

enum EditorState {

	E_Paint,E_Sculpt,E_None

};

class TerrainEditor
{
public:

	TerrainEditor(GraphNode* node) {
		m_Terrain = node;
	}
	void BeginPaint();
	void EndPaint();
	void BeginSculpt();
	void EndSculpt();
	void Update();
	void SetPosition(glm::vec3 pos) {
		m_EditPos = pos;
	}
	void Paint();
	void Sculpt();
	void SetEditLayer(int layer);
private:

	GraphNode* m_Terrain;
	EditorState m_State = EditorState::E_None;
	glm::vec3 m_EditPos = glm::vec3(0, 0, 0);
	float m_TerrainBrushSize = 0.75;
	GraphNode* m_BrushNode = nullptr;
	Texture2D* m_White;
	float m_TerrainStrength = 0.5f;
	bool m_TerrainEditing = false;
	int m_TerrainLayer = 0;
};

