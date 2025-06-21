#include "NewProjectDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QFileDialog>
#include <QDialogButtonBox>
NewProjectDialog::NewProjectDialog(QWidget *parent)
	: QDialog(parent)
{
    setWindowTitle("Create New Project");

    auto formLayout = new QFormLayout();
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("My Awesome Game");
    formLayout->addRow("Project Name:", m_nameEdit);

    auto pathLayout = new QHBoxLayout();
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setPlaceholderText("Select an empty folder...");
    auto browseButton = new QPushButton("Browse...", this);
    pathLayout->addWidget(m_pathEdit);
    pathLayout->addWidget(browseButton);
    formLayout->addRow("Project Path:", pathLayout);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    connect(browseButton, &QPushButton::clicked, this, &NewProjectDialog::onBrowse);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

NewProjectDialog::~NewProjectDialog()
{}


QString NewProjectDialog::getProjectName() const {
    return m_nameEdit->text();
}

QString NewProjectDialog::getProjectPath() const {
    return m_pathEdit->text();
}

void NewProjectDialog::onBrowse() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Project Folder", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        m_pathEdit->setText(dir);
    }
}