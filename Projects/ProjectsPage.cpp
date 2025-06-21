#include "ProjectsPage.h"
#include "NewProjectDialog.h"

ProjectsPage::ProjectsPage(QWidget *parent)
	: QWidget(parent)
{
    // New top-level layout that arranges widgets vertically
    auto topLevelLayout = new QVBoxLayout(this);
    m_headerLabel = new QLabel("Vivid3D - Projects", this);
    QFont headerFont = m_headerLabel->font();
    headerFont.setPointSize(18);
    headerFont.setBold(true);
    m_headerLabel->setFont(headerFont);
    m_headerLabel->setAlignment(Qt::AlignCenter);
    topLevelLayout->addWidget(m_headerLabel);

    QFrame* separatorLine = new QFrame(this);
    separatorLine->setFrameShape(QFrame::HLine);
    separatorLine->setFrameShadow(QFrame::Sunken);
    topLevelLayout->addWidget(separatorLine);

    auto mainContentLayout = new QHBoxLayout();
    auto listLayout = new QVBoxLayout();
    projectList = new QListWidget(this);
    listLayout->addWidget(projectList);

    auto buttonLayout = new QHBoxLayout();
    newButton = new QPushButton("New");
    loadButton = new QPushButton("Load");
    deleteButton = new QPushButton("Delete");
    deleteAllButton = new QPushButton("Delete All");
    buttonLayout->addWidget(newButton);
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(deleteAllButton);
    listLayout->addLayout(buttonLayout);
    mainContentLayout->addLayout(listLayout, 1);

    auto rightLayout = new QVBoxLayout();
    auto imageButtonLayout = new QHBoxLayout();
    imageLabel = new QLabel("No image set", this);
    imageLabel->setFixedSize(200, 200);
    imageLabel->setStyleSheet("border: 1px solid gray;");
    imageLabel->setAlignment(Qt::AlignCenter);
    setImageButton = new QPushButton("Set Image", this);
    imageButtonLayout->addWidget(imageLabel);
    imageButtonLayout->addWidget(setImageButton);
    imageButtonLayout->addStretch(1);
    rightLayout->addLayout(imageButtonLayout);

    descriptionEdit = new QTextEdit(this);
    descriptionEdit->setPlaceholderText("Project description...");
    rightLayout->addWidget(descriptionEdit, 1);
    mainContentLayout->addLayout(rightLayout, 2);
    topLevelLayout->addLayout(mainContentLayout);

    // --- Connect signals & slots ---
    connect(newButton, &QPushButton::clicked, this, &ProjectsPage::onNewProject);
    connect(loadButton, &QPushButton::clicked, this, &ProjectsPage::onLoadProject);
    connect(deleteButton, &QPushButton::clicked, this, &ProjectsPage::onDeleteProject);
    connect(setImageButton, &QPushButton::clicked, this, &ProjectsPage::onSetImage);
    connect(deleteAllButton, &QPushButton::clicked, this, &ProjectsPage::onDeleteAllProjects);
    connect(projectList, &QListWidget::currentItemChanged, this, &ProjectsPage::updateUIForSelectedProject);
    connect(descriptionEdit, &QTextEdit::textChanged, this, &ProjectsPage::onDescriptionChanged);

    // --- Initialization ---
    m_projectsListPath = QCoreApplication::applicationDirPath() + "/projects.list";
    m_currentIndexPath = QCoreApplication::applicationDirPath() + "/current.index"; // <-- ADD THIS LINE
    m_newProjectTemplatePath = "C:/Vivid3D/FakeInstall/NewProject/"; // <-- ADD THIS LINE
    m_ideExecutablePath = "C:\\Vivid3D\\Vivid3D\\x64\\Debug\\Vivid3D.exe";
    loadProjectsList();

    QFile indexFile(m_currentIndexPath);
    if (indexFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&indexFile);
        bool ok;
        int savedIndex = in.readAll().toInt(&ok);
        // Check if the saved index is valid for the current list size
        if (ok && savedIndex >= 0 && savedIndex < projectList->count()) {
            projectList->setCurrentRow(savedIndex);
        }
    }

}

ProjectsPage::~ProjectsPage()
{}


