#include "PropertiesEditor.h"
#include <QScrollArea>
#include <QFrame>
#include <qvector3d.h>

PropertiesEditor::PropertiesEditor(QWidget *parent)
	: QWidget(parent)
{
	//ui.setupUi(this);
	setAutoFillBackground(true);
	setWindowTitle("Properties");
	QPalette pal = palette();
	pal.setColor(QPalette::Window, QColor("#502020"));
	setAutoFillBackground(true);
	setPalette(pal);
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Create scroll area to handle overflow
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    // Create content widget that will hold all properties
    m_contentWidget = new QWidget();
    m_contentWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    // Create vertical layout for properties
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(8, 8, 8, 8);
    m_contentLayout->setSpacing(4);

    PropertyText* m_nameProp = new PropertyText("Name", "Default Object Name");
    PropertyText* m_tagProp = new PropertyText("Tag", "Untagged");

    m_contentLayout->addWidget(m_nameProp);
    m_contentLayout->addWidget(m_tagProp);

    // Connect signals if needed
    connect(m_nameProp, &PropertyText::textChanged,
        this, [this](const QString& text) {
            // Handle name change
            qDebug() << "Name changed to:" << text;
        });

    connect(m_tagProp, &PropertyText::textChanged,
        this, [this](const QString& text) {
            // Handle tag change
            qDebug() << "Tag changed to:" << text;
        });

    // Create property widgets
    m_positionProp = new PropertyVec3("Position", nullptr);
    m_scaleProp = new PropertyVec3("Scale",nullptr);
    m_rotationProp = new PropertyVec3("Rotation",nullptr);

    // Set initial values
    m_positionProp->setValue(QVector3D(0.0f, 0.0f, 0.0f));
    m_scaleProp->setValue(QVector3D(1.0f, 1.0f, 1.0f));
    m_rotationProp->setValue(QVector3D(0.0f, 0.0f, 0.0f));

    // Add properties to layout
    m_contentLayout->addWidget(m_positionProp);
    m_contentLayout->addWidget(m_scaleProp);
    m_contentLayout->addWidget(m_rotationProp);

    // Add spacer to push properties to the top
    m_contentLayout->addStretch();

    // Connect signals if needed
    connect(m_positionProp, &PropertyVec3::valueChanged,
        this, [this](const QVector3D& value) {
            // Handle position change
         //   qDebug() << "Position changed to:" << value;
        });

    connect(m_scaleProp, &PropertyVec3::valueChanged,
        this, [this](const QVector3D& value) {
            // Handle scale change
     //       qDebug() << "Scale changed to:" << value;
        });

    connect(m_rotationProp, &PropertyVec3::valueChanged,
        this, [this](const QVector3D& value) {
            // Handle rotation change
       //     qDebug() << "Rotation changed to:" << value;
        });

    // Set up scroll area
    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea);

    // Set minimum size for the property editor
    setMinimumWidth(250);
    setMinimumHeight(200);
}

PropertiesEditor::~PropertiesEditor()
{}


QSize PropertiesEditor::sizeHint() const
{
	return QSize(300, 400);  // Suggested initial width: 300px, height: 400px
}

