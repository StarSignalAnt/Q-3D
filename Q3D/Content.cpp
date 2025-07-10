#include "Content.h"
#include "QEngine.h"
#include "MaterialPBR.h"
#include "PropertiesEditor.h"
Content* Content::m_Instance = nullptr;

Content::Content(QWidget* parent)
    : QWidget(parent)
    , m_itemSize(64)
    , m_itemSpacing(10)
    , m_layoutDirty(true)
    , m_previewLabel(nullptr)
    , m_previewTimer(nullptr)
    , m_lastHoverItem(nullptr)
{
    setMinimumHeight(250);
    setMouseTracking(true);
    QStyle* style = QApplication::style();
    m_Instance = this;

    // Setup thumbnail cache directory
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    m_thumbnailCacheDir = appDataPath + "/thumbnails";
    QDir().mkpath(m_thumbnailCacheDir);

    // Setup preview timer
    m_previewTimer = new QTimer(this);
    m_previewTimer->setSingleShot(true);
    m_previewTimer->setInterval(500); // 500ms delay before showing preview
    connect(m_previewTimer, &QTimer::timeout, this, [this]() {
        if (m_lastHoverItem && m_lastHoverItem->hasThumbnail) {
            showImagePreview(m_lastHoverItem, QCursor::pos());
        }
        });

    DirIcon = QIcon("edit/icons/folderIcon.png");
    FileIcon = QIcon("edit/icons/fileIcon.png");
    EntIcon = QIcon("edit/icons/entityIcon.png");
    ImageIcon = QIcon("edit/icons/imageicon.png");
    ScriptIcon = QIcon("edit/icons/scripticon.png");
    if (parent) {
        parent->installEventFilter(this);
    }
}

Content::~Content()
{
    hideImagePreview();
}

std::string Content::GetPath() {
    return m_CurrentPath;
}

std::string GetFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');

    if (dotPos == std::string::npos || dotPos == filename.length() - 1) {
        return "";
    }

    return filename.substr(dotPos + 1);
}

bool containsString(const std::string& fileName, const std::string& search)
{
    if (search.empty()) {
        return true;
    }

    if (fileName.empty()) {
        return false;
    }

    std::string lowerFileName = fileName;
    std::string lowerSearch = search;

    std::transform(lowerFileName.begin(), lowerFileName.end(), lowerFileName.begin(),
        [](unsigned char c) { return std::tolower(c); });

    std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(),
        [](unsigned char c) { return std::tolower(c); });

    return lowerFileName.find(lowerSearch) != std::string::npos;
}

bool Content::isImageFile(const QString& extension) {
    static QStringList imageExtensions = { "png", "jpg", "jpeg", "bmp", "gif", "tiff", "tga" };
    return imageExtensions.contains(extension.toLower());
}

QString Content::getThumbnailPath(const QString& filePath) {
    // Create unique filename using hash of original file path
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(filePath.toUtf8());
    QString hashedName = QString(hash.result().toHex());

    return m_thumbnailCacheDir + "/" + hashedName + ".thumb";
}

QPixmap Content::generateThumbnail(const QString& filePath) {
    QImageReader reader(filePath);
    if (!reader.canRead()) {
        return QPixmap();
    }

    // Scale image to fit within 72x72 while maintaining aspect ratio
    QSize originalSize = reader.size();
    QSize targetSize(72, 72);

    if (originalSize.width() > targetSize.width() || originalSize.height() > targetSize.height()) {
        originalSize.scale(targetSize, Qt::KeepAspectRatio);
        reader.setScaledSize(originalSize);
    }

    QImage image = reader.read();
    if (image.isNull()) {
        return QPixmap();
    }

    return QPixmap::fromImage(image);
}

