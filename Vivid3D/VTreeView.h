#pragma once

#include <QWidget>
#include "ui_VTreeView.h"
#include <string>
#include <vector>
#include <QResizeEvent>
#include <QDebug>
#include <qmimedata.h>
#include <QDragEnterEvent>

class TreeItem {
public:
	std::string m_Text;
	std::vector<TreeItem*> m_Items;
	bool m_Open = true;
	void* m_Data = nullptr;
	TreeItem* AddItem(std::string text) {
		TreeItem* item = new TreeItem;
		item->m_Text = text;
		m_Items.push_back(item);
		return item;
	}

};

class VTreeView : public QWidget
{
	Q_OBJECT

public:
	VTreeView(QWidget *parent = nullptr);
	~VTreeView();

	int RenderItem(TreeItem* item, int x, int y);
	int CheckItem(TreeItem* item, int x, int y, int px, int py);
	void SetRoot(TreeItem* item) {
		m_Root = item;
		m_Root->m_Text = "Root";
	}
	void SetActive(TreeItem* item) {
		m_ActiveItem = item;
	}

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
//protected:

	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;

private:
	Ui::VTreeViewClass ui;
	TreeItem* m_Root;
	TreeItem* m_OverItem = nullptr;
	TreeItem* m_ActiveItem = nullptr;
	bool m_LMB = false;
};
