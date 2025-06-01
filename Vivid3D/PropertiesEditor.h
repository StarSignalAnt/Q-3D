#pragma once

#include <QWidget>
#include "ui_PropertiesEditor.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include "PropertyVec3.h"
#include "PropertyText.h"
#include <functional>
#include <qvector3d.h>
class GraphNode;


class PropertiesEditor : public QWidget
{
	Q_OBJECT

public:
	PropertiesEditor(QWidget *parent = nullptr);
	~PropertiesEditor();
	QSize sizeHint() const override;

    static PropertiesEditor* m_Instance;

    void BeginUI();
    void EndUI();

    void SetNode(GraphNode* node);
    void AddHeader(const QString& text);

    void AddText(const QString& label, const QString& defaultText = "",
        std::function<void(const QString&)> callback = nullptr);

    void AddVec3(const QString& label, const QVector3D& defaultValue = QVector3D(0, 0, 0),
        std::function<void(const QVector3D&)> callback = nullptr);

    void AddFloat(const QString& label, double minValue, double maxValue, double interval,
        double defaultValue = 0.0, std::function<void(double)> callback = nullptr);

    void AddSlider(const QString& label, int minValue, int maxValue, int defaultValue = 0,
        std::function<void(int)> callback = nullptr);

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
};

