#include "VContentView.h"
#include <QPainter>
#include <QDir>
#include <qevent.h>
#include "Editor.h"
#include "engine.h"
#include "Importer.h"
#include "SceneGraph.h"
#include <QDirIterator>
#include "VSceneGraph.h"
#include "MaterialMeshLight.h"
#include "VPropertyEditor.h"
#include <qmimedata.h>
#include <qdrag.h>
#include "VImportModel.h"
#include "VFile.h"
#include "ImportSettings.h"
#include "VCreateScript.h"
#include "VScriptEdit.h"
#include <cstdio>  // Include the header for standard I/O functions
#include <iostream>  // Include the header for standard input/output stream objects

VContentView::VContentView(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setMouseTracking(true);

    contextMenu = new QMenu(this);

    auto create = contextMenu->addMenu("Create"); // new  QAction(tr("Action 1"), this);
    auto cr_script = create->addAction("Create Script");
    auto del = contextMenu->addAction("Delete");




    connect(cr_script, &QAction::triggered, this, [this]()
    {
            CreateScript();
    });

    connect(del, &QAction::triggered, this, [this]() {

        if (m_OverItem) {

            std::remove(m_OverItem->m_FullPath.c_str());
            Browse(m_Paths.top());
            update();
        }

        });

    m_This = this;

}

VContentView* VContentView::m_This = nullptr;

VContentView::~VContentView()
{}

void VContentView::CreateScript() {

    VCreateScript* cr_script = new VCreateScript();
    cr_script->show();
    cr_script->m_Path = m_Paths.top();

}

void VContentView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    // Create a QPainter object
    QPainter painter(this);
    QColor bg(40, 40, 40);
    // Set background color
    painter.fillRect(rect(),bg);

    QImage folder("edit/icons/folder.png");
    QImage file("edit/icons/file.png");

    for (auto item : m_All) {

        QRectF rect(item->m_DrawX, item->m_DrawY, 64, 64);

        if (item == m_OverItem) {

            QColor bg(70, 70, 70);
            // Set background color
            QRectF rect2(item->m_DrawX-8,item->m_DrawY-8,64+16,64+16);

            painter.fillRect(rect2, bg);
           // painter.drawRect(rect);


        }

        if (item->m_Folder) {
            painter.drawImage(rect, folder);
        }
        else {
            painter.drawImage(rect, file);
        }

        painter.drawText(item->m_DrawX, item->m_DrawY + 70, 120, 30, 0, item->m_Name.c_str());

    };
    
  //  RenderItem(m_Root, 25, 20);

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

//


void VContentView::mousePressEvent(QMouseEvent* event)
{

    if (event->button() == Qt::BackButton) {
        if (m_Paths.size() > 1)
        {
            m_Paths.pop();
            
            m_Files.clear();
            m_Folders.clear();
            m_All.clear();
            std::string top = m_Paths.top();
            m_Paths.pop();
           // printf("DIR:>>");
            //printf(top.c_str());
            //printf("\n");
            Browse(top);
            update();
        }
        else {
            m_Files.clear();
            m_Folders.clear();
            m_All.clear();
            std::string top = m_Paths.top();
            m_Paths.pop();
            Browse(top);
            update();
            
        }
      
    }
    if (event->button() == Qt::LeftButton) {

        if (m_OverItem != nullptr) {
            // Create a MIME data object to hold the drag data
            QMimeData* mimeData = new QMimeData;
            mimeData->setText(m_OverItem->m_Name.c_str());
            //mimeData->setData()
            QStringList filePaths;
            filePaths << m_OverItem->m_FullPath.c_str();
            QList<QUrl> urls;
            for (const QString& filePath : filePaths) {
                urls.append(QUrl::fromLocalFile(filePath));
            }
            mimeData->setUrls(urls);

            // Create a drag object and initiate the drag operation
            QDrag* drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->exec(Qt::DropAction::TargetMoveAction);
        }
    }
    /*
    if (event->button() == Qt::LeftButton)
    {
        if (m_OverItem != nullptr)
        {
            if (m_OverItem->m_Items.size() > 0) {
                m_OverItem->m_Open = m_OverItem->m_Open ? false : true;
                update();
            }
        }
        // Handle left mouse button press

    }
    */
}

