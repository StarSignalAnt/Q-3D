#pragma once

#include <QWidget>
#include "ui_PropertyText.h"
#include <QLabel>
#include <QLineEdit>

class PropertyText : public QWidget
{
	Q_OBJECT

public:
	PropertyText(const QString& label, const QString& defaultText = "", QWidget* parent = nullptr);
	~PropertyText();
	void setText(const QString& text);
	QString getText() const;

signals:
	void textChanged(const QString& text);

private slots:
	void onTextChanged();

private:
	void setupLineEdit();
	QLabel* m_label;
	QLineEdit* m_lineEdit;

	Ui::PropertyTextClass ui;
};

