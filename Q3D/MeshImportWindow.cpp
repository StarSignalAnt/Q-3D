#include "MeshImportWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include "Importer.h"
#include "StaticMeshComponent.h"
#include "VFile.h"

MeshImportWindow::MeshImportWindow(const QString& originalName, QWidget* parent)
    : QWidget(parent, Qt::Window) // Ensure it's a top-level window
{
    setWindowTitle("Import Mesh Options");
    setMinimumWidth(350);

    // --- Create Widgets ---
    m_nameLineEdit = new QLineEdit(originalName, this);

    m_scaleSpinBox = new QDoubleSpinBox(this);
    m_scaleSpinBox->setRange(0.01, 1000.0);
    m_scaleSpinBox->setSingleStep(0.1);
    m_scaleSpinBox->setValue(1.0);
    m_scaleSpinBox->setDecimals(3);

    m_skeletalCheckBox = new QCheckBox("Skeletal Mesh", this);
    m_skeletalCheckBox->setToolTip("Check if the mesh has a skeleton and should be imported for animation.");

    m_importButton = new QPushButton("Import", this);
    m_cancelButton = new QPushButton("Cancel", this);

    // Set a default button for convenience (e.g., pressing Enter)
    m_importButton->setDefault(true);

    // --- Layout ---
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    formLayout->addRow(new QLabel("Import Name:"), m_nameLineEdit);
    formLayout->addRow(new QLabel("Global Scale:"), m_scaleSpinBox);
    formLayout->addRow(new QLabel("Mesh Type:"), m_skeletalCheckBox);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(); // Pushes buttons to the right
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_importButton);

    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    // --- Connections ---
    connect(m_importButton, &QPushButton::clicked, this, &MeshImportWindow::onImportClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &MeshImportWindow::onCancelClicked);
    path = originalName.toStdString();
}

MeshImportWindow::~MeshImportWindow()
{
    qDebug() << "MeshImportWindow destroyed.";
}

std::vector<SubMesh*> GetSubMeshes(StaticMeshComponent* comp, std::vector<SubMesh*> submeshes) {

    for (auto sm : comp->GetSubMeshes()) {
        submeshes.push_back(sm);
    }

    return submeshes;

}

std::vector<StaticMeshComponent*> GetSMeshes(GraphNode* node, std::vector<StaticMeshComponent*> meshes)
{

    for (auto c : node->GetAllComponents()) {

        if (dynamic_cast<StaticMeshComponent*>(c)) {
            meshes.push_back((StaticMeshComponent*)c);
        }

    }

    for (auto n : node->GetNodes()) {

        meshes = GetSMeshes(n, meshes);

    }

    return meshes;
}

std::string remove_extension(const std::string& file_path) {
    // Create a path object from the string
    std::filesystem::path p(file_path);

    // If the path has no extension or is a directory, return it as is.
    // The check for p.stem() == "." handles cases like "/path/to/."
    if (!p.has_extension() || p.stem() == ".") {
        return file_path;
    }

    // Get the parent directory path
    std::filesystem::path parent_path = p.parent_path();

    // Get the filename without the extension
    std::filesystem::path stem = p.stem();

    // If the original path was just a filename, return only the stem
    if (parent_path.empty()) {
        return stem.string();
    }

    // Rejoin the parent path and the stem
    return (parent_path / stem).string();
}

void MeshImportWindow::onImportClicked()
{
    // Retrieve the values from the UI elements
    QString name = m_nameLineEdit->text();
    double scale = m_scaleSpinBox->value();
    bool isSkeletal = m_skeletalCheckBox->isChecked();

    // You can add validation here, e.g., check if the name is empty
    if (name.isEmpty()) {
        // Handle error, maybe show a QMessageBox
        qDebug() << "Error: Import name cannot be empty.";
        return;
    }

    // Emit the signal with the import data
    emit meshImported(name, scale, isSkeletal);

    // For demonstration, print the values
    qDebug() << "--- Import Parameters ---";
    qDebug() << "Name:" << name;
    qDebug() << "Scale:" << scale;
    qDebug() << "Is Skeletal:" << isSkeletal;
    qDebug() << "-------------------------";

    // Close the window after import
    this->close();


    if (isSkeletal) {

    }
    else {
        Importer* imp = new Importer;

        auto mesh = imp->ImportEntity(path);

        std::vector<StaticMeshComponent*> meshes;

        meshes = GetSMeshes(mesh, meshes);

        std::vector<SubMesh*> sub_meshes;

        for (auto m : meshes) {

            sub_meshes = GetSubMeshes(m, sub_meshes);
        
        }

        std::string wpath = remove_extension(path) + ".mesh";

        VFile* f = new VFile(wpath.c_str() , FileMode::Write);

        f->WriteInt(sub_meshes.size());

        for (auto mesh : sub_meshes) {

            mesh->Write(f);

        }

        f->Close();
    
         int b = 5;
    }


}

void MeshImportWindow::onCancelClicked()
{
    // Just close the window
    this->close();
}
