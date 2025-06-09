#include "Content.h"

Content* Content::m_Instance = nullptr;

Content::Content(QWidget *parent)
	: QWidget(parent)
    , m_itemSize(64)
    , m_itemSpacing(10)
    , m_layoutDirty(true)
{
    setMinimumHeight(250);
    setMouseTracking(true);
    QStyle* style = QApplication::style();
    m_Instance = this;

    DirIcon = QIcon("edit/icons/folderIcon.png");
    // }
     //else {
    FileIcon = QIcon("edit/icons/fileIcon.png");
    EntIcon = QIcon("edit/icons/entityIcon.png");
    ImageIcon = QIcon("edit/icons/imageicon.png");
    ScriptIcon = QIcon("edit/icons/scripticon.png");
	//ui.setupUi(this);
}

Content::~Content()
{}

std::string Content::GetPath() {

    return m_CurrentPath;

}

std::string GetFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');

    if (dotPos == std::string::npos || dotPos == filename.length() - 1) {
        // No dot found, or dot is the last character (no extension)
        return "";
    }

    return filename.substr(dotPos + 1);
}


bool containsString(const std::string& fileName, const std::string& search)
{
    // Handle empty search string - return true (empty string is found in any string)
    if (search.empty()) {
        return true;
    }

    // Handle empty fileName with non-empty search - return false
    if (fileName.empty()) {
        return false;
    }

    // Convert both strings to lowercase for case-insensitive comparison
    std::string lowerFileName = fileName;
    std::string lowerSearch = search;

    std::transform(lowerFileName.begin(), lowerFileName.end(), lowerFileName.begin(),
        [](unsigned char c) { return std::tolower(c); });

    std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(),
        [](unsigned char c) { return std::tolower(c); });

    // Check if search string is found within fileName
    return lowerFileName.find(lowerSearch) != std::string::npos;
}

void Content::Browse(const std::string& path)
{
    m_currentPath = QString::fromStdString(path);
    m_items.clear();
    m_CurrentPath = path;

    QDir dir(m_currentPath);
    if (!dir.exists()) {
        qDebug() << "Error: Directory does not exist:" << m_currentPath;
        m_layoutDirty = true;
        update();
        return;
    }

    // Get directory entries
    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot,
        QDir::DirsFirst | QDir::Name);

    if (entries.isEmpty() && !dir.isReadable()) {
        qDebug() << "Error: Cannot read directory:" << m_currentPath;
        m_layoutDirty = true;
        update();
        return;
    }

    // Add directories first, then files
    for (const QFileInfo& info : entries) {
        FileItem item;
        item.fullPath = info.absoluteFilePath();
        item.isDirectory = info.isDir();



        if (item.isDirectory) {
            item.displayName = info.fileName();
        }
        else {
            // Remove extension for display
            item.displayName = info.baseName();
            item.type = FileType::FT_File;

            
            auto ext = GetFileExtension(info.fileName().toStdString());
            item.ext = QString(ext.c_str());
            if (ext == "fbx" || ext == "gltf")
            {
                item.type = FileType::FT_Entity;
            }
            if (ext == "png")
            {
                item.type = FileType::FT_Texture;
            }
            if (ext == "py")
            {
                item.type = FileType::FT_Script;
            }

            if (item.displayName.isEmpty()) {
                item.displayName = info.fileName(); // fallback for files without extension
            }
        }

        m_items.push_back(item);
    }


    update();
}

