#pragma once
#include <glm/glm.hpp>


class TranslateGizmo;
class RotateGizmo;
class ScaleGizmo;
class SceneGraph;
class GraphNode;
class SceneGizmo;

enum GizmoMode {

    Gizmo_Translate,Gizmo_Rotate,Gizmo_Scale

};

enum EditSpace {

    Edit_Local,Edit_World,Edit_Smart

};



class SceneController
{
public:
    
    SceneController();

    void onMouseClick(glm::vec2 pos);
    void onMouseUp();
    void onMouseMove(glm::vec2 pos);
    void onMouseRelease(glm::vec2 pos);
//    void onRender(); // Optional debug visuals

    void setScene(SceneGraph* scene) { m_Scene = scene; }
    void setCamera(GraphNode* camera) { m_Camera = camera; }
    void Init();
    void Render();
    void SetMode(GizmoMode mode);
    void SetSpace(EditSpace space);
    void AlignGizmos();
    GraphNode* GetSelected();
private:
    SceneGraph* m_Scene = nullptr;
    GraphNode* m_Camera = nullptr;
    GraphNode* m_OverNode = nullptr;
    GraphNode* m_SelectedNode = nullptr;
    bool m_GizmoActive = false;
    bool m_Processing = false;

    //GraphNode* m_TranslateNode, * m_RotateNode, * m_ScaleNode;
    TranslateGizmo* m_TranslateGiz;
    RotateGizmo* m_RotateGiz;
    ScaleGizmo* m_ScaleGiz;
    GizmoMode m_Mode = GizmoMode::Gizmo_Translate;


    SceneGizmo* m_CurrentGizmo = nullptr;

    //std::unique_ptr<GizmoTool> gizmo;


};

