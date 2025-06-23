#pragma once

#include <QWidget>
#include <QTextEdit>
#include "ui_ConsoleOutput.h"

class ConsoleOutput : public QTextEdit
{
	Q_OBJECT

public:
	ConsoleOutput(QWidget *parent = nullptr);
	~ConsoleOutput();
	void appendConsoleText(const QString& text, const QColor& color = Qt::black);
	void appendConsoleLine(const QString& text, const QColor& color = Qt::black);
	static ConsoleOutput* m_Instance;

private:
	Ui::ConsoleOutputClass ui;
};