void Content::calculateLayout()
{
    if (!m_layoutDirty) return;

    m_itemRects.clear();
    m_itemRects.reserve(m_items.size());

    int draw_X = 10;
    int draw_Y = 10;
    const int itemWidth = m_itemSize + 40;
    const int itemHeight = m_itemSize + 25 + 5; // icon + text + spacing

    for (size_t i = 0; i < m_items.size(); ++i) {
        // Calculate item rectangle for hit testing

        if (m_SearchTerm.size() > 0) {
            if (m_SearchTerm == m_items[i].ext) {

            }
            else {
                if (!containsString(m_items[i].displayName.toStdString(), m_SearchTerm)) {
                    continue;
                }
            }
        }

        QRect itemRect(draw_X, draw_Y, m_itemSize, itemHeight);
        m_itemRects.push_back(itemRect);

        // Store rect in item for hit testing
        m_items[i].rect = itemRect;

        draw_X += itemWidth;

        if (draw_X > (width() - (m_itemSize))) {
            draw_X = 10;
            draw_Y += 92; // Match your spacing
        }
    }

    // Calculate total content height
    m_contentHeight = draw_Y + 92 + 10; // Last row height + padding

    // Update widget size
    setFixedHeight(m_contentHeight);
    updateGeometry(); // Tell parent about size change

    m_layoutDirty = false;
}

void Content::paintEvent(QPaintEvent* event)
{


    if (m_layoutDirty) {
        calculateLayout();
    }

    QPainter painter(this);
    //  painter.setRenderHint(QPainter::Antialiasing);

      // Clear background
     // painter.fillRect(rect(), palette().base());

    if (m_items.empty()) {
        painter.setPen(palette().text().color());
        painter.drawText(rect(), Qt::AlignCenter, "No items to display");
        return;
    }

    QIcon& icon = DirIcon;

    int draw_X = 10;
    int draw_Y = 10;
    m_itemSize = 64;

    for (const FileItem& item : m_items) {

        if (m_SearchTerm.size() > 0) {
            if (m_SearchTerm == item.ext) {

            }
            else {
                if (!containsString(item.displayName.toStdString(), m_SearchTerm)) {
                    continue;
                }
            }
        }
        // Draw icon
        QRect iconRect(draw_X, draw_Y, m_itemSize, m_itemSize);

        bool over = false;

        if (&item == m_OverItem) {
            //  QRect br(draw_X + 8, draw_Y + 8, m_itemSize - 16, m_itemSize - 18);
             // painter.fillRect(br, QGradient::AquaSplash);
            over = true;
        }

        if (over) {

            QRect rr(draw_X - 4, draw_Y - 4, m_itemSize + 8, m_itemSize + 8);
            QPixmap pixmap;


            if (item.isDirectory) {
                // DirIcon.paint(&painter, iconRect);
                pixmap = DirIcon.pixmap(rr.size());
            }
            else
                if (item.type == FT_Entity)
                {
                    pixmap = EntIcon.pixmap(rr.size());
                }
                else if (item.type == FT_Texture)
                {
                    pixmap = ImageIcon.pixmap(rr.size());
                }
                else if (item.type == FT_Script)
                {
                    pixmap = ScriptIcon.pixmap(rr.size());
                }
                else {
                    pixmap = FileIcon.pixmap(rr.size());
                }
            EntIcon.pixmap(rr.size());


            QPainter iconPainter(&pixmap);
            iconPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            iconPainter.fillRect(pixmap.rect(), Qt::white);  // Tint color
            iconPainter.end();

            painter.drawPixmap(rr.topLeft(), pixmap);

        }

        if (item.isDirectory) {
            DirIcon.paint(&painter, iconRect);
        }
        else {
            if (item.type == FT_Entity)
            {
                EntIcon.paint(&painter, iconRect);
            }
            else if (item.type == FT_Texture)
            {
                ImageIcon.paint(&painter, iconRect);
            }
            else if (item.type == FT_Script) {
                ScriptIcon.paint(&painter, iconRect);
            }
            else {
                FileIcon.paint(&painter, iconRect);
            }
        }



        // Draw text below icon
        QRect textRect(draw_X + 15, draw_Y + m_itemSize - 12,
            m_itemSize * 3, 40);

        painter.setPen(palette().text().color());
        QFontMetrics fm(font());
        QString elidedText = fm.elidedText(item.displayName, Qt::ElideRight, m_itemSize);
        painter.drawText(textRect, Qt::AlignLeft, elidedText);
        draw_X += m_itemSize + 40;

        if (draw_X > (width() - (m_itemSize)))
        {
            draw_X = 10;
            draw_Y += 92;
        }
    }
    update();

}

