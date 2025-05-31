#pragma once

#include <QWidget>
#include "ui_PropertiesEditor.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include "PropertyVec3.h"
#include "PropertyText.h"
class PropertiesEditor : public QWidget
{
	Q_OBJECT

public:
	PropertiesEditor(QWidget *parent = nullptr);
	~PropertiesEditor();
	QSize sizeHint() const override;

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

