#include "Vivid3D.h"
#include <QtWidgets/QApplication>
#include <QSplashScreen>
#include <QTimer>
#include <QPixmap>
#include <QLabel>
#include <QMovie>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);



    QPixmap pixmap("edit/splash/vivid3d.png"); // Use a QRC or disk image
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
