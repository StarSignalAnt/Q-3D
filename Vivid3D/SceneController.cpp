#include "SceneController.h"
#include "SceneGraph.h"
#include "GraphNode.h"
#include "TranslateGizmo.h"
#include "RotateGizmo.h"
#include "Vivid.h"

SceneController::SceneController() {

    m_TranslateGiz = new TranslateGizmo;
    m_RotateGiz = new RotateGizmo;
    m_CurrentGizmo = m_RotateGiz;

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
            m_CurrentGizmo->SetNode(res.m_Node);
            m_GizmoActive = false;
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
    m_TranslateGiz->SetGraph(m_Scene);
    m_RotateGiz->SetGraph(m_Scene);

    //m_Scene->AddNode(node);


}

void SceneController::Render() {

    Vivid::ClearZ();
    if (m_CurrentGizmo != nullptr) {
        m_CurrentGizmo->Render(m_Scene->GetCamera());
    }

}