QPixmap Content::generateLargePreview(const QString& filePath) {
    QImageReader reader(filePath);
    if (!reader.canRead()) {
        return QPixmap();
    }

    // Scale image to fit within 256x256 while maintaining aspect ratio
    QSize originalSize = reader.size();
    QSize targetSize(256, 256);

    if (originalSize.width() > targetSize.width() || originalSize.height() > targetSize.height()) {
        originalSize.scale(targetSize, Qt::KeepAspectRatio);
        reader.setScaledSize(originalSize);
    }

    QImage image = reader.read();
    if (image.isNull()) {
        return QPixmap();
    }

    return QPixmap::fromImage(image);
}

void Content::showImagePreview(const FileItem* item, const QPoint& mousePos) {
    if (!item || !item->hasThumbnail) return;

    hideImagePreview(); // Clean up any existing preview

    // Generate large preview
    QPixmap largePreview = generateLargePreview(item->fullPath);
    if (largePreview.isNull()) return;

    // Create preview label
    m_previewLabel = new QLabel(nullptr);
    m_previewLabel->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    m_previewLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_previewLabel->setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 200); border: 2px solid white; border-radius: 5px; padding: 5px; }");
    m_previewLabel->setPixmap(largePreview);
    m_previewLabel->setFixedSize(largePreview.size() + QSize(10, 10)); // Add padding

    // Position near mouse cursor but keep on screen
    QPoint showPos = mousePos + QPoint(15, 15);
    QRect screenRect = QGuiApplication::primaryScreen()->geometry();

    // Adjust position to keep preview on screen
    if (showPos.x() + m_previewLabel->width() > screenRect.right()) {
        showPos.setX(mousePos.x() - m_previewLabel->width() - 15);
    }
    if (showPos.y() + m_previewLabel->height() > screenRect.bottom()) {
        showPos.setY(mousePos.y() - m_previewLabel->height() - 15);
    }

    m_previewLabel->move(showPos);
    m_previewLabel->show();
}

void Content::hideImagePreview() {
    if (m_previewLabel) {
        m_previewLabel->deleteLater();
        m_previewLabel->close();
        m_previewLabel = nullptr;
      
    }
}

QPixmap Content::loadOrGenerateThumbnail(const QString& filePath) {
    QString thumbnailPath = getThumbnailPath(filePath);

    // Try to load existing thumbnail
    QPixmap thumbnail;
    if (QFile::exists(thumbnailPath)) {
        if (thumbnail.load(thumbnailPath)) {
            return thumbnail;
        }
    }

    // Generate new thumbnail
    thumbnail = generateThumbnail(filePath);
    if (!thumbnail.isNull()) {
        // Save thumbnail to cache
        thumbnail.save(thumbnailPath, "PNG");
    }

    return thumbnail;
}

void Content::Browse(const std::string& path)
{
    m_currentPath = QString::fromStdString(path);
    m_items.clear();
    m_CurrentPath = path;
    if (m_rootPath.isEmpty()) {
        m_rootPath = m_currentPath;
    }
    QDir dir(m_currentPath);
    if (!dir.exists()) {
        qDebug() << "Error: Directory does not exist:" << m_currentPath;
        m_layoutDirty = true;
        update();
        return;
    }

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot,
        QDir::DirsFirst | QDir::Name);

    if (entries.isEmpty() && !dir.isReadable()) {
        qDebug() << "Error: Cannot read directory:" << m_currentPath;
        m_layoutDirty = true;
        update();
        return;
    }
    m_layoutDirty = true;
    for (const QFileInfo& info : entries) {
        FileItem item;
        item.fullPath = info.absoluteFilePath();
        item.isDirectory = info.isDir();

        if (item.isDirectory) {
            item.displayName = info.fileName();
        }
        else {
            item.displayName = info.baseName();
            item.type = FileType::FT_File;

            auto ext = GetFileExtension(info.fileName().toStdString());
            item.ext = QString(ext.c_str());

            // Skip thumbnail cache files
            if (item.ext.toLower() == "thumb") {
                continue;
            }

            if (ext == "fbx" || ext == "gltf") {
                item.type = FileType::FT_Entity;
            }
            else if (isImageFile(item.ext)) {
                item.type = FileType::FT_Texture;
                // Load or generate thumbnail for image files
                item.thumbnail = loadOrGenerateThumbnail(item.fullPath);
                item.hasThumbnail = !item.thumbnail.isNull();
            }
            else if (ext == "py" || ext == "graph") {
                item.type = FileType::FT_Script;
            }

            if (item.displayName.isEmpty()) {
                item.displayName = info.fileName();
            }
        }

        m_items.push_back(item);
    }

    update();
}

