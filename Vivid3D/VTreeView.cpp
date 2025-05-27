#include "VTreeView.h"
#include <QPainter>
#include "Node.h"
#include "Editor.h"
#include "VPropertyEditor.h"
#include "qdrag.h"
#include "SceneGraph.h"
#include "VSceneGraph.h"

VTreeView::VTreeView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    m_Root = new TreeItem;
    m_Root->m_Text = "Scene";
    auto i1 = m_Root->AddItem("Test 1");
    auto i2 = m_Root->AddItem("Test 2");
    i2->AddItem("Other 1");
    i2->AddItem("Other 2");
    m_Root->AddItem("Test 3");
    setMouseTracking(true);
    //m_OverItem = i1;
    for (int i = 0; i < 50; i++) {
        m_Root->AddItem("YEAH!");
    }
    m_Root->AddItem("Done");

    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
   setAcceptDrops(true);
  

}

int VTreeView::CheckItem(TreeItem* item, int x, int y, int px, int py) {

    if (px >= 0 && px < width())
    {
        if (py > y && py < y + 20)
        {
            m_OverItem = item;
            return y;
        }
    }


    if (item->m_Open) {
        for (auto it : item->m_Items) {

            y = CheckItem(it, x + 20, y + 20, px, py);


        }

    }

    return y;
}

void VTreeView::mouseReleaseEvent(QMouseEvent* event) {
    if (m_LMB) {
        if (m_OverItem != nullptr)
        {
            if (m_OverItem->m_Items.size() > 0) {
                m_OverItem->m_Open = m_OverItem->m_Open ? false : true;
                update();
            }
            else {
                auto node = (Node*)m_OverItem->m_Data;
                if (node != nullptr) {
                    Editor::m_CurrentNode = (Node*)node;
                    Editor::m_PropEditor->SetNode(node);

                }
                m_ActiveItem = m_OverItem;
                update();
            }
        }
        m_LMB = false;
    }
}

void VTreeView::mousePressEvent(QMouseEvent* event)
{


    if (event->button() == Qt::LeftButton)
    {
        m_LMB = true;
       
        // Handle left mouse button press

    }
    update();
}

void VTreeView::mouseMoveEvent(QMouseEvent* event)
{

    if (m_LMB) {
        if (m_OverItem == nullptr) return;
        if (m_OverItem == m_Root) return;
        m_LMB = false;
        QMimeData* mimeData = new QMimeData;
        mimeData->setText(m_OverItem->m_Text.c_str());

        auto n = (Node*)m_OverItem->m_Data;

        auto full_url = n->GetFullName();
        mimeData->setText(full_url.c_str());

    
        int b = 5;
//        auto node = m_OverItem->m_Data;

        //mimeData->setData()
       // QStringList filePaths;
       // filePaths << m_OverItem->m_FullPath.c_str();

        QList<QUrl> urls;
       // for (const QString& filePath : filePaths) {
        //    urls.append(QUrl::fromLocalFile(filePath));
       // }
        //mimeData->setUrls(urls);


        // Create a drag object and initiate the drag operation
        QDrag* drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec(Qt::DropAction::TargetMoveAction);

        return;
    }


    m_OverItem = nullptr;
    auto pos = event->position();
   // printf("PX:%d PY:%d \n", (int)pos.x(), (int)pos.y());
    int y = CheckItem(m_Root,25,5, (int)pos.x(), (int)pos.y());
    update();
    setMinimumSize(width(), y+40);
    update();

 //   QPoint delta = event->pos() - m_MouseLast;
//    m_MouseLast = event->pos();
    // Handle mouse move event
    // Use event->pos() or event->globalPos() to get the cursor position
    //if (m_CamRotate) {
       // m_ViewYaw += delta.x() * 0.2f;
       // m_ViewPitch += delta.y() * 0.2f;


        //  update();
    //

    // Call base class implementation if necessary
    //QWidget::mouseMoveEvent(event);
}


