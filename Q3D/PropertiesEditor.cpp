#include "PropertiesEditor.h"
#include <QScrollArea>
#include <QFrame>
#include <qvector3d.h>
#include "PropertySlider.h"
#include "PropertyFloat.h"
#include "PropertyHeader.h"
#include "PropertyNode.h"
#include <string>
#include "GraphNode.h"
#include "SceneView.h"
#include "ScriptComponent.h"
#include "LightComponent.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include "Texture2D.h"
#include "MaterialPBR.h"
#include "TerrainMeshComponent.h"
#include "StaticMeshComponent.h"
#include "ComponentSelector.h"
#include "TerrainLayer.h"
#include "SharpComponent.h"
#include "LGraph.h"
#include <qpointer.h>
#include "LogicGraphComponent.h"
#include <variant>
PropertiesEditor* PropertiesEditor::m_Instance = nullptr;

PropertiesEditor::PropertiesEditor(QWidget* parent)
    : QWidget(parent)
{
    //ui.setupUi(this);
    setAutoFillBackground(true);
    setWindowTitle("Properties");
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor("#502020"));
    setAutoFillBackground(true);
    setPalette(pal);
    setAcceptDrops(true);
    m_Instance = this;
}

PropertiesEditor::~PropertiesEditor()
{
}

