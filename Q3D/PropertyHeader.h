#pragma once

#include <QWidget>
#include "ui_PropertyHeader.h"
#include <QLabel>
class PropertyHeader : public QWidget
{
	Q_OBJECT

public:
	PropertyHeader(const QString& text, QWidget* parent = nullptr);
	~PropertyHeader();

protected:
	void paintEvent(QPaintEvent* event) override;

	QString m_text;

};

