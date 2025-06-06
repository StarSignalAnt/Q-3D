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
#include <QDebug>
#include <string>
#include <vector>
#include "ui_Content.h"
enum FileType {
    FT_Entity, FT_Texture, FT_File
};

struct FileItem {
    QString fullPath;
    QString displayName;
    bool isDirectory;
    QRect rect;
    QString ext;
    FileType type = FT_File;
};

class Content : public QWidget
{
	Q_OBJECT

public:
	Content(QWidget *parent = nullptr);
	~Content();
	void Browse(const std::string& path);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void SetSearch(std::string term) {
        m_SearchTerm = term;
        m_layoutDirty = true;
        update();

    }
protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
private:
	Ui::ContentClass ui;
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
    FileItem* m_OverItem = nullptr;
    std::string m_SearchTerm = "";
};

