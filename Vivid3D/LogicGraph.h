#pragma once

#include <QWidget>
#include "ui_LogicGraph.h"
#include <qsplitter.h>

class LGProperties;
class LGDesigner;

class LogicGraph : public QWidget
{
	Q_OBJECT

public:
	LogicGraph(QWidget *parent = nullptr);
	~LogicGraph();
protected:


private:
	Ui::LogicGraphClass ui;
	QSplitter* m_Splitter;
	LGProperties* m_Properties;
	LGDesigner* m_Designer;
};

