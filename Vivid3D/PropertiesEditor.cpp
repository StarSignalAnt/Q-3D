#include "PropertiesEditor.h"
#include <QScrollArea>
#include <QFrame>
#include <qvector3d.h>
#include "PropertySlider.h"
#include "PropertyFloat.h"
#include "PropertyHeader.h"
#include <string>
#include "GraphNode.h"

PropertiesEditor* PropertiesEditor::m_Instance = nullptr;

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
  
    m_Instance = this;
    /*
    BeginUI();

    PropertyHeader* basicHeader = new PropertyHeader("Basic Properties");
    m_contentLayout->addWidget(basicHeader);


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


    PropertyHeader* transformHeader = new PropertyHeader("Transform");
    m_contentLayout->addWidget(transformHeader);
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

    PropertySlider* m_opacitySlider = new PropertySlider("Opacity", 0, 100);
    PropertySlider* m_volumeSlider = new PropertySlider("Volume", 0, 255);
    PropertySlider* m_qualitySlider = new PropertySlider("Quality", 1, 10);

    // Set initial values
    m_opacitySlider->setValue(100);  // Full opacity
    m_volumeSlider->setValue(128);   // Half volume
    m_qualitySlider->setValue(5);    // Medium quality

    // Add properties to layout
    m_contentLayout->addWidget(m_opacitySlider);
    m_contentLayout->addWidget(m_volumeSlider);
    m_contentLayout->addWidget(m_qualitySlider);

    // Connect signals if needed
    connect(m_opacitySlider, &PropertySlider::valueChanged,
        this, [this](int value) {
            // Handle opacity change
            qDebug() << "Opacity changed to:" << value;
        });

    connect(m_volumeSlider, &PropertySlider::valueChanged,
        this, [this](int value) {
            // Handle volume change
            qDebug() << "Volume changed to:" << value;
        });

    PropertyFloat* m_speedProp = new PropertyFloat("Speed", 0.0, 100.0, 0.1);      // 0-100, step by 0.1
    PropertyFloat* m_weightProp = new PropertyFloat("Weight", 0.0, 1000.0, 1.0);   // 0-1000, step by 1.0
    PropertyFloat* m_precisionProp = new PropertyFloat("Precision", 0.0, 1.0, 0.01); // 0-1, step by 0.01
    PropertyFloat* m_temperatureProp = new PropertyFloat("Temperature", -273.15, 1000.0, 0.5); // Celsius range

    // Set initial values
    m_speedProp->setValue(10.5);
    m_weightProp->setValue(75.0);
    m_precisionProp->setValue(0.85);
    m_temperatureProp->setValue(20.0);

    // Add properties to layout
    m_contentLayout->addWidget(m_speedProp);
    m_contentLayout->addWidget(m_weightProp);
    m_contentLayout->addWidget(m_precisionProp);
    m_contentLayout->addWidget(m_temperatureProp);

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

    EndUI();
   */
}

PropertiesEditor::~PropertiesEditor()
{}

QVector3D ToQV(glm::vec3 v)
{
    return QVector3D(v.x, v.y, v.z);
}

void PropertiesEditor::SetNode(GraphNode* node) {

    BeginUI();

    AddHeader("Node");



    AddText("Name", node->GetName().c_str(), [node](const QString& text) {
        node->SetName(text.toStdString());
        });

    AddVec3("Position", ToQV(node->GetPosition()), [node](const QVector3D& value) {
        node->SetPosition(glm::vec3(value.x(), value.y(), value.z()));
        });
    AddVec3("Rotation", ToQV(node->GetEularRotation()), [node](const QVector3D& value) {
        node->SetRotation(glm::vec3(value.x(), value.y(), value.z()));
        });
    AddVec3("Scale", ToQV(node->GetScale()), [node](const QVector3D& value) {
        node->SetScale(glm::vec3(value.x(), value.y(), value.z()));
        });


    EndUI();

}

QSize PropertiesEditor::sizeHint() const
{
	return QSize(300, 400);  // Suggested initial width: 300px, height: 400px
}

void PropertiesEditor::BeginUI() {
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
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(0);
}

void PropertiesEditor::EndUI() {
    // This function can be used to finalize the UI if needed
     // Set up scroll area
    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea);

    // Set minimum size for the property editor
    setMinimumWidth(250);
    setMinimumHeight(200);
}


void PropertiesEditor::AddHeader(const QString& text)
{
    PropertyHeader* header = new PropertyHeader(text);
    m_contentLayout->addWidget(header);
}

void PropertiesEditor::AddText(const QString& label, const QString& defaultText,
    std::function<void(const QString&)> callback)
{
    PropertyText* textProp = new PropertyText(label, defaultText);
    m_contentLayout->addWidget(textProp);

    if (callback) {
        connect(textProp, &PropertyText::textChanged, this, [callback](const QString& text) {
            callback(text);
            });
    }
}

void PropertiesEditor::AddVec3(const QString& label, const QVector3D& defaultValue,
    std::function<void(const QVector3D&)> callback)
{
    PropertyVec3* vec3Prop = new PropertyVec3(label, nullptr);
    vec3Prop->setValue(defaultValue);
    m_contentLayout->addWidget(vec3Prop);

    if (callback) {
        connect(vec3Prop, &PropertyVec3::valueChanged, this, [callback](const QVector3D& value) {
            callback(value);
            });
    }
}

void PropertiesEditor::AddFloat(const QString& label, double minValue, double maxValue,
    double interval, double defaultValue,
    std::function<void(double)> callback)
{
    PropertyFloat* floatProp = new PropertyFloat(label, minValue, maxValue, interval);
    floatProp->setValue(defaultValue);
    m_contentLayout->addWidget(floatProp);

    if (callback) {
        connect(floatProp, &PropertyFloat::valueChanged, this, [callback](double value) {
            callback(value);
            });
    }
}

void PropertiesEditor::AddSlider(const QString& label, int minValue, int maxValue,
    int defaultValue, std::function<void(int)> callback)
{
    PropertySlider* sliderProp = new PropertySlider(label, minValue, maxValue);
    sliderProp->setValue(defaultValue);
    m_contentLayout->addWidget(sliderProp);

    if (callback) {
        connect(sliderProp, &PropertySlider::valueChanged, this, [callback](int value) {
            callback(value);
            });
    }
}