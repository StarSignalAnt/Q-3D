#pragma once
#include <qgraphicsitem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QMouseEvent>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QMap> // <-- Add this include at the top of LGNode.h
#include <qcheckbox.h>
// Include headers for the inline widgets
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include "LNode.h"
#include "SocketWidget.h" // Include the new SocketWidget header
#include "ConnectionItem.h"
// The QGraphicsItem wrapper class. Its definition is stable.
#include <qgraphicsitem>
#include <QGraphicsProxyWidget>
#include <QVariant>

// Forward declarations
class LNode;
class LGNodeWidget;
class SocketWidget;

class LGNode : public QGraphicsItem {
public:
    enum { Type = UserType + 1 };
    int type() const override { return Type; }
    LGNode(LNode* logicNode);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    LNode* getLogicNode() const { return m_logicNode; }
    SocketWidget* getSocket(SocketWidget::SocketDirection dir, SocketWidget::SocketType type, int portIndex);
    QGraphicsProxyWidget* getProxyWidget() const { return m_proxy; }
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    LNode* m_logicNode;
    QGraphicsProxyWidget* m_proxy;
    QRectF m_bounds;
}; // The QFrame that holds the node's visual content.
class LGNodeWidget : public QFrame {
    Q_OBJECT
public:
    explicit LGNodeWidget(LNode* logicNode, QWidget* parent = nullptr)
        : QFrame(parent)
    {
        // --- Basic Node Styling ---
        setFrameStyle(QFrame::Panel | QFrame::Raised);
        setStyleSheet("background-color: #383838; color: white; border-radius: 8px;");

        QVBoxLayout* rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(8, 8, 8, 8);
        rootLayout->setSpacing(5);

        // --- Header Section (Exec In pin and Title) ---
        QWidget* headerWidget = new QWidget();
        QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
        headerLayout->setContentsMargins(0, 0, 0, 0);

        if (logicNode->hasExecIn()) {
            auto execInSocket = new SocketWidget(logicNode, SocketWidget::SocketDirection::In, SocketWidget::SocketType::Exec, 0, this);
            execInSocket->setFixedSize(10, 10);
            execInSocket->setStyleSheet("background-color: white; border-radius: 3px;");
            headerLayout->addWidget(execInSocket);
        }

        headerLayout->addStretch();
        QLabel* titleLabel = new QLabel(QString::fromStdString(logicNode->GetName()), this);
        titleLabel->setStyleSheet("font-weight: bold;");
        headerLayout->addWidget(titleLabel);
        headerLayout->addStretch();
        rootLayout->addWidget(headerWidget);

        QFrame* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        rootLayout->addWidget(line);

        // --- Pins Section ---
        bool hasInputs = !logicNode->GetInputs().empty();
        bool hasDataOutputs = !logicNode->GetOutputs().empty();
        bool hasExecOutputs = !logicNode->GetExecOutputs().empty();

        if (hasInputs || hasDataOutputs || hasExecOutputs) {
            QWidget* columnsContainer = new QWidget();
            QHBoxLayout* columnsLayout = new QHBoxLayout(columnsContainer);
            columnsLayout->setContentsMargins(0, 0, 0, 0);
            columnsLayout->setSpacing(15);

            // --- Input Column (Left) ---
            if (hasInputs) {
                QVBoxLayout* inputsLayout = new QVBoxLayout();
                inputsLayout->setSpacing(4);
                for (int i = 0; i < logicNode->GetInputs().size(); ++i) {
                    const auto& input = logicNode->GetInputs()[i];
                    QWidget* rowWidget = new QWidget();
                    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
                    rowLayout->setContentsMargins(0, 0, 0, 0);
                    rowLayout->setSpacing(5);

                    auto socket = new SocketWidget(logicNode, SocketWidget::SocketDirection::In, SocketWidget::SocketType::Data, i, rowWidget);
                    socket->setFixedSize(10, 10);
                    socket->setStyleSheet("background-color: #5DADE2; border-radius: 5px;");
                    QLabel* label = new QLabel(QString::fromStdString(input->GetName()));
                    rowLayout->addWidget(socket);
                    rowLayout->addWidget(label);

                    DataType type = input->GetType();
                    QWidget* editableWidget = nullptr;
                    const char* widgetStyle = "background-color: #2E2E2E; border: 1px solid #555; border-radius: 3px;";

                    // THIS IS THE FULL SWITCH BLOCK THAT WAS PREVIOUSLY MISSING
                    switch (type) {
                    case DataType::Bool: {
                        auto* checkBox = new QCheckBox();
                        editableWidget = checkBox;
                        if (const auto* pval = std::get_if<bool>(&input->GetDefaultValue())) { checkBox->setChecked(*pval); }
                        connect(checkBox, &QCheckBox::checkStateChanged, [input](int state) {
                            LGInput::ValueVariant val = (state == Qt::Checked);
                            input->SetDefaultValue(val);
                            });
                        break;
                    }
                    case DataType::String:
                    case DataType::GraphNodeRef: {
                        auto* lineEdit = new QLineEdit();
                        lineEdit->setStyleSheet(widgetStyle);
                        editableWidget = lineEdit;
                        lineEdit->setObjectName("input_widget_" + QString::number(i));
                        if (const auto* pval = std::get_if<std::string>(&input->GetDefaultValue())) { lineEdit->setText(QString::fromStdString(*pval)); }
                        connect(lineEdit, &QLineEdit::textChanged, [input](const QString& text) {
                            LGInput::ValueVariant val = text.toStdString();
                            input->SetDefaultValue(val);
                            });
                        break;
                    }
                    case DataType::Int: {
                        auto* spinBox = new QSpinBox();
                        spinBox->setStyleSheet(widgetStyle);
                        spinBox->setRange(-2147483647, 2147483647);
                        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
                        editableWidget = spinBox;
                        if (const auto* pval = std::get_if<int>(&input->GetDefaultValue())) { spinBox->setValue(*pval); }
                        connect(spinBox, qOverload<int>(&QSpinBox::valueChanged), [input](int value) {
                            LGInput::ValueVariant val = value;
                            input->SetDefaultValue(val);
                            });
                        break;
                    }
                    case DataType::Float: {
                        auto* doubleSpinBox = new QDoubleSpinBox();
                        doubleSpinBox->setStyleSheet(widgetStyle);
                        doubleSpinBox->setRange(-1.0e+12, 1.0e+12);
                        doubleSpinBox->setDecimals(3);
                        doubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
                        editableWidget = doubleSpinBox;
                        if (const auto* pval = std::get_if<float>(&input->GetDefaultValue())) { doubleSpinBox->setValue(*pval); }
                        connect(doubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [input](double value) {
                            LGInput::ValueVariant val = static_cast<float>(value);
                            input->SetDefaultValue(val);
                            });
                        break;
                    }
                    case DataType::Vec3: {
                        auto* vecWidget = new QWidget();
                        auto* vecLayout = new QHBoxLayout(vecWidget);
                        vecLayout->setContentsMargins(0, 0, 0, 0); vecLayout->setSpacing(2);
                        glm::vec3 initialVec = glm::vec3(0.0f);
                        if (const auto* pval = std::get_if<glm::vec3>(&input->GetDefaultValue())) { initialVec = *pval; }
                        for (int j = 0; j < 3; ++j) {
                            auto* spin = new QDoubleSpinBox();
                            spin->setStyleSheet(widgetStyle); spin->setFixedWidth(50);
                            spin->setRange(-1.0e+12, 1.0e+12); spin->setDecimals(2);
                            spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
                            spin->setValue(initialVec[j]);
                            vecLayout->addWidget(spin);
                            connect(spin, qOverload<double>(&QDoubleSpinBox::valueChanged), [input, j](double value) {
                                glm::vec3 currentVec = glm::vec3(0.0f);
                                if (const auto* pval = std::get_if<glm::vec3>(&input->GetDefaultValue())) currentVec = *pval;
                                currentVec[j] = static_cast<float>(value);
                                LGInput::ValueVariant val = currentVec;
                                input->SetDefaultValue(val);
                                });
                        }
                        editableWidget = vecWidget;
                        break;
                    }
                                       // (Add cases for Vec2 and Vec4 here if needed)
                    default:
                        break;
                    }

                    if (editableWidget) {
                        rowLayout->addWidget(editableWidget);
                        m_widgetToInputMap[editableWidget] = input;

                    }
                    else {
                        rowLayout->addStretch();
                    }
                    inputsLayout->addWidget(rowWidget);
                }
                inputsLayout->addStretch();
                columnsLayout->addLayout(inputsLayout);
            }

            // --- Output Column (Right) ---
            if (hasDataOutputs || hasExecOutputs) {
                QVBoxLayout* outputsLayout = new QVBoxLayout();
                outputsLayout->setSpacing(4);

                // Create Execution Output pins
                for (int i = 0; i < logicNode->GetExecOutputs().size(); ++i) {
                    const auto& execPin = logicNode->GetExecOutputs()[i];
                    QWidget* rowWidget = new QWidget();
                    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
                    rowLayout->setContentsMargins(0, 0, 0, 0);
                    rowLayout->setSpacing(5);
                    QLabel* label = new QLabel(QString::fromStdString(execPin->name));
                    auto socket = new SocketWidget(logicNode, SocketWidget::SocketDirection::Out, SocketWidget::SocketType::Exec, i, rowWidget);
                    socket->setFixedSize(10, 10);
                    socket->setStyleSheet("background-color: white; border-radius: 3px;");
                    rowLayout->addStretch();
                    rowLayout->addWidget(label);
                    rowLayout->addWidget(socket);
                    outputsLayout->addWidget(rowWidget);
                }

                // Create Data Output pins
                for (int i = 0; i < logicNode->GetOutputs().size(); ++i) {
                    const auto& output = logicNode->GetOutputs()[i];
                    QWidget* rowWidget = new QWidget();
                    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
                    rowLayout->setContentsMargins(0, 0, 0, 0);
                    rowLayout->setSpacing(5);
                    QLabel* label = new QLabel(QString::fromStdString(output->GetName()));
                    auto socket = new SocketWidget(logicNode, SocketWidget::SocketDirection::Out, SocketWidget::SocketType::Data, i, rowWidget);
                    socket->setFixedSize(10, 10);
                    socket->setStyleSheet("background-color: #A9DFBF; border-radius: 5px;");
                    rowLayout->addStretch();
                    rowLayout->addWidget(label);
                    rowLayout->addWidget(socket);
                    outputsLayout->addWidget(rowWidget);
                }

                outputsLayout->addStretch();
                columnsLayout->addLayout(outputsLayout);
            }
            rootLayout->addWidget(columnsContainer);
        }

        adjustSize();
    }
    // A public getter to access the map from the outside.
    const QMap<QWidget*, LGInput*>& getWidgetToInputMap() const {
        return m_widgetToInputMap;
    }
    QMap<QWidget*, LGInput*> m_widgetToInputMap;
};