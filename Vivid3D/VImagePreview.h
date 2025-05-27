#pragma once

#include <QWidget>
#include "ui_VImagePreview.h"
#include <qlabel.h>
#include <qmimedata.h>
#include <QDragEnterEvent>

class VImagePreview : public QLabel
{
	Q_OBJECT

public:
	VImagePreview(QWidget *parent = nullptr);
	~VImagePreview();
	void SetImage(std::string path);
	void SetSize(int w, int h);
signals:
	void dropped(const QString& filePath);


protected:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;

private:
	Ui::VImagePreviewClass ui;
	int m_PrevW, m_PrevH;
};
