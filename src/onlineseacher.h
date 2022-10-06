#ifndef ONLINESEACHER_H
#define ONLINESEACHER_H

#include <QMessageBox>
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
    void doSearch(int method = 1);
    void download(QStringList uri, const QString &path, const QStringList &names);
signals:
    /// doSearch()结束后调用
    void done();
    void downloaded();
};

#endif // ONLINESEACHER_H