QVector3D ToQV(glm::vec3 v)
{
    return QVector3D(v.x, v.y, v.z);
}
void PropertiesEditor::ClearUI() {
    if (m_contentLayout) {
        // Delete all child widgets from the content layout only
        QLayoutItem* item;
        while ((item = m_contentLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }

    // Reset widget pointers since they've been deleted
    m_currentNameProp = nullptr;
    m_currentPositionProp = nullptr;
    m_currentRotationProp = nullptr;
    m_currentScaleProp = nullptr;
}

void PropertiesEditor::UpdateNode(GraphNode* node) {

    if (!node) return;

    // Update name property
    if (m_currentNameProp) {
        // Temporarily disconnect signal to avoid triggering callbacks during update
        m_currentNameProp->blockSignals(true);
        m_currentNameProp->setText(QString::fromStdString(node->GetName()));
        m_currentNameProp->blockSignals(false);
    }

    // Update position property
    if (m_currentPositionProp) {
        m_currentPositionProp->blockSignals(true);
        m_currentPositionProp->setValue(ToQV(node->GetPosition()));
        m_currentPositionProp->blockSignals(false);
    }

    // Update rotation property
    if (m_currentRotationProp) {
        m_currentRotationProp->blockSignals(true);
        m_currentRotationProp->setValue(ToQV(node->GetEularRotation()));
        m_currentRotationProp->blockSignals(false);
    }

    // Update scale property
    if (m_currentScaleProp) {
        m_currentScaleProp->blockSignals(true);
        m_currentScaleProp->setValue(ToQV(node->GetScale()));
        m_currentScaleProp->blockSignals(false);
    }

}
std::string AddSpaces(const std::string& text) {
    if (text.empty()) return "";

    std::string result;
    result.reserve(text.size() * 2);

    // Capitalize the first character
    result += std::toupper(static_cast<unsigned char>(text[0]));

    for (size_t i = 1; i < text.size(); ++i) {
        char c = text[i];
        char prev = text[i - 1];

        // Add space before capital letters if previous is lowercase
        if (std::isupper(static_cast<unsigned char>(c)) && std::islower(static_cast<unsigned char>(prev))) {
            result += ' ';
        }

        // Add space before digits if previous is not a digit or capital letter
        if (std::isdigit(static_cast<unsigned char>(c)) &&
            !(std::isdigit(static_cast<unsigned char>(prev)) || std::isupper(static_cast<unsigned char>(prev)))) {
            result += ' ';
        }

        result += c;
    }

    return result;
}
void PropertiesEditor::SetNode(GraphNode* node) {
    if (!m_mainLayout) {
        BeginUI();
    }

    ClearUI();  //

    if (node == nullptr) return;

    AddHeader("Node");


    m_currentNameProp = AddText("Name", node->GetName().c_str(), [node](const QString& text) {
        node->SetName(text.toStdString());
        });

    m_currentPositionProp = AddVec3("Position", ToQV(node->GetPosition()), 0.5, [node](const QVector3D& value) {
        node->SetPosition(glm::vec3(value.x(), value.y(), value.z()));
        SceneView::m_Instance->AlignGizmo();
        });

    m_currentRotationProp = AddVec3("Rotation", ToQV(node->GetEularRotation()), 5.0f, [node](const QVector3D& value) {
        node->SetRotation(glm::vec3(value.x(), value.y(), value.z()));
        SceneView::m_Instance->AlignGizmo();
        });

    m_currentScaleProp = AddVec3("Scale", ToQV(node->GetScale()), 0.1, [node](const QVector3D& value) {
        node->SetScale(glm::vec3(value.x(), value.y(), value.z()));
        SceneView::m_Instance->AlignGizmo();
        });

    auto lc = node->GetComponent<LightComponent>();

    if (lc != nullptr) {

        AddHeader("Light Component");

        std::string current = "Point";

        if(lc->GetLightType() == LightType::Directional) {
            current = "Directional";
        } else if (lc->GetLightType() == LightType::Spot) {
            current = "Spot";
		}

        AddStringList("Light Type", { "Point","Directional","Spot" }, current.c_str(), [lc](QString op)
            {
                if (op == "Point") {
                    lc->SetLightType(LightType::Point);
                }
                if (op == "Directional") {
					lc->SetLightType(LightType::Directional);
                }
                if (op == "Spot") {
					lc->SetLightType(LightType::Spot);
                }



                

                });

        AddVec3("Diffuse", QVector3D(lc->GetColor().r, lc->GetColor().g, lc->GetColor().b), 0.05, [lc](const QVector3D& col)
            {
                lc->SetColor(glm::vec3(col.x(), col.y(), col.z()));
            });

        AddFloat("Range", 0, 2500, 1, lc->GetRange(), [lc](double range) {
            lc->SetRange(range);
            });

        AddFloat("Intensity", 0, 5000, 2, lc->GetIntensity(), [lc](double val) {
            lc->SetIntensity(val);
            });

    }

    AddHeader("Physics");

    QStringList bodyTypes = { "None", "Box","Convex Hull","TriMesh" };

    QString init = "Box";

    switch (node->GetBodyType()) {
    case T_Box:
        init = "Box";
        break;
    case T_TriMesh:
        init = "TriMesh";
        break;
    case T_ConvexHull:
        init = "Convex Hull";
        break;
    case T_None:
        init = "None";
        break;
    default:
        init = "Box";
        break;

    }

    AddStringList("Body Type", bodyTypes, init, [node](const QString& mode) {
        // Handle render mode change
        if (mode == "Box")
        {
            node->SetBody(BodyType::T_Box);
        }
        if (mode == "TriMesh")
        {
            node->SetBody(BodyType::T_TriMesh);
        }
        if (mode == "Convex Hull")
        {
            node->SetBody(BodyType::T_ConvexHull);
        }
        qDebug() << "Render mode changed to:" << mode;
        // node->SetRenderMode(mode.toStdString());
        });


    auto mesh = node->GetComponent<StaticMeshComponent>();

    if (mesh != nullptr) {

        int i = 0;
        for (auto m : mesh->GetSubMeshes()) {

            AddHeader((std::string("Mesh ") + std::to_string(i)).c_str());

            auto mat = m->m_Material;
            if (dynamic_cast<MaterialPBR*>(mat)) {

                auto pbr = (MaterialPBR*)mat;
                AddTexture("Color", pbr->GetColorTexture()->GetPath(), [node,pbr](std::string val)
                    {
                        pbr->SetColorTexture(new Texture2D(val));
                        pbr->Save(pbr->GetPath());
                    });
                AddTexture("Normal", pbr->GetNormalTexture()->GetPath(), [node,pbr](std::string val)
                    {
                        pbr->SetNormalTexture(new Texture2D(val));
                        pbr->Save(pbr->GetPath());
                    });
                AddTexture("Metallic", pbr->GetMetallicTexture()->GetPath(), [node,pbr](std::string val) {
                    pbr->SetMetallicTexture(new Texture2D(val));
                    pbr->Save(pbr->GetPath());
                    });
                AddTexture("Roughness", pbr->GetRoughTexture()->GetPath(), [node,pbr](std::string val) {
                    pbr->SetRoughnessTexture(new Texture2D(val));
                    pbr->Save(pbr->GetPath());
                    });
                int b = 5;
            }

            i++;
        }

    }


	auto scomp = node->GetComponents<SharpComponent>();

    for (auto sc : scomp)
    {

        auto name = "(Sharp)" + sc->GetName();
        AddHeader(name.c_str());

        for (auto v : sc->GetClass()->GetInstanceFields())
        {

            switch (v.ctype) {
            case SHARP_TYPE_VOID:
            {
                auto cls = v.etype;


                if (cls == "Vivid.Scene.GraphNode")
                {
                    auto node = sc->GetClass()->GetFieldValue<MClass*>(v.name);

                    std::string name = "None";
                    if (node != nullptr) {
                        //name = no
                        auto r = node->CallFunctionValue_String("GetName");
                        name = r;
                        int b = 5;
                    } else {

						name = "None";



                    }

                    AddNodeProperty((AddSpaces(v.name) + " (Node)").c_str(), name.c_str(), [sc, v](GraphNode* droppedNode)
                        {
                            if (droppedNode) {
                                // The callback now provides the GraphNode pointer directly.
                                // We can now pass this to the script component.
                                // For now, we'll just update the string representation,
                                // which is how script variables are currently stored.
                                //sc->SetString(var.name, droppedNode->GetName());
                                //sc->SetNode(var.name, droppedNode);



                                auto new_cls = sc->CreateGraphNode();

                                new_cls->SetNativePtr("NodePtr", droppedNode);

                                sc->GetClass()->SetFieldClass(v.name, new_cls);



                                //sc->SetClass(var.name,)



                                // A future improvement would be to have a method like:
                                // sc->SetObject(var.name, droppedNode);
                            }
                        });
                }
                int b = 5;
            }

  
            
                break;
            case SHARP_TYPE_INT:
                AddInt(AddSpaces(v.name).c_str(), -1000, 1000, 1, sc->GetClass()->GetFieldValue<int>(v.name), [sc, v](const int value)
                    {
						sc->GetClass()->SetFieldValue(v.name, value);
                        //sc->SetInt(var.name, value);


                    });
                break;
            case SHARP_TYPE_FLOAT:
                AddFloat(AddSpaces(v.name).c_str(), -1000, 1000, 0.001, sc->GetClass()->GetFieldValue<float>(v.name), [sc, v](const double value)
                    {
						sc->GetClass()->SetFieldValue(v.name, (float)value);

                    });
                break;
            case SHARP_TYPE_STRING:

                AddText(AddSpaces(v.name).c_str(), sc->GetClass()->GetFieldValue<std::string>(v.name).c_str(), [sc, v](const QString& str)
                {
                        sc->GetClass()->SetFieldValue(v.name, str.toStdString());
                });

                break;
            case SHARP_TYPE_VEC3:

                auto val = sc->GetClass()->GetFieldValue<MClass*>(v.name);

                auto vx = val->GetFieldValue<float>("x");
                auto vy = val->GetFieldValue<float>("y");
                auto vz = val->GetFieldValue<float>("z");

                AddVec3(AddSpaces(v.name).c_str(), QVector3D(vx, vy, vz), 0.05f, [sc, v](const QVector3D& v3) {

                    //auto v1 = sc->GetClass()->GetFieldValue<MClass*>(v.name);

                    float* nv = new float[3];
                    nv[0] = v3.x();
                    nv[1] = v3.y();
                    nv[2] = v3.z();

                    sc->GetClass()->SetFieldStruct(v.name, nv);

                    //v1->SetFieldValue<float>("x", v3.x());
                    //v1->SetFieldValue<float>("y", v3.y());
                    //v1->SetFieldValue<float>("z", v3.z());


                    });

                break;
            }

        }

    }


    auto scripts = node->GetComponents<ScriptComponent>();

    for (auto sc : scripts) {

        auto name = "(Py)" + sc->GetName();
        AddHeader(name.c_str());

        for (auto var : sc->GetPythonVars()) {

            switch (var.type) {
            case VT_Object:

                if (var.cls == "GraphNode")
                {
                    
                    auto node = sc->GetNode(var.name);

                    std::string name = "None";
                    if (node != nullptr) {
                       name =  node->GetName();
                    }

                    // Use the new AddNodeProperty and the new callback signature
                    AddNodeProperty((AddSpaces(var.name) + " (Node)").c_str(),name.c_str(), [sc, var](GraphNode* droppedNode)
                        {
                            if (droppedNode) {
                                // The callback now provides the GraphNode pointer directly.
                                // We can now pass this to the script component.
                                // For now, we'll just update the string representation,
                                // which is how script variables are currently stored.
                                //sc->SetString(var.name, droppedNode->GetName());
                                sc->SetNode(var.name, droppedNode);

                                
                                
                                //sc->SetClass(var.name,)



                                // A future improvement would be to have a method like:
                                // sc->SetObject(var.name, droppedNode);
                            }
                        });
                }

                break;
            case VT_Int:
                AddInt(AddSpaces(var.name).c_str(), -1000, 1000, 1, sc->GetInt(var.name), [sc, var](const int value)
                    {
                        sc->SetInt(var.name, value);

                    });
                break;
            case VT_Float:

                AddFloat(AddSpaces(var.name).c_str(), -1000, 1000, 0.001, sc->GetFloat(var.name), [sc, var](const double value)
                    {
                        sc->SetFloat(var.name, value);

                    });
                break;
            case VT_String:

                AddText(AddSpaces(var.name).c_str(), sc->GetString(var.name).c_str(), [sc, var](const QString& value)
                    {

                        sc->SetString(var.name, value.toStdString());

                    });

                break;
            }

        }

    }

    auto graphs = node->GetComponents<LogicGraphComponent>();

    for (auto g : graphs) {
        auto name = "(Graph)" + g->GetName();
        AddHeader(name.c_str());
    
        for (auto v : g->GetVars()) {

            switch (v->GetType()) {
            case DataType::Vec3:
            {
                glm::vec3 dv = glm::vec3(0, 0, 0);
                if (const glm::vec3* value_ptr = std::get_if<glm::vec3>(&v->GetDefaultValue())) {
                    // The variant holds a float. We can safely access it.
                    dv = *value_ptr;

                }

                AddVec3(AddSpaces(v->GetName()).c_str(), QVector3D(dv.x, dv.y, dv.z), 1, [v, g](QVector3D v3)
                    {
                        v->SetDefaultValue(glm::vec3(v3.x(), v3.y(), v3.z()));
                    });
            }
                break;
            case DataType::Float:
            {
                float dv = 0;
                if (const float* value_ptr = std::get_if<float>(&v->GetDefaultValue())) {
                    // The variant holds a float. We can safely access it.
                    dv = *value_ptr;

                }
                AddFloat(AddSpaces(v->GetName()).c_str(), -10000, 10000, 1, (float)dv, [v, g](double val) {

                    v->SetDefaultValue(val);

                    });
            }
                break;
            case DataType::GraphNodeRef:
            {
                AddNodeProperty((AddSpaces(v->GetName()) + " (Node)").c_str(), name.c_str(), [v, g](GraphNode* droppedNode)
                    {
                        if (droppedNode) {
                            v->SetDefaultValue(droppedNode);
                        }
                    });
            }
                break;
            }

        }

    }

    AddButton("Add Component", [node, this]() {
        qDebug() << "Add Component button clicked";

        // Get or create the component selector
        ComponentSelector* selector = ComponentSelector::getInstance(this);

        if (!selector) {
            qDebug() << "Failed to create ComponentSelector";
            return;
        }

        // Clear any existing categories first
        selector->clearCategories();
        qDebug() << "Cleared existing categories";

        // Add categories and their component types
        QStringList sharpComponents;
        auto classes = QEngine::m_ComponentClasses;

        qDebug() << "Available component classes:" << classes.size();

        for (auto c : classes) {
            QString componentName = QString::fromStdString(c.className);
            sharpComponents.append(componentName);
            qDebug() << "Added component:" << componentName;
        }

        // DEBUGGING: Ensure we have components to add
        if (sharpComponents.isEmpty()) {
            qDebug() << "Warning: No Sharp Components found!";
            // Add some test components for debugging
            sharpComponents << "Test Component 1" << "Test Component 2" << "Test Component 3";
        }

        selector->addCategory("Sharp Components", sharpComponents);
        selector->addCategory("Rendering", { "Static Mesh Component", "Terrain Mesh Component", "Particle System" });
        selector->addCategory("Lighting", { "Light Component", "Environment Light", "Spot Light" });
        selector->addCategory("Physics", { "Rigid Body", "Collider", "Kinematic Body" });

        qDebug() << "Added all categories";

        // CRITICAL FIX: Use QPointer to safely check if PropertiesEditor still exists
        // Also use Qt::QueuedConnection to ensure the call is made safely
        QPointer<PropertiesEditor> safeThis(this);

        connect(selector, &ComponentSelector::itemSelected, selector,
            [safeThis, node](const QString& category, const QString& componentType) {
                qDebug() << "Selected component:" << componentType << "from category:" << category;

                // SAFETY CHECK: Ensure PropertiesEditor still exists
                if (!safeThis) {
                    qDebug() << "PropertiesEditor was destroyed, ignoring signal";
                    return;
                }

                // SAFETY CHECK: Ensure node still exists (you might want to use QPointer for node too)
                if (!node) {
                    qDebug() << "Node is null, ignoring signal";
                    return;
                }

                if (category == "Sharp Components") {
                    // Handle Sharp Components
                    qDebug() << "Creating Sharp Component:" << componentType;
                    auto classes = QEngine::m_ComponentClasses;
                    for (auto c : classes) {
                        if (c.className == componentType.toStdString()) {
                            SharpComponent* comp = new SharpComponent;
                            node->AddComponent(comp);
                            auto cls = QEngine::m_MonoLib->GetClass(c.className);
                            comp->SetClass(cls, QEngine::m_MonoLib->GetAssembly(),QEngine::m_MonoLib->GetVivid());
                            comp->SetName(c.className);
                            comp->SetTime(QEngine::s_last_dll_write_time);
                            // Refresh the properties editor - use the safe pointer
                            safeThis->SetNode(node);
                            break;
                        }
                    }
                }
                else {
                    qDebug() << "Creating component from category:" << category;
                    // Handle other component types here
                    if (componentType == "Script Component") {
                        // Create script component
                    }
                    else if (componentType == "Light Component") {
                        // Create light component
                    }
                }
            }, Qt::QueuedConnection); // Use queued connection for safety

        qDebug() << "Connected itemSelected signal";

        // Show the selector near the mouse
        QPoint mousePos = QCursor::pos();
        qDebug() << "Showing selector at mouse position:" << mousePos;

        selector->showAt(mousePos);

        // DEBUGGING: Check if selector is actually visible
        QTimer::singleShot(100, [selector]() {
            if (selector) {
                qDebug() << "Selector visibility after 100ms:" << selector->isVisible();
                qDebug() << "Selector position:" << selector->pos();
                qDebug() << "Selector size:" << selector->size();
            }
            });
        });

    EndUI();
    m_Node = node;
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

// Modify the Add methods to return pointers to the created widgets:
PropertyText* PropertiesEditor::AddText(const QString& label, const QString& defaultText,
    std::function<void(const QString&)> callback)
{
    PropertyText* textProp = new PropertyText(label, defaultText);
    m_contentLayout->addWidget(textProp);

    if (callback) {
        connect(textProp, &PropertyText::textChanged, this, [callback](const QString& text) {
            callback(text);
            });
    }

    return textProp;
}

PropertyNode* PropertiesEditor::AddNodeProperty(const QString& label, const QString& defaultText,
    std::function<void(GraphNode*)> callback)
{
    PropertyNode* nodeProp = new PropertyNode(label, defaultText);
    m_contentLayout->addWidget(nodeProp);

    if (callback) {
        // Connect to the new signal that passes the GraphNode pointer
        connect(nodeProp, &PropertyNode::nodeDropped, this, [callback](GraphNode* node) {
            callback(node);
            });
    }

    return nodeProp;
}


PropertyVec3* PropertiesEditor::AddVec3(const QString& label, const QVector3D& defaultValue, double interval,
    std::function<void(const QVector3D&)> callback)
{
    PropertyVec3* vec3Prop = new PropertyVec3(label, nullptr);
    vec3Prop->setValue(defaultValue);
    vec3Prop->setInterval(interval);
    m_contentLayout->addWidget(vec3Prop);

    if (callback) {
        connect(vec3Prop, &PropertyVec3::valueChanged, this, [callback](const QVector3D& value) {
            callback(value);
            });
    }

    return vec3Prop;
}


PropertyInt* PropertiesEditor::AddInt(const QString& label, int minValue, int maxValue,
    int interval, int defaultValue,
    std::function<void(int)> callback)
{
    PropertyInt* intProp = new PropertyInt(label, minValue, maxValue, interval);
    intProp->setValue(defaultValue);
    m_contentLayout->addWidget(intProp);

    if (callback) {
        connect(intProp, &PropertyInt::valueChanged, this, [callback](int value) {
            callback(value);
            });
    }

    return intProp;
}


PropertyFloat* PropertiesEditor::AddFloat(const QString& label, double minValue, double maxValue,
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

    return floatProp;
}

PropertySlider* PropertiesEditor::AddSlider(const QString& label, int minValue, int maxValue,
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

    return sliderProp;
}

PropertyStringList* PropertiesEditor::AddStringList(const QString& label, const QStringList& options,
    const QString& defaultValue, std::function<void(const QString&)> callback)
{
    PropertyStringList* stringListProp = new PropertyStringList(label, options);

    if (!defaultValue.isEmpty()) {
        stringListProp->setCurrentText(defaultValue);
    }

    m_contentLayout->addWidget(stringListProp);

    if (callback) {
        connect(stringListProp, &PropertyStringList::valueChanged, this, [callback](const QString& value) {
            callback(value);
            });
    }

    return stringListProp;
}

void PropertiesEditor::dragEnterEvent(QDragEnterEvent* event)
{
    // Check if the dragged data contains text. In your Content widget,
    // you set the filename as text data.
    if (event->mimeData()->hasText()) {
        // Accept the drag event, which allows a drop to occur.
        event->acceptProposedAction();
    }
}

void PropertiesEditor::dropEvent(QDropEvent* event)
{
    // Check again for text data and process the drop.
    if (event->mimeData()->hasText()) {
        QString resourceName = event->mimeData()->text();

        // Call your new method with the filename.
        AddedFromDrop(resourceName);

        event->acceptProposedAction();
    }
}

std::string getBaseName(const std::string& fullPath) {
    // Find the position of the last directory separator
    size_t lastSlash = fullPath.find_last_of('/');
    size_t lastBackslash = fullPath.find_last_of('\\');
    size_t separatorPos = std::string::npos;

    // Determine the actual last separator's position
    if (lastSlash != std::string::npos && lastBackslash != std::string::npos) {
        separatorPos = std::max(lastSlash, lastBackslash);
    }
    else if (lastSlash != std::string::npos) {
        separatorPos = lastSlash;
    }
    else {
        separatorPos = lastBackslash;
    }

    // Get the substring containing just the filename (after the last separator)
    std::string filename = (separatorPos == std::string::npos)
        ? fullPath
        : fullPath.substr(separatorPos + 1);

    // Find the position of the last dot (the extension) in the filename
    size_t dotPos = filename.find_last_of('.');

    // If there is no dot, or if the dot is the first character (e.g., ".config"),
    // return the entire filename, as it has no extension to remove.
    if (dotPos == std::string::npos || dotPos == 0) {
        return filename;
    }

    // Return the substring before the last dot
    return filename.substr(0, dotPos);
}

std::string getFileExtension(const std::string& filePath) {
    // Find the position of the last dot.
    size_t dotPos = filePath.find_last_of('.');

    // Check for edge cases:
    // 1. No dot was found.
    // 2. The dot is the last character in the path (e.g., "folder/.").
    // 3. The dot is part of a directory name like "./" or "../"
    if (dotPos == std::string::npos || dotPos == filePath.length() - 1) {
        return ""; // No valid extension found
    }

    // Check if the character after the dot is a path separator
    if (filePath[dotPos + 1] == '/' || filePath[dotPos + 1] == '\\') {
        return ""; // Not a file extension
    }


    // Return the substring from the character after the dot to the end.
    return filePath.substr(dotPos + 1);
}

void PropertiesEditor::AddedFromDrop(const QString& resourceName)
{
    // This is the empty method for you to implement.
    // For now, a debug message confirms it's being called correctly.
    qDebug() << "Resource dropped onto Properties Editor:" << resourceName;
    if (m_Node == nullptr) return;

    auto path = resourceName.toStdString();

    auto name = getBaseName(resourceName.toStdString());

    auto ext = getFileExtension(path);


    if (ext == "py") {
        auto comp = new ScriptComponent();
        m_Node->AddComponent(comp);
        comp->SetScript(path, name);
        SetNode(m_Node);
    }
    if (m_Node) {
        if (ext == "graph") {

            LGraph* tg = new LGraph("Graph");

            tg = tg->LoadFromFile(resourceName.toStdString(), QEngine::GetNodeRegistry());
            LogicGraphComponent* lg = new LogicGraphComponent;
            m_Node->AddComponent(lg);
            lg->SetGraph(tg);

           
            SetNode(m_Node);

        }
    }
    int a = 5;

    // You can now process the dropped resource, for example:
    // if (m_selectedNode && resourceName.endsWith(".py")) {
    //     m_selectedNode->AddComponent(new ScriptComponent(resourceName));
    //     SetNode(m_selectedNode); // Refresh the UI
    // }
}


PropertyTexture* PropertiesEditor::AddTexture(const QString& label, const std::string& defaultPath,
    std::function<void(const std::string&)> callback)
{
    PropertyTexture* textureProp = new PropertyTexture(label);

    if (!defaultPath.empty()) {
        textureProp->setTexturePath(defaultPath);
    }

    m_contentLayout->addWidget(textureProp);

    if (callback) {
        connect(textureProp, &PropertyTexture::textureChanged, this, [callback](const std::string& path) {
            callback(path);
            });
    }

    return textureProp;
}

void PropertiesEditor::SetMaterial(MaterialPBR* mat)
{
    if (!m_mainLayout) {
        BeginUI();
    }

    ClearUI();  //

    AddHeader(std::string("Material:" + mat->GetName()).c_str());
        
    m_Material = mat;
   
    if (dynamic_cast<MaterialPBR*>(mat)) {

        auto pbr = (MaterialPBR*)mat;
        AddTexture("Color", pbr->GetColorTexture()->GetPath(), [pbr](std::string val)
            {
                pbr->SetColorTexture(new Texture2D(val));
                pbr->Save(pbr->GetPath());
            });
        AddTexture("Normal", pbr->GetNormalTexture()->GetPath(), [pbr](std::string val)
            {
                pbr->SetNormalTexture(new Texture2D(val));
                pbr->Save(pbr->GetPath());
            });
        AddTexture("Metallic", pbr->GetMetallicTexture()->GetPath(), [pbr](std::string val) {
            pbr->SetMetallicTexture(new Texture2D(val));
            pbr->Save(pbr->GetPath());
            });
        AddTexture("Roughness", pbr->GetRoughTexture()->GetPath(), [pbr](std::string val) {
            pbr->SetRoughnessTexture(new Texture2D(val));
            pbr->Save(pbr->GetPath());
            });
        int b = 5;
    }


    EndUI();
    m_Material = mat;
}

void PropertiesEditor::SetTerrain(GraphNode* node) {
    if (!m_mainLayout) {
        BeginUI();
    }

    ClearUI();  //

    auto tmesh = node->GetComponent<TerrainMeshComponent>();

    AddHeader("Terrain");

    AddInt("Edit Layer", 0, tmesh->GetLayers().size(), 1, 0, [](int v) {

        SceneView::m_Instance->SetEditLayer(v);

        });

    AddStringList("Brush Mode",{"Add","Subtract","Direct"},"Add",[](QString mode) {
        if (mode == "Add") {
            SceneView::m_Instance->SetTerrainBrushMode(EditBrushMode::BM_Add);
        } else if (mode == "Subtract") {
            SceneView::m_Instance->SetTerrainBrushMode(EditBrushMode::BM_Subtract);
        } else if (mode == "Direct") {
            SceneView::m_Instance->SetTerrainBrushMode(EditBrushMode::BM_Direct);
        }
		});

    AddFloat("Brush Size",0.01,10.0f,0.1,SceneView::m_Instance->GetTerrainBrushSize(), [](double v) {
        SceneView::m_Instance->SetTerrainBrushSize(v);
		});

    AddFloat("Brush Strength", 0.01, 15.0f, 0.01, SceneView::m_Instance->GetTerrainStrength(), [](double v) {
        SceneView::m_Instance->SetTerrainStrength(v);
        });


 

    auto layers = tmesh->GetLayers();

    int ix = 0;
    for (auto layer : layers) {

        std::string head = "Layer " + std::to_string(ix);

        AddHeader(head.c_str());

        AddTexture("Color", layer->GetColor()->GetPath(), [layer](std::string v) {
            layer->SetColor(new Texture2D(v));
            });
        AddTexture("Normal", layer->GetNormal()->GetPath(), [layer](std::string v) {
            layer->SetNormal(new Texture2D(v));
            });
        AddTexture("Specular", layer->GetSpec()->GetPath(), [layer](std::string v) {
            layer->SetSpecular(new Texture2D(v));
            });
        ix++;
    }

    EndUI();
    m_Node = node;

}

PropertyButton* PropertiesEditor::AddButton(const QString& buttonText,
    std::function<void()> callback)
{
    PropertyButton* buttonProp = new PropertyButton(buttonText);
    m_contentLayout->addWidget(buttonProp);

    if (callback) {
        connect(buttonProp, &PropertyButton::buttonClicked, this, [callback]() {
            callback();
            });
    }

    return buttonProp;
}