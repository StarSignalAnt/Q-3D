#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Vivid3D.h"
#include "SceneView.h"
#include "PropertiesEditor.h"

class Vivid3D : public QMainWindow
{
    Q_OBJECT

public:
    Vivid3D(QWidget *parent = nullptr);
    ~Vivid3D();

private:
    Ui::Vivid3DClass ui;
};

