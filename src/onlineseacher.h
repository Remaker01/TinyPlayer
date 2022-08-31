#ifndef ONLINESEACHER_H
#define ONLINESEACHER_H

#include <QObject>
#include <QProcess>
#include <QFile>
#include "searchresultwidget.h"
class OnlineSeacher : public QObject {
    Q_OBJECT
private:
    QString keyword;
    QProcess prog,down_prog;
public:
    static const QString PROGRAM,DOWN_PROGRAM;
    explicit OnlineSeacher(QObject *parent = nullptr);
    OnlineSeacher(const QString &kwd,QObject *parent = nullptr);
    void setKeyWord(const QString &kwd);
    /**
     * 分析结果，返回搜索结果的列表
     * @note 必须在done()信号调用后使用
     */
    QList<ResultInfo> analyzeResult();
    void doSearch();
    void download(QStringList uri,const QString &path = QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
signals:
    /// doSearch()结束后调用
    void done();
    void downloaded();
};

#endif // ONLINESEACHER_H