void Content::GoBack()
{
    // Use QDir::cleanPath to normalize paths (e.g., remove trailing slashes) for a reliable comparison.
    if (!m_rootPath.isEmpty() && QDir::cleanPath(m_currentPath) == QDir::cleanPath(m_rootPath))
    {
        qDebug() << "Already at root content path. Cannot go back further.";
        return; // We are at the root, do nothing.
    }

    QDir dir(m_currentPath);

    // cdUp() navigates to the parent directory.
    if (dir.cdUp()) {
        // Browse to the new parent path.
        Browse(dir.absolutePath().toStdString());
    }
    else {
        qDebug() << "Failed to navigate up from current path:" << m_currentPath;
    }
}

void Content::calculateLayout()
{
    if (!m_layoutDirty) return;

    m_itemRects.clear();
    m_itemRects.reserve(m_items.size());

    int draw_X = 10;
    int draw_Y = 10;
    const int itemWidth = m_itemSize + 40;
    const int itemHeight = m_itemSize + 25 + 5;

    for (size_t i = 0; i < m_items.size(); ++i) {
        if (m_SearchTerm.size() > 0) {
            if (m_SearchTerm == m_items[i].ext) {
                // Match by extension
            }
            else {
                if (!containsString(m_items[i].displayName.toStdString(), m_SearchTerm)) {
                    continue;
                }
            }
        }

        QRect itemRect(draw_X, draw_Y, m_itemSize, itemHeight);
        m_itemRects.push_back(itemRect);
        m_items[i].rect = itemRect;

        draw_X += itemWidth;

        if (draw_X > (width() - (m_itemSize))) {
            draw_X = 10;
            draw_Y += 92;
        }
    }

    m_contentHeight = draw_Y + 92 + 10;
    setFixedHeight(m_contentHeight);
    updateGeometry();
    m_layoutDirty = false;
}

