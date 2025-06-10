#pragma once

#include <QWidget>
#include "ui_PropertiesEditor.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include "PropertyVec3.h"
#include "PropertyText.h"
#include "PropertyFloat.h"
#include "PropertySlider.h"
#include "PropertyInt.h"
#include "PropertyNode.h"
#include <functional>
#include <qvector3d.h>
#include "PropertyStringList.h"
#include "PropertyTexture.h"
// Forward declaration of GraphNode is good practice
class GraphNode;

class MaterialPBR;

class PropertiesEditor : public QWidget
{
    Q_OBJECT

public:
    PropertiesEditor(QWidget* parent = nullptr);
    ~PropertiesEditor();
    QSize sizeHint() const override;

    static PropertiesEditor* m_Instance;
    // Add to a public section of PropertiesEditor in PropertiesEditor.h
    void AddedFromDrop(const QString& resourceName);
    void BeginUI();
    void EndUI();
    void ClearUI();
    void UpdateNode(GraphNode* node);
    void SetNode(GraphNode* node);
    void SetMaterial(MaterialPBR* mat);
    void AddHeader(const QString& text);

    PropertyText* AddText(const QString& label, const QString& defaultText = "",
        std::function<void(const QString&)> callback = nullptr);

    // Callback now takes a GraphNode pointer
    PropertyNode* AddNodeProperty(const QString& label, const QString& defaultText = "",
        std::function<void(GraphNode*)> callback = nullptr);

    PropertyVec3* AddVec3(const QString& label, const QVector3D& defaultValue = QVector3D(0, 0, 0), double interval = 1.0f,
        std::function<void(const QVector3D&)> callback = nullptr);

    PropertyFloat* AddFloat(const QString& label, double minValue, double maxValue, double interval,
        double defaultValue = 0.0, std::function<void(double)> callback = nullptr);

    PropertyInt* AddInt(const QString& label, int minValue, int maxValue, int interval,
        int defaultValue = 0.0, std::function<void(int)> callback = nullptr);


    PropertySlider* AddSlider(const QString& label, int minValue, int maxValue, int defaultValue = 0,
        std::function<void(int)> callback = nullptr);
    PropertyStringList* AddStringList(const QString& label, const QStringList& options,
        const QString& defaultValue = "", std::function<void(const QString&)> callback = nullptr);
    PropertyTexture* AddTexture(const QString& label, const std::string& defaultPath = "",
        std::function<void(const std::string&)> callback = nullptr);

protected:

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    Ui::PropertiesEditorClass ui;
    QVBoxLayout* m_mainLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    QVBoxLayout* m_contentLayout;

    // Property widgets
    PropertyVec3* m_positionProp;
    PropertyVec3* m_scaleProp;
    PropertyVec3* m_rotationProp;
    PropertyText* m_Name;
    PropertyText* m_Tag;
    PropertyText* m_currentNameProp = nullptr;
    PropertyVec3* m_currentPositionProp = nullptr;
    PropertyVec3* m_currentRotationProp = nullptr;
    PropertyVec3* m_currentScaleProp = nullptr;
    GraphNode* m_Node = nullptr;
    MaterialPBR* m_Material;

};
