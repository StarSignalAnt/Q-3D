#include "SceneController.h"
#include "SceneGraph.h"
#include "GraphNode.h"
#include "TranslateGizmo.h"
#include "RotateGizmo.h"
#include "ScaleGizmo.h"
#include "QEngine.h"

SceneController::SceneController() {

    m_TranslateGiz = new TranslateGizmo;
    m_RotateGiz = new RotateGizmo;
    m_ScaleGiz = new ScaleGizmo;
    //m_CurrentGizmo = m_RotateGiz;
    SetMode(GizmoMode::Gizmo_Translate);

}

void SceneController::onMouseClick(glm::vec2 pos) {

    if (!m_Processing) {
        

        if (m_SelectedNode != nullptr) {
            if (m_CurrentGizmo != nullptr) {
                if (m_CurrentGizmo->Click(pos.x, pos.y)) {

                    m_GizmoActive = true;
                    return;

                }
                else {
                    m_GizmoActive = false;
                }

            }
        }
        
        m_Processing = true;
        printf("Picking... Proc:%d\n",(int)m_Processing);
        int start = clock();
        auto res = m_Scene->MousePick(pos.x, pos.y);
        int len = clock() - start;
        printf("Took %d ms\n", len);

        if (res.m_Hit) {
            printf("Hit!\n");
            m_SelectedNode = res.m_Node;
            //m_CurrentGizmo->SetNode(res.m_Node);
            m_TranslateGiz->SetNode(res.m_Node);
            m_RotateGiz->SetNode(res.m_Node);
            m_ScaleGiz->SetNode(res.m_Node);
            //m_ScaleGiz
            m_GizmoActive = false;
            m_TranslateGiz->AlignGizmo();
            m_RotateGiz->AlignGizmo();
            m_ScaleGiz->AlignGizmo();

        }
        else {
            m_SelectedNode = nullptr;
            m_CurrentGizmo->SetNode(nullptr);
            m_GizmoActive = false;
            printf("No Hit.\n");
            
        }

        m_Processing = false;
    }
    else {
        printf("Busy...\n");
    }

}

void SceneController::onMouseUp() {

    m_GizmoActive = false;
}

void SceneController::onMouseMove(glm::vec2 pos) {

    if (m_GizmoActive) {

        m_CurrentGizmo->Move(pos);
    }

}

void SceneController::onMouseRelease(glm::vec2 pos) {

    m_GizmoActive = false;

}

void SceneController::Init() {

    auto node = m_TranslateGiz->GetNode();
    node->SetScale(glm::vec3(1, 1, 1));
    auto rnode = m_RotateGiz->GetNode();
    rnode->SetScale(glm::vec3(1, 1, 1));
    auto snode = m_ScaleGiz->GetNode();
    snode->SetScale(glm::vec3(1, 1, 1));
    m_TranslateGiz->SetGraph(m_Scene);
    m_RotateGiz->SetGraph(m_Scene);
    m_ScaleGiz->SetGraph(m_Scene);

    //m_Scene->AddNode(node);


}

void SceneController::Render() {

    Q3D::Engine::QEngine::ClearZ();

    if (m_CurrentGizmo != nullptr) {
        m_CurrentGizmo->Render(m_Scene->GetCamera());
    }

}

void SceneController::SetMode(GizmoMode mode) {

    m_Mode = mode;
    switch (mode) {
    case Gizmo_Translate:
        m_CurrentGizmo = m_TranslateGiz;
        break;
    case Gizmo_Rotate:
        m_CurrentGizmo = m_RotateGiz;
        break;
    case Gizmo_Scale:
        m_CurrentGizmo = m_ScaleGiz;
        break;
    }
    m_CurrentGizmo->SetNode(m_SelectedNode);
    m_CurrentGizmo->AlignGizmo();

}

void SceneController::SetSpace(EditSpace space) {
    
    switch (space)
    {
    case GizmoSpace::Local:
        m_CurrentGizmo->SetSpace(GizmoSpace::Local);
        m_TranslateGiz->SetSpace(GizmoSpace::Local);
        m_RotateGiz->SetSpace(GizmoSpace::Local);
        break;
    case GizmoSpace::World:
        m_CurrentGizmo->SetSpace(GizmoSpace::World);
        m_TranslateGiz->SetSpace(GizmoSpace::World);
        m_RotateGiz->SetSpace(GizmoSpace::World);
        break;
    }
    m_CurrentGizmo->AlignGizmo();
}

void SceneController::AlignGizmos() {

    m_TranslateGiz->AlignGizmo();
    m_RotateGiz->AlignGizmo();
    m_ScaleGiz->AlignGizmo();

}

GraphNode* SceneController::GetSelected() {

    return m_SelectedNode;

}

void SceneController::SelectNode(GraphNode* node) {

    m_SelectedNode = node;
    //m_CurrentGizmo->SetNode(res.m_Node);
    m_TranslateGiz->SetNode(node);
    m_RotateGiz->SetNode(node);
    m_ScaleGiz->SetNode(node);
    //m_ScaleGiz

    m_GizmoActive = false;
    m_TranslateGiz->AlignGizmo();
    m_RotateGiz->AlignGizmo();
    m_ScaleGiz->AlignGizmo();

}


GraphNode* SceneController::GetGizNode() {

    if (m_CurrentGizmo != nullptr) {

        return m_CurrentGizmo->GetNode();

    }

}