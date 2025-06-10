#include "Content.h"
#include "Vivid.h"
#include "MaterialPBR.h"
#include "PropertiesEditor.h"
Content* Content::m_Instance = nullptr;

Content::Content(QWidget* parent)
    : QWidget(parent)
    , m_itemSize(64)
    , m_itemSpacing(10)
    , m_layoutDirty(true)
{
    setMinimumHeight(250);
    setMouseTracking(true);
    QStyle* style = QApplication::style();
    m_Instance = this;

    // Setup thumbnail cache directory
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    m_thumbnailCacheDir = appDataPath + "/thumbnails";
    QDir().mkpath(m_thumbnailCacheDir);

    DirIcon = QIcon("edit/icons/folderIcon.png");
    FileIcon = QIcon("edit/icons/fileIcon.png");
    EntIcon = QIcon("edit/icons/entityIcon.png");
    ImageIcon = QIcon("edit/icons/imageicon.png");
    ScriptIcon = QIcon("edit/icons/scripticon.png");
}

Content::~Content()
{
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
            else if (ext == "py") {
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
            for (auto m : Vivid::m_ActiveMaterials) {
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
            Vivid::m_ActiveMaterials.push_back(pbr);
            pbr->SetName(GetFileNameWithoutExtension(filePath.toStdString()));
            PropertiesEditor::m_Instance->SetMaterial(pbr);
        }
    }
}

void Content::mouseMoveEvent(QMouseEvent* event) {
    QPoint clickPos = event->pos();
    m_OverItem = nullptr;
    auto p = m_OverItem;

    for (FileItem& item : m_items) {
        if (item.rect.contains(clickPos)) {
            m_OverItem = &item;
            break;
        }
    }

    if (p != nullptr) {
        update();
    }

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
        QRect textBannerRect(0, dragPixmap.height() - bannerHeight, dragPixmap.width(), bannerHeight);

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
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    }
    QWidget::mousePressEvent(event);
}