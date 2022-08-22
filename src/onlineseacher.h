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
    QProcess p;
public:
    static const QString PROGRAM;
    explicit OnlineSeacher(QObject *parent = nullptr);
    OnlineSeacher(const QString &kwd,QObject *parent = nullptr);
    void setKeyWord(const QString &kwd);
    /**
     * 分析结果，返回搜索结果的列表
     * @note 必须在done()信号调用后使用
     */
    QList<ResultInfo> analyzeResult();
    void doSearch();
signals:
    /// doSearch()结束后调用
    void done();
};

#endif // ONLINESEACHER_H
