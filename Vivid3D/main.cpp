#include "Vivid3D.h"
#include <QtWidgets/QApplication>
#include "GeneralInput.h"
#include "qfile.h"
#include "qstyle.h"
#include "qstylefactory.h"
QString loadStyleSheet(const QString& fileName)
{
    QFile styleFile(fileName);
    styleFile.open(QFile::ReadOnly | QFile::Text);
    QTextStream styleStream(&styleFile);
    return styleStream.readAll();
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList styles = QStyleFactory::keys();
    for (const QString& style : styles) {
        qDebug() << style;
    }

    //a.setStyle(QStyleFactory::create("Windows"));


    Vivid3D w;
    w.show();
    return a.exec();
}
