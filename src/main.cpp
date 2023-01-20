#include "playerwindow.h"
#include <QSharedMemory>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QSharedMemory shared("player");
    QFont font("幼圆",9);
    if(shared.create(1)) {
        if(!font.exactMatch())
            QMessageBox::warning(nullptr,"提示","字体缺失，请下载幼圆字体");
        QDir::setCurrent(a.applicationDirPath());
        QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,a.applicationDirPath());
        QStringList args = a.arguments();
        PlayerWindow w(argc>1?args[1]:"");
        if(!w.isVisible())
            w.show();
        return a.exec();
    }
    else
        QMessageBox::warning(nullptr,"温馨提示","TinyPlayer已在运行");
    return 0;
}
