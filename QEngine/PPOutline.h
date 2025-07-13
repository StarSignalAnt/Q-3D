#pragma once
#include "PostProcess.h"

class GraphNode;
class SceneGraph;

class PPOutline :
    public PostProcess
{
public:
    PPOutline();
    Q3D::Engine::Texture::Texture2D* Process(Q3D::Engine::Texture::Texture2D* frame);
    void SetNode(GraphNode* node);
    void SetCam(GraphNode* cam);
private:

    GraphNode* m_Node = nullptr;
    SceneGraph* m_Scene = nullptr;
    GraphNode* m_Camera = nullptr;

};

