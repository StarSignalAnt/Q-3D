#pragma once

#include <QWidget>
#include "ui_VCodeComplete.h"
#include "qlistwidget.h"
#include "qevent.h"

class VCodeComplete : public QWidget
{
	Q_OBJECT

public:
	VCodeComplete(QWidget *parent = nullptr);
	~VCodeComplete();
	void AddItem(std::string item);
	void SetWord(std::string word)
	{
		m_Search = word;
	}
	std::string GetSelection()
	{
		return m_List->currentItem()->text().toStdString();
	}
	std::string GetSearch() {
		return m_Search;
	}
	int GetCount() {
		return m_List->count();
	}
	bool HasWord(std::string ch) {

		for (int i = 0; i < m_List->count(); ++i)
		{
			QListWidgetItem* item = m_List->item(i);
			// Do something with the item
			QString text = item->text();
			if (ch == text.toStdString()) {
				return true;
			}
			// Process the item as needed
		}
		return false;
	}
protected:

	void keyPressEvent(QKeyEvent* event) override {
			
		if (m_List->count() == 0)
		{
			close();
			return;
		}
		if (event->key() == Qt::Key_Up)
		{
			m_Selected--;
			if (m_Selected < 0) m_Selected = 0;
		}
		if (event->key() == Qt::Key_Down) {
			m_Selected++;
			if (m_Selected >= m_List->count()) {
				m_Selected = m_List->count() - 1;
			}
		}

		m_List->setCurrentRow(m_Selected);

	}

private:
	int m_Selected = 0;
	Ui::VCodeCompleteClass ui;
	QListWidget* m_List;
	std::string m_Search;
};
