#pragma once
#include "BasicMath.hpp"
#include "MathsHelp.h"

using namespace Diligent;
struct LineVertex {
	float3 position;
	float4 color;
};

class MeshLinesBuffer;
class MaterialMeshLines;

struct Line {
	int V0, V1;
};

class MeshLines
{
public:

	MeshLines();

	void AddVertex(LineVertex vertex) {
		m_Vertices.push_back(vertex);
	}

	void AddLine(Line line) {
		m_Lines.push_back(line);
	}

	std::vector<LineVertex> GetVertices();
	std::vector<Line> GetLines();

	void CreateBuffer();
	void Render();
    MeshLinesBuffer* GetBuffer() {
        return m_Buffer;
    }

    void CreateBox(float3 center, float3 size)
    {
        // Compute half extents
        float3 halfSize = size / 2.0f;

        // Define the 8 corners of the box
        float3 corners[8] = {
            { center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z }, // 0: bottom-back-left
            { center.x + halfSize.x, center.y - halfSize.y, center.z - halfSize.z }, // 1: bottom-back-right
            { center.x - halfSize.x, center.y + halfSize.y, center.z - halfSize.z }, // 2: bottom-front-left
            { center.x + halfSize.x, center.y + halfSize.y, center.z - halfSize.z }, // 3: bottom-front-right
            { center.x - halfSize.x, center.y - halfSize.y, center.z + halfSize.z }, // 4: top-back-left
            { center.x + halfSize.x, center.y - halfSize.y, center.z + halfSize.z }, // 5: top-back-right
            { center.x - halfSize.x, center.y + halfSize.y, center.z + halfSize.z }, // 6: top-front-left
            { center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z }  // 7: top-front-right
        };

        // Define the 12 lines that make up the box
        Line lines[12] = {
            // Bottom face
            { 0, 1 }, { 1, 3 }, { 3, 2 }, { 2, 0 },
            // Top face
            { 4, 5 }, { 5, 7 }, { 7, 6 }, { 6, 4 },
            // Connections between top and bottom faces
            { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
        };

        // Clear existing vertices and lines
        m_Vertices.clear();
        m_Lines.clear();

        // Add vertices for each corner
        for (int i = 0; i < 8; ++i) {
            LineVertex vertex;
            vertex.position = corners[i];
            vertex.color = MathsHelp::getRandomFloat4(0, 1.0f); // White color (adjust as needed)
            m_Vertices.push_back(vertex);
        }

        // Add lines to form the box outline
        for (int i = 0; i < 12; ++i) {
            m_Lines.push_back(lines[i]);
        }

      
    }

private:


	std::vector<LineVertex> m_Vertices;
	std::vector<Line> m_Lines;
	MeshLinesBuffer* m_Buffer;
	MaterialMeshLines* m_Material;

};

