#ifndef CONTENTBROWSER_H
#define CONTENTBROWSER_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QApplication>
#include <QStyle>
#include <QFontMetrics>
#include <QDebug>
#include <string>
#include <vector>

enum FileType {
    FT_Entity,FT_Texture,FT_File
};

struct FileItem {
    QString fullPath;
    QString displayName;
    bool isDirectory;
    QRect rect;
    FileType type = FT_File;
};

class ContentBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit ContentBrowser(QWidget* parent = nullptr);
    void Browse(const std::string& path);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void calculateLayout();
    void fileClicked(const QString& filePath, bool isDirectory);

    std::vector<FileItem> m_items;
    int m_itemSize;
    int m_itemSpacing;
    QString m_currentPath;
    bool m_layoutDirty;
    QIcon DirIcon, FileIcon;
    QIcon EntIcon;
    QIcon ImageIcon;
    std::vector<QRect> m_itemRects; // Store item rectangles for hit testing
    int m_contentHeight; // Track actual content height


};

#endif // CONTENTBROWSER_H