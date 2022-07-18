#include "onlineseacher.h"
#include <QDebug>
OnlineSeacher::OnlineSeacher(QObject *parent) : QObject(parent){
    p.setProgram("net_music.exe");
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
        info.url = now[2];
        result.append(info);
    }
    f.remove();
    return result;
}

void OnlineSeacher::setKeyWord(const QString &kwd) {keyword = kwd;}

bool OnlineSeacher::doSearch() {
    p.setArguments(QStringList(keyword));
    p.start();
    //这个信号可能多次发出，可在第一次响应后删除文件，并判断文件是否存在即可
    connect(&p,QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),this,&OnlineSeacher::done);
    return true;
}
