#include "playerwindow.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QFile qss(":/Icons/images/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)) {
        QString style = qss.readAll();
        a.setStyleSheet(style);
        qss.close();
    }
    PlayerWindow w;
    w.show();
    return a.exec();
}
