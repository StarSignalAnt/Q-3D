#ifndef MESHIMPORTWINDOW_H
#define MESHIMPORTWINDOW_H

#include <QWidget>
#include <QString>

class QLineEdit;
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;

class MeshImportWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MeshImportWindow(const QString& originalName, QWidget* parent = nullptr);
    ~MeshImportWindow();

signals:
    // Signal emitted when the import button is clicked
    void meshImported(const QString& name, double scale, bool isSkeletal);

private slots:
    void onImportClicked();
    void onCancelClicked();

private:
    QLineEdit* m_nameLineEdit;
    QDoubleSpinBox* m_scaleSpinBox;
    QCheckBox* m_skeletalCheckBox;
    QPushButton* m_importButton;
    QPushButton* m_cancelButton;
    std::string path;
};

#endif // MESHIMPORTWINDOW_H
