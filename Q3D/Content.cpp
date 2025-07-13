#include "Content.h"
#include "QEngine.h"
#include "MaterialPBR.h"
#include "PropertiesEditor.h"
#include "MeshImportWindow.h"
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QImageReader>
#include <QLabel>
#include <QTimer>
#include <QGuiApplication>
#include <QScreen>
#include <QMenu>
#include <QFileDialog>
#include <QContextMenuEvent>


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
    setContextMenuPolicy(Qt::DefaultContextMenu); // Use the contextMenuEvent override
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

void Content::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu contextMenu(this);

    QAction* importMeshAction = contextMenu.addAction("Import Mesh...");
    QAction* importTextureAction = contextMenu.addAction("Import Texture...");

    connect(importMeshAction, &QAction::triggered, this, &Content::onImportMesh);
    connect(importTextureAction, &QAction::triggered, this, &Content::onImportTexture);

    contextMenu.exec(event->globalPos());
}

void Content::onImportMesh()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Import Mesh"), m_currentPath,
        tr("Mesh Files (*.fbx *.obj *.gltf);;All Files (*)"));

    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.baseName(); // Get name without extension

        // Open the import options window
        MeshImportWindow* importWindow = new MeshImportWindow(filePath, this);
        importWindow->setAttribute(Qt::WA_DeleteOnClose); // Clean up memory

        // Here you would connect the 'meshImported' signal to your actual import logic
        // connect(importWindow, &MeshImportWindow::meshImported, this, &YourClass::handleMeshImport);

        importWindow->show();
    }
}

void Content::onImportTexture()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this,
        tr("Import Textures"), m_currentPath,
        tr("Image Files (*.png *.jpg *.jpeg *.bmp *.tga);;All Files (*)"));

    if (!filePaths.isEmpty()) {
        // Process each selected texture file
        for (const QString& filePath : filePaths) {
            qDebug() << "Importing texture:" << filePath;
            // Add your texture import logic here
            // e.g., copy file to project assets, generate metadata, etc.
        }
        Browse(m_CurrentPath); // Refresh the view
    }
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
    QSize originalSize = reader.size();
    QSize targetSize(72, 72);
    if (originalSize.width() > targetSize.width() || originalSize.height() > targetSize.height()) {
        originalSize.scale(targetSize, Qt::KeepAspectRatio);
        reader.setScaledSize(originalSize);
    }
    QImage image = reader.read();
    return image.isNull() ? QPixmap() : QPixmap::fromImage(image);
}

QPixmap Content::generateLargePreview(const QString& filePath) {
    QImageReader reader(filePath);
    if (!reader.canRead()) return QPixmap();
    QSize originalSize = reader.size();
    QSize targetSize(256, 256);
    if (originalSize.width() > targetSize.width() || originalSize.height() > targetSize.height()) {
        originalSize.scale(targetSize, Qt::KeepAspectRatio);
        reader.setScaledSize(originalSize);
    }
    QImage image = reader.read();
    return image.isNull() ? QPixmap() : QPixmap::fromImage(image);
}

void Content::showImagePreview(const FileItem* item, const QPoint& mousePos) {
    if (!item || !item->hasThumbnail) return;
    hideImagePreview();
    QPixmap largePreview = generateLargePreview(item->fullPath);
    if (largePreview.isNull()) return;

    m_previewLabel = new QLabel(nullptr, Qt::ToolTip | Qt::FramelessWindowHint);
    m_previewLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_previewLabel->setPixmap(largePreview);
    m_previewLabel->setFixedSize(largePreview.size());

    QPoint showPos = mousePos + QPoint(15, 15);
    QRect screenRect = QGuiApplication::primaryScreen()->geometry();
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
        m_previewLabel = nullptr;
    }
}

QPixmap Content::loadOrGenerateThumbnail(const QString& filePath) {
    QString thumbnailPath = getThumbnailPath(filePath);
    QPixmap thumbnail;
    if (QFile::exists(thumbnailPath) && thumbnail.load(thumbnailPath)) {
        return thumbnail;
    }
    thumbnail = generateThumbnail(filePath);
    if (!thumbnail.isNull()) {
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
        m_layoutDirty = true;
        update();
        return;
    }

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
    m_layoutDirty = true;
    for (const QFileInfo& info : entries) {
        FileItem item;
        item.fullPath = info.absoluteFilePath();
        item.isDirectory = info.isDir();
        item.displayName = info.isDir() ? info.fileName() : info.baseName();
        if (!item.isDirectory) {
            item.type = FileType::FT_File;
            auto ext = GetFileExtension(info.fileName().toStdString());
            item.ext = QString(ext.c_str());
            if (item.ext.toLower() == "thumb") continue;
            if (ext == "mesh") item.type = FileType::FT_Entity;
            else if (isImageFile(item.ext)) {
                item.type = FileType::FT_Texture;
                item.thumbnail = loadOrGenerateThumbnail(item.fullPath);
                item.hasThumbnail = !item.thumbnail.isNull();
            }
            else if (ext == "py" || ext == "graph") {
                item.type = FileType::FT_Script;
            }
            if (item.displayName.isEmpty()) item.displayName = info.fileName();
        }
        m_items.push_back(item);
    }
    update();
}

void Content::GoBack()
{
    if (!m_rootPath.isEmpty() && QDir::cleanPath(m_currentPath) == QDir::cleanPath(m_rootPath)) return;
    QDir dir(m_currentPath);
    if (dir.cdUp()) {
        Browse(dir.absolutePath().toStdString());
    }
}

