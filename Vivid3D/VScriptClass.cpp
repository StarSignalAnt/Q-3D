#include "VScriptClass.h"
#include "Editor.h"
#include "Node.h"
#include "SceneGraph.h"
#include "VClass.h"
#include "VVar.h"
#include "VPropEditor.h"
#include "VPropertyEditor.h"
VScriptClass::VScriptClass(QWidget *parent)
	: QLineEdit(parent)
{
	ui.setupUi(this);
    setAcceptDrops(true);
}


void VScriptClass::dragEnterEvent(QDragEnterEvent* event) {
//    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();


    //}

}

void VScriptClass::dropEvent(QDropEvent* event) {


    std::string node_url = event->mimeData()->text().toStdString();

    auto gnode = Editor::m_Graph->FindNode(node_url);

    for (auto s : gnode->GetScripts()) {

        if (s->GetName().GetNames()[0] == m_ClassType)
        {
            //m_Entry = new VVar;

            //m_Entry->SetType(T_Class);
            //m_Entry->SetClassType(m_ClassType);
            m_Entry->SetClassValue(s);
            s->SetDataName(gnode->GetFullName());
            Editor::m_PropEditor->SetNode(Editor::m_CurrentNode);
        }

    };

    int b = 5;

    return;
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        QString filePath;
        if (!urls.isEmpty() && urls.first().isLocalFile()) {
            filePath = urls.first().toLocalFile();

            int b = 5;
           
            
            // if (m_OverItem != nullptr) {
            //    auto node = (Node*)m_OverItem->m_Data;
            //    node->AddScript(filePath.toStdString());
            //}


            int bb = 5;
            //setPixmap(QPixmap(filePath));
            //SetImage(filePath.toStdString());
            //emit dropped(filePath); // Emit the custom dropped sig
        }
        event->acceptProposedAction();
    }
}

void VScriptClass::dragMoveEvent(QDragMoveEvent* event)
{
    //    QPoint localPos = event->pos();
    //    qDebug() << "Drag Move at: " << localPos;

  //  m_OverItem = nullptr;
  //  QPoint globalPos = QCursor::pos();
  //  QPoint localPos = mapFromGlobal(globalPos);
    //auto pos = QPointF(localPos.x(), localPos.y());// event->position();
    //printf("PX:%d PY:%d \n", (int)pos.x(), (int)pos.y());
  //  int y = CheckItem(m_Root, 25, 5, (int)pos.x(), (int)pos.y());
  //  update();
  //  setMinimumSize(width(), y + 40);
  //  update();
  //  update();


    event->acceptProposedAction();
}



VScriptClass::~VScriptClass()
{}
