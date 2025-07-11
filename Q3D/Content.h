#pragma once

#include <QWidget>
#include <QDir>
#include <QFileInfo>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QApplication>
#include <QStyle>
#include <QFontMetrics>
#include <QDrag>
#include <QMimeData>
#include <QPixmap>
#include <QImageReader>
#include <QStandardPaths>
#include <QCryptographicHash>

#include <QTimer>
#include <QLabel>

#include <QDebug>
#include <string>
#include <vector>
#include "ui_Content.h"
enum FileType {
    FT_Entity, FT_Texture, FT_File, FT_Script
};

struct FileItem {
    QString fullPath;
    QString displayName;
    bool isDirectory;
    QRect rect;
    QString ext;
    FileType type = FT_File;
    QPixmap thumbnail; // Store the thumbnail pixmap
    bool hasThumbnail = false;
};

class Content : public QWidget
{
    Q_OBJECT

public:
    Content(QWidget* parent = nullptr);
    ~Content();
    void Browse(const std::string& path);
    static Content* m_Instance;
    std::string GetPath();
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void SetSearch(std::string term) {
        m_SearchTerm = term;
        m_layoutDirty = true;
        update();

    }
    void GoBack();
protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;


private slots:
    void onImportMesh();
    void onImportTexture();

private:
    Ui::ContentClass ui;
    void calculateLayout();
    void fileClicked(const QString& filePath, bool isDirectory);
    bool isImageFile(const QString& extension);
    QString getThumbnailPath(const QString& filePath);
    QPixmap generateThumbnail(const QString& filePath);
    QPixmap loadOrGenerateThumbnail(const QString& filePath);
    void showImagePreview(const FileItem* item, const QPoint& mousePos);
    void hideImagePreview();
    QPixmap generateLargePreview(const QString& filePath);
    QString m_rootPath;
    std::vector<FileItem> m_items;
    int m_itemSize;
    int m_itemSpacing;
    QString m_currentPath;
    bool m_layoutDirty;
    QIcon DirIcon, FileIcon;
    std::string m_CurrentPath;
    QIcon EntIcon;
    QIcon ImageIcon;
    QIcon ScriptIcon;
    std::vector<QRect> m_itemRects; // Store item rectangles for hit testing
    int m_contentHeight; // Track actual content height
    FileItem* m_OverItem = nullptr;
    std::string m_SearchTerm = "";
    QPoint m_dragStartPosition;
    QString m_thumbnailCacheDir; // Directory to store thumbnail cache

    // Image preview popup
    QLabel* m_previewLabel;
    QTimer* m_previewTimer;
    FileItem* m_lastHoverItem;
};
