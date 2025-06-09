#include "Vivid3D.h"
#include <QtWidgets/QApplication>
#include <QSplashScreen>
#include <QTimer>
#include <QPixmap>
#include <QLabel>
#include <QMovie>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QTabWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QScrollBar>
#include <QStyleFactory>



int main(int argc, char *argv[])
{

    QApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, false);
    QApplication app(argc, argv);

    app.setStyleSheet(R"(
        QMenu {
            background-color: #2b2b2b;
            color: white;
        }

        QMenu::item {
            padding: 5px 20px;
            background-color: transparent;
        }

        QMenu::item:selected {
            background-color: #606060;
        }
    )");

    QPixmap pixmap("edit/splash/vivid3d.jpg"); // Use a QRC or disk image
    QSplashScreen splash(pixmap);
    splash.show();

    // Optional message overlay
    splash.showMessage("Loading Vivid3D resources...", Qt::AlignBottom | Qt::AlignLeft, Qt::white);

    // Simulate loading delay or do actual init work here
    QTimer::singleShot(50, &splash, &QSplashScreen::close); // Close splash after 2 sec


    Vivid3D window;

    QTimer::singleShot(50, [&window]() {
        window.show();
        });


    //window.show();
    return app.exec();
}