void ProjectsPage::onDeleteAllProjects()
{
    if (m_projects.isEmpty()) {
        QMessageBox::information(this, "No Projects", "There are no projects to delete.");
        return;
    }

    // Show a strong confirmation dialog before proceeding
    auto reply = QMessageBox::question(this, "Confirm Delete All",
        "<b>Are you sure you want to delete ALL projects?</b>"
        "\n\nThis action is irreversible and will permanently delete all associated project folders and files from your disk.",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No); // Default focus to 'No'

    if (reply == QMessageBox::No) {
        return;
    }

    // Iterate through all managed projects
    for (VProject* proj : m_projects.values()) {
        if (!proj) continue;

        QString projPath = QString::fromStdString(proj->GetPath());
        QString projName = QString::fromStdString(proj->GetName());

        // Delete the .project file and the entire project directory from the disk
        QFile::remove(projPath + "/" + projName + ".project");
        QDir(projPath).removeRecursively();

        // Delete the VProject object from memory
        delete proj;
    }

    // Clear all internal data structures
    m_projects.clear();

    // Clear the UI list. This automatically triggers a UI update.
    projectList->clear();

    // Update the persistent projects.list file, which will now be empty
    saveProjectsList();
}
void ProjectsPage::loadProjectsList()
{
    QFile file(m_projectsListPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return; // File doesn't exist yet, which is fine on first run
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString projectFilePath = in.readLine();
        if (projectFilePath.isEmpty() || !QFile::exists(projectFilePath)) continue;

        VProject* project = new VProject();
        project->Load(projectFilePath.toStdString());

        QString name = QString::fromStdString(project->GetName());
        m_projects.insert(name, project);
        projectList->addItem(name);
    }
}

void ProjectsPage::saveProjectsList()
{
    QFile file(m_projectsListPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not save project list.");
        return;
    }

    QTextStream out(&file);
    for (VProject* proj : m_projects.values()) {
        QString projPath = QString::fromStdString(proj->GetPath());
        QString projName = QString::fromStdString(proj->GetName());
        out << projPath + "/" + projName + ".project" << "\n";
    }
}

void ProjectsPage::onNewProject()
{
    NewProjectDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString name = dialog.getProjectName().trimmed();
    QString path = dialog.getProjectPath();

    // --- Validation (mostly unchanged) ---
    if (name.isEmpty() || path.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Project name and path cannot be empty.");
        return;
    }
    if (m_projects.contains(name)) {
        QMessageBox::warning(this, "Invalid Input", "A project with this name already exists.");
        return;
    }
    QDir dir(path);
    if (!dir.exists() || !dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty()) {
        QMessageBox::warning(this, "Invalid Path", "The selected path must be an existing, empty folder.");
        return;
    }

    // --- ADD THIS BLOCK to copy template files ---
    QDir templateDir(m_newProjectTemplatePath);
    if (!templateDir.exists()) {
        QMessageBox::critical(this, "Configuration Error", "Project template source directory not found:\n" + m_newProjectTemplatePath);
        return;
    }

    if (!copyDirectoryRecursively(m_newProjectTemplatePath, path)) {
        QMessageBox::critical(this, "File Error", "Failed to copy template files to the new project directory.");
        // Consider cleaning up the partially created directory here if necessary
        return;
    }
    // --- END of new block ---

    // --- Create and Save Project (unchanged) ---
    VProject* project = new VProject();
    project->SetName(name.toStdString());
    project->SetPath(path.toStdString());

    QString projectFilePath = path + "/" + name + ".project";
    project->Save(projectFilePath.toStdString());

    // --- Update UI and Data Model (unchanged) ---
    m_projects.insert(name, project);
    projectList->addItem(name);
    projectList->setCurrentRow(projectList->count() - 1);
    saveProjectsList();
}

bool ProjectsPage::copyDirectoryRecursively(const QString& sourcePath, const QString& destPath)
{
    QDir sourceDir(sourcePath);
    if (!sourceDir.exists()) {
        return false;
    }

    QDir destDir(destPath);
    if (!destDir.exists()) {
        destDir.mkpath(".");
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& fileInfo : fileInfoList) {
        QString newDestPath = destPath + QDir::separator() + fileInfo.fileName();

        if (fileInfo.isDir()) {
            if (!copyDirectoryRecursively(fileInfo.filePath(), newDestPath)) {
                return false;
            }
        }
        else {
            if (!QFile::copy(fileInfo.filePath(), newDestPath)) {
                return false;
            }
        }
    }
    return true;
}

void ProjectsPage::onDeleteProject()
{
    QListWidgetItem* item = projectList->currentItem();
    if (!item) return;

    QString name = item->text();
    VProject* proj = m_projects.value(name, nullptr);
    if (!proj) return;

    auto reply = QMessageBox::question(this, "Confirm Delete",
        "Are you sure you want to permanently delete the project '" + name + "'?\nThis will delete the .project file and its containing folder.",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) return;

    // Delete from data model
    m_projects.remove(name);
    delete proj;

    // Delete from UI
    delete projectList->takeItem(projectList->row(item));

    // Delete files and save the updated list
    QString projPath = QString::fromStdString(proj->GetPath());
    QFile::remove(projPath + "/" + name + ".project");
    QDir(projPath).removeRecursively();

    saveProjectsList();
}