void Content::calculateLayout()
{
    if (!m_layoutDirty) return;

    int draw_X = 10;
    int draw_Y = 10;
    const int itemWidth = m_itemSize + 40;
    const int itemHeight = m_itemSize + 25 + 5;

    for (size_t i = 0; i < m_items.size(); ++i) {
        bool shouldDisplay = true;
        if (!m_SearchTerm.empty()) {
            if (m_SearchTerm != m_items[i].ext.toStdString()) {
                if (!containsString(m_items[i].displayName.toStdString(), m_SearchTerm)) {
                    shouldDisplay = false;
                }
            }
        }

        if (shouldDisplay) {
            m_items[i].rect = QRect(draw_X, draw_Y, itemWidth, itemHeight);
            draw_X += itemWidth;
            if (draw_X > (width() - itemWidth)) {
                draw_X = 10;
                draw_Y += 92;
            }
        }
        else {
            m_items[i].rect = QRect(); // Invalidate rect for filtered items
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

    for (const FileItem& item : m_items) {
        if (item.rect.isNull()) continue; // Skip items filtered by calculateLayout

        QRect iconRect(item.rect.x(), item.rect.y(), m_itemSize, m_itemSize);
        bool isHovered = (&item == m_OverItem);

        if (isHovered) {
            painter.setBrush(QColor(255, 255, 255, 30));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(iconRect.adjusted(-2, -2, 2, 2), 5, 5);
        }

        // Draw normal icon or thumbnail
        if (item.hasThumbnail) {
            QSize thumbnailSize(48, 48);
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
            if (item.type == FT_Entity) EntIcon.paint(&painter, iconRect);
            else if (item.type == FT_Texture) ImageIcon.paint(&painter, iconRect);
            else if (item.type == FT_Script) ScriptIcon.paint(&painter, iconRect);
            else FileIcon.paint(&painter, iconRect);
        }

        // Draw the name
        QRect textRect(item.rect.x(), item.rect.y() + m_itemSize - 12, m_itemSize, 40);
        painter.setPen(palette().text().color());
        QFontMetrics fm(font());
        QString elidedText = fm.elidedText(item.displayName + "." + item.ext, Qt::ElideRight, textRect.width());
        painter.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, elidedText);
    }
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

QSize Content::sizeHint() const { return QSize(400, m_contentHeight); }
QSize Content::minimumSizeHint() const { return QSize(200, 250); }

std::string GetFileNameWithoutExtension(const std::string& path) {
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    size_t lastSlash = normalized.find_last_of('/');
    std::string filename = (lastSlash != std::string::npos) ? normalized.substr(lastSlash + 1) : normalized;
    size_t dotPos = filename.find_last_of('.');
    return (dotPos != std::string::npos) ? filename.substr(0, dotPos) : filename;
}

void Content::fileClicked(const QString& filePath, bool isDirectory)
{
    if (isDirectory) {
        Browse(filePath.toStdString());
        m_layoutDirty = true;
        update();
    }
    else {
        auto ext = GetFileExtension(filePath.toStdString());
        if (ext == "material") {
            auto check_name = GetFileNameWithoutExtension(filePath.toStdString());
            bool found = false;
            for (auto m : Q3D::Engine::QEngine::GetActiveMaterials()) {
                if (m->GetName() == check_name) {
                    PropertiesEditor::m_Instance->SetMaterial(m);
                    found = true;
                    break;
                }
            }
            if (found) return;
            auto pbr = new MaterialPBR;
            pbr->Load(filePath.toStdString());
            auto am = Q3D::Engine::QEngine::GetActiveMaterials();
            am.push_back(pbr);
            Q3D::Engine::QEngine::SetActiveMaterials(am);
            pbr->SetName(GetFileNameWithoutExtension(filePath.toStdString()));
            PropertiesEditor::m_Instance->SetMaterial(pbr);
        }
    }
}

void Content::mouseMoveEvent(QMouseEvent* event) {
    QPoint clickPos = event->pos();
    FileItem* previousOverItem = m_OverItem;
    m_OverItem = nullptr;

    for (FileItem& item : m_items) {
        if (item.rect.contains(clickPos)) {
            m_OverItem = &item;
            break;
        }
    }

    if (m_OverItem != m_lastHoverItem) {
        m_previewTimer->stop();
        hideImagePreview();
        if (m_OverItem && m_OverItem->hasThumbnail) {
            m_lastHoverItem = m_OverItem;
            m_previewTimer->start();
        }
        else {
            m_lastHoverItem = nullptr;
        }
    }

    if (previousOverItem != m_OverItem) {
        update();
    }

    if (!(event->buttons() & Qt::LeftButton)) return;
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) return;

    if (m_OverItem) {
        hideImagePreview();
        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData;
        mimeData->setText(m_OverItem->fullPath);
        drag->setMimeData(mimeData);
        drag->exec(Qt::CopyAction);
    }
}

void Content::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::BackButton) {
        GoBack();
        return;
    }
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
        hideImagePreview();
    }
    QWidget::mousePressEvent(event);
}

void Content::leaveEvent(QEvent* event) {
    m_previewTimer->stop();
    hideImagePreview();
    if (m_OverItem) {
        m_OverItem = nullptr;
        update();
    }
    m_lastHoverItem = nullptr;
    QWidget::leaveEvent(event);
}

bool Content::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::BackButton) {
            GoBack();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}
