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
    connectSlots();
}

OnlineSeacher::OnlineSeacher(const QString &kwd,QObject *parent):OnlineSeacher(parent) {
    keyword = kwd;
}

inline void OnlineSeacher::connectSlots() {
    connect(&prog,QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),this,&OnlineSeacher::done);
    connect(&down_prog,QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),this,&OnlineSeacher::downloaded);
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
        QStringList now = line.split(u8"\u00a0");
        info.title = now[0]/*.replace("下载","").replace("mp3","")*/;//防止"下载","MP3"等关键字出现异常
        info.artist = now[1];
        info.url = now[2].replace('\r',"").replace('\n',"").replace("&amp;","&");
        result.append(info);
    }
    f.remove();
    return result;
}

void OnlineSeacher::setKeyWord(const QString &kwd) {keyword = kwd;}

void OnlineSeacher::doSearch(int method) {
    if(method < 0||method > 3) {
#ifndef NDEBUG
        qCritical() << "doSearch:invalid method.";
#endif
        emit done();
        return;
    }
    if(!QFile::exists(PROGRAM)) {
        emit done();
        QMessageBox::critical(nullptr,"出错了!","找不到或无法正确加载搜索引擎");
        return;
    }
    QString methodStrs[] = {"0","1","2","3"};
    prog.setArguments({methodStrs[method],keyword});
    prog.start();
}

void OnlineSeacher::download(const QList<QUrl> &uri, const QString &path, const QStringList &names) {
    if(!QFile::exists(DOWN_PROGRAM)) {
        emit downloaded();
        QMessageBox::critical(nullptr,"出错了!","找不到或无法正确加载下载器");
        return;
    }
    QStringList args;args.reserve(names.length());
    for(int i = 0; i < names.length(); i++) {
        QString suffix = uri[i].url().endsWith(".mp3",Qt::CaseInsensitive) ? ".mp3" : ".m4a";
#ifndef NDEBUG
        qDebug() << suffix;
#endif
        args.append(uri[i].url()+';'+names[i]); // toString:QUrl(...)
        if(!args.last().endsWith(suffix,Qt::CaseInsensitive))
            args.last().append(suffix);
    }
    args.prepend(path);
    down_prog.setArguments(args);
    down_prog.start();
}

OnlineSeacher::~OnlineSeacher() {
    if(prog.state() == QProcess::Running)
        prog.kill();
    if(down_prog.state() == QProcess::Running)
        down_prog.kill();
    prog.close();
    down_prog.close();
}
