#include "PropertyTexture.h"
#include <QHBoxLayout>
#include <QPixmap>
#include <QDebug>
#include <QFileInfo>

PropertyTexture::PropertyTexture(const QString& label, QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(8); // Fixed spacing between label and image

    // Create and configure label
    m_label = new QLabel(label + ":");
    m_label->setFixedWidth(80); // Fixed width for consistent alignment
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Create image box
    setupImageBox();

    // Add widgets to layout
    layout->addWidget(m_label);
    layout->addWidget(m_imageBox);
    layout->addStretch(); // Push everything to the left

    // Set fixed height for consistency
    setFixedHeight(IMAGE_BOX_SIZE + 8); // Add padding
}

PropertyTexture::~PropertyTexture()
{
}

void PropertyTexture::setupImageBox()
{
    m_imageBox = new QLabel;
    m_imageBox->setFixedSize(IMAGE_BOX_SIZE, IMAGE_BOX_SIZE);
    m_imageBox->setScaledContents(true);
    m_imageBox->setAlignment(Qt::AlignCenter);
    m_imageBox->setStyleSheet(
        "QLabel {"
        "   background-color: white;"
        "   border: 2px solid #888888;"
        "   border-radius: 4px;"
        "}"
    );

    // Set default white background
    QPixmap whitePixmap(IMAGE_BOX_SIZE, IMAGE_BOX_SIZE);
    whitePixmap.fill(Qt::white);
    m_imageBox->setPixmap(whitePixmap);

    // Enable drag and drop
    setAcceptDrops(true);
}

void PropertyTexture::setTexturePath(const std::string& path)
{
    m_texturePath = path;
    updateImageDisplay();
}

std::string PropertyTexture::getTexturePath() const
{
    return m_texturePath;
}

void PropertyTexture::updateImageDisplay()
{
    if (m_texturePath.empty()) {
        // Set white background when no texture
        QPixmap whitePixmap(IMAGE_BOX_SIZE, IMAGE_BOX_SIZE);
        whitePixmap.fill(Qt::white);
        m_imageBox->setPixmap(whitePixmap);
        return;
    }

    // Load and display the texture
    QString qPath = QString::fromStdString(m_texturePath);
    QPixmap pixmap(qPath);

    if (pixmap.isNull()) {
        // If image failed to load, show error pattern or keep white
        QPixmap errorPixmap(IMAGE_BOX_SIZE, IMAGE_BOX_SIZE);
        errorPixmap.fill(Qt::lightGray);
        m_imageBox->setPixmap(errorPixmap);
        qDebug() << "Failed to load texture:" << qPath;
    }
    else {
        // Scale the pixmap to fit the image box
        QPixmap scaledPixmap = pixmap.scaled(IMAGE_BOX_SIZE, IMAGE_BOX_SIZE,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation);
        m_imageBox->setPixmap(scaledPixmap);
    }
}

bool PropertyTexture::isImageFile(const std::string& path) const
{
    QString qPath = QString::fromStdString(path);
    QFileInfo fileInfo(qPath);
    QString extension = fileInfo.suffix().toLower();

    // Check for common image file extensions
    QStringList imageExtensions = { "jpg", "jpeg", "png", "bmp", "tga", "tiff", "gif", "hdr", "exr" };
    return imageExtensions.contains(extension);
}

void PropertyTexture::dragEnterEvent(QDragEnterEvent* event)
{
    // Check if the dragged data contains text (file path)
    if (event->mimeData()->hasText()) {
        QString draggedPath = event->mimeData()->text();

        // Accept if it's an image file
        if (isImageFile(draggedPath.toStdString())) {
            event->acceptProposedAction();

            // Visual feedback - change border color during drag
            m_imageBox->setStyleSheet(
                "QLabel {"
                "   background-color: white;"
                "   border: 2px solid #4CAF50;"  // Green border when accepting
                "   border-radius: 4px;"
                "}"
            );
        }
    }
}

void PropertyTexture::dropEvent(QDropEvent* event)
{
    // Reset border style
    m_imageBox->setStyleSheet(
        "QLabel {"
        "   background-color: white;"
        "   border: 2px solid #888888;"
        "   border-radius: 4px;"
        "}"
    );

    if (event->mimeData()->hasText()) {
        QString droppedPath = event->mimeData()->text();

        if (isImageFile(droppedPath.toStdString())) {
            setTexturePath(droppedPath.toStdString());
            emit textureChanged(m_texturePath);
            event->acceptProposedAction();
        }
    }
}