void VTreeView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    // Create a QPainter object
    QPainter painter(this);
    QColor bg(40, 40, 40);
    // Set background color
    painter.fillRect(rect(), bg);

    RenderItem(m_Root, 25, 20);

    /*
    // Set pen and brush
    QPen pen(Qt::black);
    painter.setPen(pen);
    QBrush brush(Qt::blue);
    painter.setBrush(brush);

    // Draw shapes
    painter.drawRect(50, 50, 100, 100);       // Draw a rectangle
    painter.drawEllipse(200, 50, 100, 100);   // Draw an ellipse
    painter.drawLine(50, 200, 300, 200);      // Draw a line

    // Draw text
    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", 20));
    painter.drawText(50, 250, "Hello, Qt!");
    */
}

int VTreeView::RenderItem(TreeItem* item, int x, int y) {

    QPainter painter(this);

    if (item == m_OverItem || item == m_ActiveItem) {

        QBrush brush(Qt::gray);
        painter.setBrush(brush);

        // Set the pen (optional, for the outline of the rectangle)
        QPen pen(Qt::white);
        painter.setPen(pen);

        // Draw the filled rectangle
        QRect rect(0, y - 15, size().width()-2, 20); // x, y, width, height
        painter.drawRect(rect);

        if (item->m_Open) {

        }


    }

    if (item->m_Items.size() > 0) {
        // Set the brush for filling the rectangle
        QBrush brush(Qt::black);
        painter.setBrush(brush);

        // Set the pen (optional, for the outline of the rectangle)
        QPen pen(Qt::white);
        painter.setPen(pen);

        // Draw the filled rectangle
        QRect rect(x - 16, y - 10, 10, 10); // x, y, width, height
        painter.drawRect(rect);

        if (item->m_Open) {
            QBrush brush1(Qt::white);
            painter.setBrush(brush1);

            // Set the pen (optional, for the outline of the rectangle)
            QPen pen1(Qt::white);
            painter.setPen(pen1);

            // Draw the filled rectangle
            QRect rect1(x - 14, y - 8, 6, 6); // x, y, width, height
            painter.drawRect(rect1);
        }
    }

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    painter.drawText( x, y, item->m_Text.c_str());

    if (item->m_Open) {

        for (auto it : item->m_Items) {

            y = RenderItem(it, x + 20, y + 20);


        }

    }

    return y;

}

VTreeView::~VTreeView()
{}


void VTreeView::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
       

    }
    if (event->mimeData()->hasText()) {

        auto pn = Editor::m_Graph->FindNode(event->mimeData()->text().toStdString());
        event->acceptProposedAction();

    }
    
}

void VTreeView::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        QString filePath;
        if (!urls.isEmpty() && urls.first().isLocalFile()) {
            filePath = urls.first().toLocalFile();
            if (m_OverItem != nullptr) {
                auto node = (Node*)m_OverItem->m_Data;
                node->AddScript(filePath.toStdString());
            }

            int bb = 5;
            //setPixmap(QPixmap(filePath));
            //SetImage(filePath.toStdString());
            //emit dropped(filePath); // Emit the custom dropped sig
        }
        event->acceptProposedAction();
    }
    else if(event->mimeData()->hasText()){

        if (m_OverItem != nullptr) {
            auto pn = Editor::m_Graph->FindNode(event->mimeData()->text().toStdString());
            auto top_node = (Node*)m_OverItem->m_Data;
            if (pn == top_node) {
                return;
            }
            if (pn == Editor::m_Graph->GetRoot()) {
                return;
            }
            if (pn->IsChildOf(top_node)) {
                return;
            }
            pn->Remove();
            top_node->AddNode(pn);
            Editor::m_SceneGraph->UpdateGraph();
        }

    }
}

void VTreeView::dragMoveEvent(QDragMoveEvent* event) 
{
//    QPoint localPos = event->pos();
//    qDebug() << "Drag Move at: " << localPos;

    m_OverItem = nullptr;
    QPoint globalPos = QCursor::pos();
    QPoint localPos = mapFromGlobal(globalPos);
    auto pos = QPointF(localPos.x(), localPos.y());// event->position();
    //printf("PX:%d PY:%d \n", (int)pos.x(), (int)pos.y());
    int y = CheckItem(m_Root, 25, 5, (int)pos.x(), (int)pos.y());
    update();
    setMinimumSize(width(), y + 40);
    update();
    update();

    event->acceptProposedAction();
}

