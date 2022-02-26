#include "playerwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TinyPlayer_" + QLocale(locale).name();
        if (translator.load(":/qm/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    PlayerWindow w;
    w.show();
    return a.exec();
}
