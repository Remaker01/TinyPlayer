#include "onlineseacher.h"
#include <QDebug>
#ifdef Q_OS_WIN
const QString OnlineSeacher::PROGRAM = "net_music.exe";
const QString OnlineSeacher::DOWN_PROGRAM = "down.exe";
#elif defined Q_OS_MACOS
const QString OnlineSeacher::PROGRAM = "net_music.app";
const QString OnlineSeacher::DOWN_PROGRAM = "down.app";
#else
const QString OnlineSeacher::PROGRAM = "net_music";
const QString OnlineSeacher::DOWN_PROGRAM = "down";
#endif
OnlineSeacher::OnlineSeacher(QObject *parent) : QObject(parent){
    prog.setProgram(PROGRAM);
    down_prog.setProgram(DOWN_PROGRAM);
}

OnlineSeacher::OnlineSeacher(const QString &kwd,QObject *parent):OnlineSeacher(parent) {
    keyword = kwd;
}
//注意不能在doSearch中直接调用这个函数
QList<ResultInfo> OnlineSeacher::analyzeResult() {
    QFile f("links.tmp");
    QList<ResultInfo> result;
    if(!f.open(QIODevice::ReadWrite))
        return result;
    ResultInfo info;
    while (!f.atEnd()) {
        QString line = f.readLine();
        QStringList now = line.split(";");
        info.title = now[0].replace("下载","").replace("mp3","");
        info.artist = now[1];
        info.url = now[2].replace('\r',"").replace('\n',"").replace("&amp;","&");
        result.append(info);
    }
    f.remove();
    return result;
}

void OnlineSeacher::setKeyWord(const QString &kwd) {keyword = kwd;}

void OnlineSeacher::doSearch(int method) {
    assert(method == 1||method == 2);
    if(!QFile::exists(PROGRAM)) {
        emit done();
        QMessageBox::critical(nullptr,"错误","找不到执行搜索需要的程序");
        return;
    }
    QString methodStr = (method == 1) ? "1" : "2";
    prog.setArguments(QStringList({methodStr,keyword}));
    prog.start();
    //这个信号可能多次发出，可在第一次响应后删除文件，并判断文件是否存在即可
    connect(&prog,QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),this,&OnlineSeacher::done);
}

void OnlineSeacher::download(QStringList uri, const QString &path, const QStringList &names) {
    if(!QFile::exists(DOWN_PROGRAM)) {
        emit downloaded();
        QMessageBox::critical(nullptr,"错误","找不到下载器");
        return;
    }
    for(int i = 0; i < names.length(); i++) {
        uri[i] += ';';
        uri[i] += names[i];
        if(!uri[i].endsWith(".mp3",Qt::CaseInsensitive))
            uri[i].append(".mp3");
    }
    uri.prepend(path);
    down_prog.setArguments(uri);
    down_prog.start();
    connect(&down_prog,QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),this,&OnlineSeacher::downloaded);
}
