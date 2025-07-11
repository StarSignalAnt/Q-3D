#ifndef COMPONENTBROWSER_H
#define COMPONENTBROWSER_H

#include <QWidget>
#include <string>
#include <vector>
#include "ComponentContent.h"

class ComponentContent;

class ComponentBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit ComponentBrowser(QWidget* parent = nullptr);
    void SetComponents(const std::vector<ComponentInfo>& components);
    void SetSearch(std::string term);
    static ComponentBrowser* m_Instance;

public slots:
    void GoBack();

private:
    ComponentContent* m_ComponentContent;
};

#endif // COMPONENTBROWSER_H