void Content::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPoint clickPos = event->pos();

    for (const FileItem& item : m_items) {
        if (item.rect.contains(clickPos)) {
            fileClicked(item.fullPath, item.isDirectory);
            return;
        }
    }

    QWidget::mouseDoubleClickEvent(event);
}

void Content::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    // Mark layout as dirty when width changes
    if (event->oldSize().width() != event->size().width()) {
        m_layoutDirty = true;
    }
}
QSize Content::sizeHint() const
{
    return QSize(400, m_contentHeight); // Return actual content height
}
QSize Content::minimumSizeHint() const
{
    return QSize(200, 250);
}


void Content::fileClicked(const QString& filePath, bool isDirectory)
{
    if (isDirectory) {
        qDebug() << "Directory double-clicked:" << filePath;
        Browse(filePath.toStdString());
        m_layoutDirty = true;
        update();
        // You can implement directory navigation here
        // For example: Browse(filePath.toStdString());
    }
    else {
        qDebug() << "File double-clicked:" << filePath;
        // You can implement file opening logic here
    }
}

void Content::mouseMoveEvent(QMouseEvent* event) {

    QPoint clickPos = event->pos();
    m_OverItem = nullptr;
    auto p = m_OverItem;

    // Find the item under the cursor for hover effects
    for (FileItem& item : m_items) {
        if (item.rect.contains(clickPos)) {
            m_OverItem = &item;
            break;
        }
    }

    // If the mouse has moved, trigger an update for the hover effect
    if (p != nullptr) {
        update();
    }

    // --- Drag and Drop Logic ---
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    // Find the item to drag (the one that was under the cursor on mouse press)
    FileItem* draggedItem = nullptr;
    for (FileItem& item : m_items) {
        if (item.rect.contains(m_dragStartPosition)) {
            draggedItem = &item;
            break;
        }
    }

    if (draggedItem) {
        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData;

        mimeData->setText(draggedItem->fullPath); // Set the filename with extension as data
        drag->setMimeData(mimeData);

        // 1. Get the base icon pixmap. The final drag image will have the same dimensions.
        QIcon& icon = draggedItem->isDirectory ? DirIcon : (draggedItem->type == FT_Entity ? EntIcon : (draggedItem->type == FT_Texture ? ImageIcon : (draggedItem->type == FT_Script ? ScriptIcon : FileIcon)));
        QPixmap iconPixmap = icon.pixmap(QSize(m_itemSize, m_itemSize));
        QString fileName = QFileInfo(draggedItem->fullPath).fileName();

        // 2. Create the new pixmap and a painter to draw on it
        QPixmap dragPixmap(iconPixmap.size());
        dragPixmap.fill(Qt::transparent);
        QPainter painter(&dragPixmap);

        // 3. Draw the main icon first
        painter.drawPixmap(0, 0, iconPixmap);

        // 4. Define a rectangle at the bottom of the icon for the text overlay
        QFontMetrics fontMetrics(this->font());
        QRect textBoundingRect = fontMetrics.boundingRect(fileName);
        int padding = 4;
        int bannerHeight = textBoundingRect.height() + (padding * 2);
        QRect textBannerRect(0, dragPixmap.height() - bannerHeight, dragPixmap.width(), bannerHeight);

        // 5. Draw a semi-transparent background banner for the text
        painter.setBrush(QColor(0, 0, 0, 140)); // Black, semi-transparent
        painter.setPen(Qt::NoPen);
        painter.drawRect(textBannerRect);

        // 6. Draw the filename, centered within the banner
        painter.setPen(Qt::white); // White text for good contrast
        painter.drawText(textBannerRect, Qt::AlignCenter, fileName);
        painter.end();

        // 7. Set the newly created pixmap as the drag image
        drag->setPixmap(dragPixmap);

        // 8. Adjust the "hot spot" to be the center of the cursor
        drag->setHotSpot(QPoint(dragPixmap.width() / 2, dragPixmap.height() / 2));
        drag->exec(Qt::CopyAction);
    }

}

void Content::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    }
    QWidget::mousePressEvent(event);
}