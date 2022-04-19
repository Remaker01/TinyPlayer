#include "playerwindow.h"
#include <QSharedMemory>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QSharedMemory shared("player");
    if(shared.create(1)) {
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
    return 0;
}