void ProjectsPage::onSetImage()
{
    QListWidgetItem* item = projectList->currentItem();
    if (!item) return;

    VProject* proj = m_projects.value(item->text(), nullptr);
    if (!proj) return;

    QString fileName = QFileDialog::getOpenFileName(this, "Select Project Image", "", "Images (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        proj->SetImage(fileName.toStdString());
        // Save the project file to persist the new image path
        QString projPath = QString::fromStdString(proj->GetPath());
        QString projName = QString::fromStdString(proj->GetName());
        proj->Save(projPath.toStdString() + "/" + projName.toStdString() + ".project");

        updateUIForSelectedProject(item);
    }
}

void ProjectsPage::onDescriptionChanged()
{
    QListWidgetItem* item = projectList->currentItem();
    if (!item) return;
    VProject* proj = m_projects.value(item->text(), nullptr);
    if (!proj) return;

    // To prevent saving on every keystroke, you might add a "Save" button
    // or a timer. For simplicity, we save on change here.
    proj->SetDescription(descriptionEdit->toPlainText().toStdString());

    QString projPath = QString::fromStdString(proj->GetPath());
    QString projName = QString::fromStdString(proj->GetName());
    proj->Save(projPath.toStdString() + "/" + projName.toStdString() + ".project");
}

void ProjectsPage::updateUIForSelectedProject(QListWidgetItem* item)
{
    int currentIndex = -1;
    if (item) {
        currentIndex = projectList->row(item);
    }

    QFile indexFile(m_currentIndexPath);
    if (indexFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&indexFile);
        out << currentIndex;
    }
    if (!item) {
        descriptionEdit->clear();
        imageLabel->clear();
        imageLabel->setText("No Project Selected");
        descriptionEdit->setEnabled(false);
        setImageButton->setEnabled(false);
        return;
    }

    VProject* proj = m_projects.value(item->text(), nullptr);
    if (!proj) return;

    // Temporarily disconnect the signal to prevent feedback loops
    disconnect(descriptionEdit, &QTextEdit::textChanged, this, &ProjectsPage::onDescriptionChanged);
    descriptionEdit->setText(QString::fromStdString(proj->GetDescription()));
    connect(descriptionEdit, &QTextEdit::textChanged, this, &ProjectsPage::onDescriptionChanged);

    imageLabel->setPixmap(convertPixelMapToPixmap(proj->GetImage()));

    descriptionEdit->setEnabled(true);
    setImageButton->setEnabled(true);
}

QPixmap ProjectsPage::convertPixelMapToPixmap(PixelMap* map)
{
    if (!map || !map->GetData()) {
        return QPixmap(); // Return an empty pixmap
    }

    int width = map->GetWidth();
    int height = map->GetHeight();
    float* floatData = map->GetData();

    QImage image(width, height, QImage::Format_RGBA8888);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 4;
            float r = floatData[index + 0];
            float g = floatData[index + 1];
            float b = floatData[index + 2];
            float a = floatData[index + 3];
            image.setPixel(x, y, qRgba(r * 255, g * 255, b * 255, a * 255));
        }
    }

    return QPixmap::fromImage(image).scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void ProjectsPage::onLoadProject()
{
    QListWidgetItem* item = projectList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Project Selected", "Please select a project to open.");
        return;
    }

    VProject* proj = m_projects.value(item->text(), nullptr);
    if (!proj) return;

    // 1. Check if the IDE path has been set and if the executable exists
    if (m_ideExecutablePath.isEmpty() || !QFile::exists(m_ideExecutablePath)) {
        QMessageBox::critical(this, "IDE Not Found",
            "The path to the Vivid3D IDE is not set or is invalid.\n"
            "Please configure it.\n\nPath: " + m_ideExecutablePath);
        return;
    }

    // 2. Prepare the arguments and working directory
    QStringList arguments;
    QString projectPath = QString::fromStdString(proj->GetPath());

    // The command-line argument is still the project path
    arguments << projectPath;

    // The working directory will also be the project path
    QString workingDirectory = "C:\\Vivid3D\\Vivid3D\\";

    // 3. Launch the IDE process with the specified working directory
    // --- THIS LINE IS CHANGED ---
    bool success = QProcess::startDetached(m_ideExecutablePath, arguments, workingDirectory);
    // --- END OF CHANGE ---

    if (!success) {
        QMessageBox::critical(this, "Error", "Failed to launch the Vivid3D IDE.");
    }
}