void VContentView::mouseMoveEvent(QMouseEvent* event)
{
    auto pos = event->position();
    m_OverItem = nullptr;
    update();

    for (auto item : m_All) {
    
        if (pos.x() > item->m_DrawX-16 && pos.y() > item->m_DrawY-8) {
            if (pos.x() < item->m_DrawX + 64+16 && pos.y() < item->m_DrawY + 64+16)
            {
                m_OverItem = item;
                update();
                return;
            }
        }

    }
    /*
    m_OverItem = nullptr;
    auto pos = event->position();
    printf("PX:%d PY:%d \n", (int)pos.x(), (int)pos.y());
    int y = CheckItem(m_Root, 25, 5, (int)pos.x(), (int)pos.y());
    update();
    setMinimumSize(width(), y + 40);
    update();
    */
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

QString extractFileName(const QString& filePath)
{
    // Find the last occurrence of the path separator '/'
    int lastIndex = filePath.lastIndexOf('/');
    if (lastIndex == -1) {
        // If no separator found, return the original file path
        return filePath;
    }

    // Extract the file name (including extension) from the file path
    return filePath.mid(lastIndex + 1);
}

QString extractFileExtension(const QString& fileName)
{
    int dotIndex = fileName.lastIndexOf('.');
    if (dotIndex != -1) {
        return fileName.mid(dotIndex + 1);
    }
    return QString(); // Return an empty string if no extension found
}

void VContentView::Browse(std::string path) {

    m_Files.clear();
    m_Folders.clear();
    m_All.clear();

    m_Paths.push(path);

    QDir directory(path.c_str());

    // Check if the directory exists
    if (!directory.exists()) {
        qDebug() << "Directory does not exist.";
        return;
    }

    // Create a QDirIterator to iterate through the directory
    QFileInfoList fileInfoList = directory.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    // Iterate through the entries
    for (const QFileInfo& fileInfo : fileInfoList) {
        if (fileInfo.isFile()) {
            //qDebug() << "File:" << fileInfo.absoluteFilePath();
            ContentItem* item = new ContentItem;
            item->m_Name = extractFileName(fileInfo.absoluteFilePath()).toStdString();
            item->m_FullPath = fileInfo.absoluteFilePath().toStdString();
            item->m_Folder = false;
            item->m_Ext = extractFileExtension(fileInfo.absoluteFilePath()).toStdString();
            m_Files.push_back(item);
        }
        else if (fileInfo.isDir()) {
            qDebug() << "Directory:" << fileInfo.absoluteFilePath();
            ContentItem* item = new ContentItem;
            item->m_Name = extractFileName(fileInfo.absoluteFilePath()).toStdString();
            item->m_FullPath = fileInfo.absoluteFilePath().toStdString();
            item->m_Folder = true;
            m_Folders.push_back(item);
        }
    }

    UpdateView();
    update();
}

void VContentView::UpdateView() {

    auto p = (QWidget*)this->parent();
    int max_w = p->width();

    int dx, dy;

    m_All.clear();

    dx = 15;
    dy = 15;

    for (auto folder : m_Folders) {

        folder->m_DrawX = dx;
        folder->m_DrawY = dy;
        dx = dx + 128;
        if (dx >= max_w-64)
        {
            dx = 5;
            dy = dy + 128;
        }
        m_All.push_back(folder);


    }

    for (auto file : m_Files) {

        file->m_DrawX = dx;
        file->m_DrawY = dy;

        dx = dx + 128;
        if (dx >= max_w-64)
        {
            dx = 5;
            dy = dy + 128;
        }
        m_All.push_back(file);
    }

   // setMaximumSize(max_w, dy);
    setMinimumHeight(dy+96);
    int b = 5;

}

void VContentView::resizeEvent(QResizeEvent* event)
{

    UpdateView();
    update();
    /// Call the base class implementation (optional, but good practice)
    /*
    QWidget::resizeEvent(event);

    // Handle the resize event
    QSize newSize = event->size();
    QSize oldSize = event->oldSize();
    //  if (pSelf->m_pSwapChain != nullptr)
    m_pSwapChain->Resize(static_cast<Uint32>(newSize.width()), static_cast<Uint32>(newSize.height()));
    // For demonstration, print the new size to the debug output
    qDebug() << "Widget resized from" << oldSize << "to" << newSize;
    Engine::m_FrameWidth = newSize.width();
    Engine::m_FrameHeight = newSize.height();
    */
}

void VContentView::mouseDoubleClickEvent(QMouseEvent* event) 
{
    // Call the base class implementation of mouseDoubleClickEvent
    QWidget::mouseDoubleClickEvent(event);


    if (m_OverItem != nullptr) {
        // Check if the event occurred in the left mouse button
        if (m_OverItem->m_Folder) {
            if (event->button() == Qt::LeftButton) {
                if (m_OverItem != nullptr) {
                    m_Files.clear();
                    m_Folders.clear();
                    m_All.clear();

                    Browse(m_OverItem->m_FullPath);
                    update();

                }
                //qDebug() << "Double-click event occurred at (" << event->x() << ", " << event->y() << ")";
            }
        }
        else {
            if (event->button() == Qt::LeftButton) {
                if (m_OverItem != nullptr) {
                    if (m_OverItem->m_Ext == "v")
                    {


                        if (Editor::m_ScriptEdit == nullptr) {
                            VScriptEdit* editor = new VScriptEdit;

                            editor->show();
                            editor->m_This->LoadScript(m_OverItem->m_FullPath);
                        }
                        else {
                            Editor::m_ScriptEdit->LoadScript(m_OverItem->m_FullPath);
                        }
                    }
                    if (m_OverItem->m_Ext == "fbx")
                    {

                        auto imp_path = m_OverItem->m_FullPath + ".import";

                        if (VFile::Exists(imp_path.c_str())) {

                            auto imp = new ImportSettings;
                            imp->Load(m_OverItem->m_FullPath + ".import");
                            auto node = imp->GetNode();
                           
                            Editor::m_Graph->AddNode(node);
                         Editor::m_SceneGraph->UpdateGraph();
                         node->SetRTEnable();
                         Editor::m_Graph->Updated();
                            

                        }
                        else {

                            auto imp_model = new VImportModel;

                          
                            imp_model->Set(m_OverItem->m_FullPath);
                            imp_model->show();

                        }

                        //Importer* imp = new Importer;
                        //auto node = imp->ImportNode(m_OverItem->m_FullPath);
                        //Editor::m_Graph->AddNode(node);
                       // Editor::m_SceneGraph->UpdateGraph();
                       // node->SetRTEnable();
                        //Editor::m_Graph->Updated();




                        //exit(1);

                    }
                    if (m_OverItem->m_Ext == "material")
                    {
                         auto old_mat = Engine::FindActiveMaterial(m_OverItem->m_FullPath);

                        if (old_mat != nullptr) {

                            Editor::m_PropEditor->SetMaterial(old_mat);
                            return;
                        }

                        MaterialBase* mat;// = new MaterialMeshLight;
                        mat = MaterialBase::LoadMaterial(m_OverItem->m_FullPath);
                        int b = 5;
                        Editor::m_PropEditor->SetMaterial(mat);
                    }

                }
            }
        }
    }
}

void VContentView::contextMenuEvent(QContextMenuEvent* event)
{
    contextMenu->exec(event->globalPos());
}
