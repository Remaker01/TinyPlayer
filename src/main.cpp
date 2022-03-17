#include "playerwindow.h"

#include <QApplication>
#include <QLocale>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QFile qss(":/Icons/images/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)) {
        QString style = qss.readAll();
        a.setStyleSheet(style);
        qss.close();
    }
    else {
        QMessageBox::critical(nullptr,"错误","样式加载失败");
    }
    PlayerWindow w;
    w.show();
    return a.exec();
}
