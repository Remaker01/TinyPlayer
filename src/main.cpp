/*
 * BugFix:
 * 1. 部分文件暂停后进度条归0
 * 2. 打开文件目录不正确
 * 3. 关闭“关于”窗口后自动弹出网页
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