void Content::paintEvent(QPaintEvent* event)
{
    if (m_layoutDirty) {
        calculateLayout();
    }

    QPainter painter(this);

    if (m_items.empty()) {
        painter.setPen(palette().text().color());
        painter.drawText(rect(), Qt::AlignCenter, "No items to display");
        return;
    }

    int draw_X = 10;
    int draw_Y = 10;
    m_itemSize = 64;

    for (const FileItem& item : m_items) {
        if (m_SearchTerm.size() > 0) {
            if (m_SearchTerm == item.ext) {
                // Match by extension
            }
            else {
                if (!containsString(item.displayName.toStdString(), m_SearchTerm)) {
                    continue;
                }
            }
        }

        QRect iconRect(draw_X, draw_Y, m_itemSize, m_itemSize);
        bool over = (&item == m_OverItem);

        if (over) {
            QPixmap pixmap;
            QRect rr;

            if (item.hasThumbnail) {
                // For thumbnails, make hover size just a bit bigger than the rendered thumbnail (48x48)
                QSize hoverSize(56, 56); // Just 8 pixels bigger than 48x48
                rr = QRect(draw_X + (m_itemSize - hoverSize.width()) / 2,
                    draw_Y + (m_itemSize - hoverSize.height()) / 2,
                    hoverSize.width(), hoverSize.height());
                pixmap = item.thumbnail.scaled(hoverSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            else {
                // For regular icons, use the existing larger hover size
                rr = QRect(draw_X - 4, draw_Y - 4, m_itemSize + 8, m_itemSize + 8);
                if (item.isDirectory) {
                    pixmap = DirIcon.pixmap(rr.size());
                }
                else if (item.type == FT_Entity) {
                    pixmap = EntIcon.pixmap(rr.size());
                }
                else if (item.type == FT_Texture) {
                    pixmap = ImageIcon.pixmap(rr.size());
                }
                else if (item.type == FT_Script) {
                    pixmap = ScriptIcon.pixmap(rr.size());
                }
                else {
                    pixmap = FileIcon.pixmap(rr.size());
                }
            }

            QPainter iconPainter(&pixmap);
            iconPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            iconPainter.fillRect(pixmap.rect(), Qt::white);
            iconPainter.end();

            painter.drawPixmap(rr.topLeft(), pixmap);
        }

        // Draw normal icon or thumbnail
        if (item.hasThumbnail) {
            // Draw thumbnail scaled to smaller size to not obscure text
            QSize thumbnailSize(48, 48); // Smaller than the 64x64 icon size
            QPixmap scaledThumbnail = item.thumbnail.scaled(thumbnailSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            QRect centeredRect = iconRect;
            centeredRect.setSize(scaledThumbnail.size());
            centeredRect.moveCenter(iconRect.center());
            painter.drawPixmap(centeredRect, scaledThumbnail);
        }
        else if (item.isDirectory) {
            DirIcon.paint(&painter, iconRect);
        }
        else {
            if (item.type == FT_Entity) {
                EntIcon.paint(&painter, iconRect);
            }
            else if (item.type == FT_Texture) {
                ImageIcon.paint(&painter, iconRect);
            }
            else if (item.type == FT_Script) {
                ScriptIcon.paint(&painter, iconRect);
            }
            else {
                FileIcon.paint(&painter, iconRect);
            }
        }

        QRect textRect(draw_X, draw_Y + m_itemSize - 12, m_itemSize, 40);
        painter.setPen(palette().text().color());
        QFontMetrics fm(font());
        QString elidedText = fm.elidedText(item.displayName + "." + item.ext, Qt::ElideRight, textRect.width());
        painter.drawText(textRect, Qt::AlignCenter, elidedText);

        draw_X += m_itemSize + 40;

        if (draw_X > (width() - (m_itemSize))) {
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

    if (event->oldSize().width() != event->size().width()) {
        m_layoutDirty = true;
    }
}

QSize Content::sizeHint() const
{
    return QSize(400, m_contentHeight);
}

QSize Content::minimumSizeHint() const
{
    return QSize(200, 250);
}

std::string GetFileNameWithoutExtension(const std::string& path) {
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    size_t lastSlash = normalized.find_last_of('/');
    std::string filename = (lastSlash != std::string::npos)
        ? normalized.substr(lastSlash + 1)
        : normalized;

    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filename.substr(0, dotPos);
    }

    return filename;
}

void Content::fileClicked(const QString& filePath, bool isDirectory)
{
    if (isDirectory) {
        qDebug() << "Directory double-clicked:" << filePath;
        Browse(filePath.toStdString());
        m_layoutDirty = true;
        update();
    }
    else {
        qDebug() << "File double-clicked:" << filePath;

        auto ext = GetFileExtension(filePath.toStdString());

        if (ext == "material") {
            auto check_name = GetFileNameWithoutExtension(filePath.toStdString());

            bool found = false;
            for (auto m : QEngine::GetActiveMaterials()) {
                if (m->GetName() == check_name) {
                    PropertiesEditor::m_Instance->SetMaterial(m);
                    found = true;
                    break;
                }
            }
            if (found) {
                return;
            }

            auto pbr = new MaterialPBR;
            pbr->Load(filePath.toStdString());
            auto am = QEngine::GetActiveMaterials();
            am.push_back(pbr);
            QEngine::SetActiveMaterials(am);
            pbr->SetName(GetFileNameWithoutExtension(filePath.toStdString()));
            PropertiesEditor::m_Instance->SetMaterial(pbr);
        }
    }
}

void Content::mouseMoveEvent(QMouseEvent* event) {
    QPoint clickPos = event->pos();
    FileItem* previousOverItem = m_OverItem;
    m_OverItem = nullptr;

    // Find the item under the cursor for hover effects
    for (FileItem& item : m_items) {
        if (item.rect.contains(clickPos)) {
            m_OverItem = &item;
            break;
        }
    }

    // Handle image preview with improved logic
    if (m_OverItem != m_lastHoverItem) {
        // Mouse moved to different item or off items
        m_previewTimer->stop();
        hideImagePreview();

        if (m_OverItem && m_OverItem->hasThumbnail) {
            // Start timer for new item with thumbnail
            m_lastHoverItem = m_OverItem;
            m_previewTimer->start();
        }
        else {
            // Mouse moved off item or to item without thumbnail
            m_lastHoverItem = nullptr;
        }
    }

    // If the mouse has moved, trigger an update for the hover effect
    if (previousOverItem != m_OverItem) {
        update();
    }

    // --- Drag and Drop Logic ---
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    FileItem* draggedItem = nullptr;
    for (FileItem& item : m_items) {
        if (item.rect.contains(m_dragStartPosition)) {
            draggedItem = &item;
            break;
        }
    }

    if (draggedItem) {
        // Hide preview during drag
        hideImagePreview();

        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData;

        mimeData->setText(draggedItem->fullPath);
        drag->setMimeData(mimeData);

        QPixmap iconPixmap;
        if (draggedItem->hasThumbnail) {
            iconPixmap = draggedItem->thumbnail.scaled(QSize(m_itemSize, m_itemSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else {
            QIcon& icon = draggedItem->isDirectory ? DirIcon :
                (draggedItem->type == FT_Entity ? EntIcon :
                    (draggedItem->type == FT_Texture ? ImageIcon :
                        (draggedItem->type == FT_Script ? ScriptIcon : FileIcon)));
            iconPixmap = icon.pixmap(QSize(m_itemSize, m_itemSize));
        }

        QString fileName = QFileInfo(draggedItem->fullPath).fileName();

        QPixmap dragPixmap(iconPixmap.size());
        dragPixmap.fill(Qt::transparent);
        QPainter painter(&dragPixmap);

        painter.drawPixmap(0, 0, iconPixmap);

        QFontMetrics fontMetrics(this->font());
        QRect textBoundingRect = fontMetrics.boundingRect(fileName);
        int padding = 4;
        int bannerHeight = textBoundingRect.height() + (padding * 2);
        QRect textBannerRect(0, dragPixmap.width() - bannerHeight, dragPixmap.width(), bannerHeight);

        painter.setBrush(QColor(0, 0, 0, 140));
        painter.setPen(Qt::NoPen);
        painter.drawRect(textBannerRect);

        painter.setPen(Qt::white);
        painter.drawText(textBannerRect, Qt::AlignCenter, fileName);
        painter.end();

        drag->setPixmap(dragPixmap);
        drag->setHotSpot(QPoint(dragPixmap.width() / 2, dragPixmap.height() / 2));
        drag->exec(Qt::CopyAction);
    }
}
void Content::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::BackButton)
    {
        GoBack();
        return; // The event is handled, no need for further processing.
    }
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
        // Hide preview when clicking
        hideImagePreview();
    }
    QWidget::mousePressEvent(event);
}

void Content::leaveEvent(QEvent* event) {
    // Immediately hide preview and stop timer when mouse leaves the widget
    m_previewTimer->stop();
    hideImagePreview();

    // Reset hover state
    if (m_OverItem) {
        m_OverItem = nullptr;
        update(); // Refresh to remove hover effect
    }

    m_lastHoverItem = nullptr;

    QWidget::leaveEvent(event);
}

bool Content::eventFilter(QObject* watched, QEvent* event)
{
    // Check if the event is a mouse button press.
    if (event->type() == QEvent::MouseButtonPress) {
        // Cast the generic event to a mouse event to access button info.
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::BackButton) {
            GoBack();
            return true; // Return true to indicate we have handled the event.
            // It will not be passed on to the original target.
        }
    }

    // For all other events, pass them on to the default processing.
    return QWidget::eventFilter(watched, event);
}
