#pragma once

#include <QWidget>
#include <qlineedit.h>
#include "ui_VScriptClass.h"
#include <qmimedata.h>
#include <QDragEnterEvent>

class VClass;
class VVar;

class VScriptClass : public QLineEdit
{
	Q_OBJECT

public:
	VScriptClass(QWidget *parent = nullptr);
	~VScriptClass();
	void SetType(std::string type) {
		m_ClassType = type;
	}
	VClass* GetClass() {
		return m_Class;
	}
	void SetEntry(VVar* entry) {
		m_Entry = entry;
	}
protected:
	//protected:

	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;

private:
	Ui::VScriptClassClass ui;
	std::string m_ClassType;
	VClass* m_Class;
	VVar* m_Entry;

};
