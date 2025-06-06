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

class Content;

class ContentBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit ContentBrowser(QWidget* parent = nullptr);
    void Browse(std::string path);
    void SetSearch(std::string term);
    static ContentBrowser* m_Instance;
protected:

private:
 
    Content* m_Content;

};

#endif // CONTENTBROWSER_H