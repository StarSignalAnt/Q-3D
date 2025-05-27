#pragma once

#include <QWidget>
#include "ui_VContentView.h"
#include <vector>
#include <stack>

class ContentItem {
public:
	std::string m_Name = "";
	std::string m_FullPath = "";
	std::string m_Ext;
	int m_DrawX, m_DrawY;
	bool m_Folder = false;
};

class VContentView : public QWidget
{
	Q_OBJECT

public:
	VContentView(QWidget *parent = nullptr);
	~VContentView();
	void Browse(std::string path);
	void UpdateView();
	static VContentView* m_This;


protected:
	void mousePressEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void contextMenuEvent(QContextMenuEvent* event) override;
	void CreateScript();

private:
	Ui::VContentViewClass ui;
	ContentItem* m_OverItem = nullptr;
	std::vector<ContentItem*> m_Files;
	std::vector<ContentItem*> m_Folders;
	std::vector<ContentItem*> m_All;
	std::stack<std::string> m_Paths;
	QMenu* contextMenu;

};
