#include "playerwindow.h"

#include <QApplication>
#include <QLocale>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    PlayerWindow w;
    w.show();
    return a.exec();
}
