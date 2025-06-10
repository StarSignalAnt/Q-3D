#pragma once

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPixmap>
#include <string>
#include <functional>

class PropertyTexture : public QWidget
{
    Q_OBJECT

public:
    PropertyTexture(const QString& label, QWidget* parent = nullptr);
    ~PropertyTexture();

    void setTexturePath(const std::string& path);
    std::string getTexturePath() const;

signals:
    void textureChanged(const std::string& path);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void setupImageBox();
    void updateImageDisplay();
    bool isImageFile(const std::string& path) const;

    QLabel* m_label;
    QLabel* m_imageBox;
    std::string m_texturePath;

    static const int IMAGE_BOX_SIZE = 64;
};