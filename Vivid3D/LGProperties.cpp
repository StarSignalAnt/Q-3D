#include "LGProperties.h"
#include "LNode.h"
#include "LGraph.h"
#include "LGraphVariable.h"

// Required Qt Headers for all UI elements
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QFormLayout>
#include <QPushButton>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>

LGProperties::LGProperties(QWidget* parent)
    : QWidget(parent), m_currentNode(nullptr), m_graph(nullptr), m_propertyContainer(nullptr)
{
    // The main layout for this entire panel contains only the scroll area.
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    mainLayout->addWidget(m_scrollArea);

    // Show the initial "Graph Variables" view.
    inspectNode(nullptr);
}

LGProperties::~LGProperties() {}

// This public slot allows the main window to give this panel a pointer to the active graph
void LGProperties::setGraph(LGraph* graph) {
    m_graph = graph;
    // Refresh the view if no node is currently selected
    if (!m_currentNode) {
        inspectNode(nullptr);
    }
}

// This is the complete function that handles both node inspection and graph variable editing
void LGProperties::inspectNode(LNode* node)
{
    m_currentNode = node;

    // To ensure a clean state, we delete the old container and create a new one.
    if (m_propertyContainer) {
        delete m_propertyContainer;
    }

    m_propertyContainer = new QWidget();
    m_scrollArea->setWidget(m_propertyContainer);

    // The main layout for the content inside the scroll area.
    QVBoxLayout* mainContainerLayout = new QVBoxLayout(m_propertyContainer);
    mainContainerLayout->setContentsMargins(4, 4, 4, 4);
    mainContainerLayout->setSpacing(8);

    // --- STATE 1: A NODE IS SELECTED ---
    if (m_currentNode) {
        QFormLayout* formLayout = new QFormLayout();
        mainContainerLayout->addLayout(formLayout);
        formLayout->setLabelAlignment(Qt::AlignLeft);
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

        // Node Name (Editable)
        auto* nameEdit = new QLineEdit(QString::fromStdString(m_currentNode->GetName()));
        connect(nameEdit, &QLineEdit::textChanged, this, [this](const QString& newName) {
            if (m_currentNode) m_currentNode->SetName(newName.toStdString());
            });
        formLayout->addRow("<b>Name</b>", nameEdit);
        formLayout->addRow("<b>Type</b>", new QLabel(QString::fromStdString(m_currentNode->GetTypeName())));

        auto* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        formLayout->addRow(line);

        formLayout->addRow(new QLabel("<b>Default Inputs:</b>"));

        int editablePropertyCount = 0;
        for (LGInput* input : m_currentNode->GetInputs()) {
            if (input->isConnected()) continue;

            editablePropertyCount++;
            DataType type = input->GetType();

            // --- This is the full switch for creating node input editors ---
            switch (type) {
            case DataType::Bool: {
                auto* editorWidget = new QCheckBox();
                if (auto* pval = std::get_if<bool>(&input->GetDefaultValue())) editorWidget->setChecked(*pval);
                connect(editorWidget, &QCheckBox::checkStateChanged, [input](int state) { input->SetDefaultValue(LGInput::ValueVariant{ state == Qt::Checked }); });
                formLayout->addRow(QString::fromStdString(input->GetName()), editorWidget);
                break;
            }
            case DataType::String:
            case DataType::GraphNodeRef: {
                auto* editorWidget = new QLineEdit();
                if (auto* pval = std::get_if<std::string>(&input->GetDefaultValue())) editorWidget->setText(QString::fromStdString(*pval));
                connect(editorWidget, &QLineEdit::textChanged, [input](const QString& text) { input->SetDefaultValue(LGInput::ValueVariant{ text.toStdString() }); });
                formLayout->addRow(QString::fromStdString(input->GetName()), editorWidget);
                break;
            }
            case DataType::Int: {
                auto* editorWidget = new QSpinBox();
                editorWidget->setButtonSymbols(QAbstractSpinBox::NoButtons); editorWidget->setRange(INT_MIN, INT_MAX);
                if (auto* pval = std::get_if<int>(&input->GetDefaultValue())) editorWidget->setValue(*pval);
                connect(editorWidget, qOverload<int>(&QSpinBox::valueChanged), [input](int val) { input->SetDefaultValue(LGInput::ValueVariant{ val }); });
                formLayout->addRow(QString::fromStdString(input->GetName()), editorWidget);
                break;
            }
            case DataType::Float: {
                auto* editorWidget = new QDoubleSpinBox();
                editorWidget->setButtonSymbols(QAbstractSpinBox::NoButtons); editorWidget->setRange(-1.0e+12, 1.0e+12); editorWidget->setDecimals(3);
                if (auto* pval = std::get_if<float>(&input->GetDefaultValue())) editorWidget->setValue(*pval);
                connect(editorWidget, qOverload<double>(&QDoubleSpinBox::valueChanged), [input](double val) { input->SetDefaultValue(LGInput::ValueVariant{ static_cast<float>(val) }); });
                formLayout->addRow(QString::fromStdString(input->GetName()), editorWidget);
                break;
            }
            case DataType::Vec3: {
                auto* vecWidget = new QWidget();
                auto* vecLayout = new QHBoxLayout(vecWidget);
                vecLayout->setContentsMargins(0, 0, 0, 0); vecLayout->setSpacing(2);
                glm::vec3 initialVec = std::get_if<glm::vec3>(&input->GetDefaultValue()) ? *std::get_if<glm::vec3>(&input->GetDefaultValue()) : glm::vec3(0.0f);
                for (int j = 0; j < 3; ++j) {
                    auto* spin = new QDoubleSpinBox();
                    spin->setButtonSymbols(QAbstractSpinBox::NoButtons); spin->setDecimals(2); spin->setRange(-1e12, 1e12);
                    spin->setValue(initialVec[j]);
                    connect(spin, qOverload<double>(&QDoubleSpinBox::valueChanged), [input, j](double value) {
                        glm::vec3 currentVec = std::get_if<glm::vec3>(&input->GetDefaultValue()) ? *std::get_if<glm::vec3>(&input->GetDefaultValue()) : glm::vec3(0.0f);
                        currentVec[j] = static_cast<float>(value);
                        input->SetDefaultValue(LGInput::ValueVariant{ currentVec });
                        });
                    vecLayout->addWidget(spin, 1);
                }
                formLayout->addRow(QString::fromStdString(input->GetName()), vecWidget);
                break;
            }
            case DataType::Vec4: {
                auto* vecWidget = new QWidget();
                auto* vecLayout = new QHBoxLayout(vecWidget);
                vecLayout->setContentsMargins(0, 0, 0, 0); vecLayout->setSpacing(2);
                glm::vec4 initialVec = std::get_if<glm::vec4>(&input->GetDefaultValue()) ? *std::get_if<glm::vec4>(&input->GetDefaultValue()) : glm::vec4(0.0f);
                for (int j = 0; j < 4; ++j) {
                    auto* spin = new QDoubleSpinBox();
                    spin->setButtonSymbols(QAbstractSpinBox::NoButtons); spin->setDecimals(2); spin->setRange(-1e12, 1e12);
                    spin->setValue(initialVec[j]);
                    connect(spin, qOverload<double>(&QDoubleSpinBox::valueChanged), [input, j](double value) {
                        glm::vec4 currentVec = std::get_if<glm::vec4>(&input->GetDefaultValue()) ? *std::get_if<glm::vec4>(&input->GetDefaultValue()) : glm::vec4(0.0f);
                        currentVec[j] = static_cast<float>(value);
                        input->SetDefaultValue(LGInput::ValueVariant{ currentVec });
                        });
                    vecLayout->addWidget(spin, 1);
                }
                formLayout->addRow(QString::fromStdString(input->GetName()), vecWidget);
                break;
            }
            case DataType::Mat4:
            default:
                formLayout->addRow(QString::fromStdString(input->GetName()), new QLabel("<i>(Not editable)</i>"));
                break;
            }
        }
        if (editablePropertyCount == 0) {
            formLayout->addRow(new QLabel("<i>(No editable inputs)</i>"));
        }

    }
    else if (m_graph) {
        // --- STATE 2: NO NODE IS SELECTED, SHOW GRAPH VARIABLES ---
        mainContainerLayout->addWidget(new QLabel("<b>Graph Variables</b>"));

        QPushButton* addButton = new QPushButton("Add New Variable...");
        mainContainerLayout->addWidget(addButton);
        connect(addButton, &QPushButton::clicked, this, [this]() {
            if (!m_graph) return;
            QDialog dialog(this);
            dialog.setWindowTitle("Create Variable");
            QFormLayout form(&dialog);
            auto* nameEdit = new QLineEdit(&dialog);
            form.addRow("Name:", nameEdit);
            auto* typeCombo = new QComboBox(&dialog);
            typeCombo->addItems({ "Float", "Int", "Bool", "String", "Vec3", "Vec4", "Mat4", "GraphNode" });
            form.addRow("Type:", typeCombo);
            QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
            form.addRow(&buttonBox);
            connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

            if (dialog.exec() == QDialog::Accepted) {
                std::string newName = nameEdit->text().toStdString();
                DataType newType = DataType::Float;
                QString typeString = typeCombo->currentText();
                if (typeString == "Int") newType = DataType::Int; else if (typeString == "Bool") newType = DataType::Bool; else if (typeString == "String") newType = DataType::String; else if (typeString == "Vec3") newType = DataType::Vec3; else if (typeString == "Vec4") newType = DataType::Vec4; else if (typeString == "Mat4") newType = DataType::Mat4; else if (typeString == "GraphNode") newType = DataType::GraphNodeRef;
                if (!newName.empty() && !m_graph->FindVariable(newName)) {
                    m_graph->AddVariable(new LGraphVariable(newName, newType));
                    inspectNode(nullptr);
                }
            }
            });

        auto* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        mainContainerLayout->addWidget(line);

        if (!m_graph->GetVariables().empty()) {
            QFormLayout* varFormLayout = new QFormLayout();
            varFormLayout->setLabelAlignment(Qt::AlignLeft);
            varFormLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
            for (LGraphVariable* var : m_graph->GetVariables()) {
                // This is the full switch for creating graph variable editors
                switch (var->GetType()) {
                case DataType::Bool: {
                    auto* editorWidget = new QCheckBox();
                    if (auto* pval = std::get_if<bool>(&var->GetDefaultValue())) editorWidget->setChecked(*pval);
                    connect(editorWidget, &QCheckBox::checkStateChanged, [var](int state) { var->SetDefaultValue(LGInput::ValueVariant{ state == Qt::Checked }); });
                    varFormLayout->addRow(QString::fromStdString(var->GetName()), editorWidget);
                    break;
                }
                case DataType::String:
                case DataType::GraphNodeRef: {
                    auto* editorWidget = new QLineEdit();
                    if (auto* pval = std::get_if<std::string>(&var->GetDefaultValue())) editorWidget->setText(QString::fromStdString(*pval));
                    connect(editorWidget, &QLineEdit::textChanged, [var](const QString& text) { var->SetDefaultValue(LGInput::ValueVariant{ text.toStdString() }); });
                    varFormLayout->addRow(QString::fromStdString(var->GetName()), editorWidget);
                    break;
                }
                case DataType::Int: {
                    auto* editorWidget = new QSpinBox();
                    editorWidget->setButtonSymbols(QAbstractSpinBox::NoButtons); editorWidget->setRange(INT_MIN, INT_MAX);
                    if (auto* pval = std::get_if<int>(&var->GetDefaultValue())) editorWidget->setValue(*pval);
                    connect(editorWidget, qOverload<int>(&QSpinBox::valueChanged), [var](int val) { var->SetDefaultValue(LGInput::ValueVariant{ val }); });
                    varFormLayout->addRow(QString::fromStdString(var->GetName()), editorWidget);
                    break;
                }
                case DataType::Float: {
                    auto* editorWidget = new QDoubleSpinBox();
                    editorWidget->setButtonSymbols(QAbstractSpinBox::NoButtons); editorWidget->setRange(-1.0e+12, 1.0e+12); editorWidget->setDecimals(3);
                    if (auto* pval = std::get_if<float>(&var->GetDefaultValue())) editorWidget->setValue(*pval);
                    connect(editorWidget, qOverload<double>(&QDoubleSpinBox::valueChanged), [var](double val) { var->SetDefaultValue(LGInput::ValueVariant{ static_cast<float>(val) }); });
                    varFormLayout->addRow(QString::fromStdString(var->GetName()), editorWidget);
                    break;
                }
                case DataType::Vec3: {
                    auto* vecWidget = new QWidget();
                    auto* vecLayout = new QHBoxLayout(vecWidget);
                    vecLayout->setContentsMargins(0, 0, 0, 0); vecLayout->setSpacing(2);
                    glm::vec3 initialVec = std::get_if<glm::vec3>(&var->GetDefaultValue()) ? *std::get_if<glm::vec3>(&var->GetDefaultValue()) : glm::vec3(0.0f);
                    for (int j = 0; j < 3; ++j) {
                        auto* spin = new QDoubleSpinBox();
                        spin->setButtonSymbols(QAbstractSpinBox::NoButtons); spin->setDecimals(2); spin->setRange(-1e12, 1e12);
                        spin->setValue(initialVec[j]);
                        connect(spin, qOverload<double>(&QDoubleSpinBox::valueChanged), [var, j](double value) {
                            glm::vec3 currentVec = std::get_if<glm::vec3>(&var->GetDefaultValue()) ? *std::get_if<glm::vec3>(&var->GetDefaultValue()) : glm::vec3(0.0f);
                            currentVec[j] = static_cast<float>(value);
                            var->SetDefaultValue(LGInput::ValueVariant{ currentVec });
                            });
                        vecLayout->addWidget(spin, 1);
                    }
                    varFormLayout->addRow(QString::fromStdString(var->GetName()), vecWidget);
                    break;
                }
                case DataType::Vec4: {
                    auto* vecWidget = new QWidget();
                    auto* vecLayout = new QHBoxLayout(vecWidget);
                    vecLayout->setContentsMargins(0, 0, 0, 0); vecLayout->setSpacing(2);
                    glm::vec4 initialVec = std::get_if<glm::vec4>(&var->GetDefaultValue()) ? *std::get_if<glm::vec4>(&var->GetDefaultValue()) : glm::vec4(0.0f);
                    for (int j = 0; j < 4; ++j) {
                        auto* spin = new QDoubleSpinBox();
                        spin->setButtonSymbols(QAbstractSpinBox::NoButtons); spin->setDecimals(2); spin->setRange(-1e12, 1e12);
                        spin->setValue(initialVec[j]);
                        connect(spin, qOverload<double>(&QDoubleSpinBox::valueChanged), [var, j](double value) {
                            glm::vec4 currentVec = std::get_if<glm::vec4>(&var->GetDefaultValue()) ? *std::get_if<glm::vec4>(&var->GetDefaultValue()) : glm::vec4(0.0f);
                            currentVec[j] = static_cast<float>(value);
                            var->SetDefaultValue(LGInput::ValueVariant{ currentVec });
                            });
                        vecLayout->addWidget(spin, 1);
                    }
                    varFormLayout->addRow(QString::fromStdString(var->GetName()), vecWidget);
                    break;
                }
                case DataType::Mat4:
                default:
                    varFormLayout->addRow(QString::fromStdString(var->GetName()), new QLabel("<i>(Not editable)</i>"));
                    break;
                }
            }
            mainContainerLayout->addLayout(varFormLayout);
        }
        else {
            mainContainerLayout->addWidget(new QLabel("<i>No variables in this graph.</i>"));
        }
    }
    else {
        mainContainerLayout->addWidget(new QLabel("No graph loaded."));
    }

    mainContainerLayout->addStretch();
}