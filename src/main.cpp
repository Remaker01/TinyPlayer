/*
 * TODO:
 * 1.支持线上音乐搜索&播放
 * 2. 用QStackedWidget在两个播放列表间切换
 *     1. 本地播放列表，即原来的播放列表
 *     2. 线上音乐播放列表
 *
 */
#include "playerwindow.h"
#include <QSharedMemory>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QSharedMemory shared("player");
    QFont font("幼圆",9);
    if(shared.create(1)) {
        a.setFont(font);
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
    else
        QMessageBox::warning(nullptr,"警告","TinyPlayer已在运行");
    return 0;
}
