#pragma once

#include <QWidget>
#include "ui_NewProjectDialog.h"
#include <QDialog>
#include <QLineEdit>
class NewProjectDialog : public QDialog
{
	Q_OBJECT

public:
	NewProjectDialog(QWidget *parent = nullptr);
	~NewProjectDialog();

	QString getProjectName() const;
	QString getProjectPath() const;

private slots:
	void onBrowse();
private:
	QLineEdit* m_nameEdit;
	QLineEdit* m_pathEdit;
};

