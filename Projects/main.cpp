#include "Projects.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Projects window;
    window.show();
    return app.exec();
}
