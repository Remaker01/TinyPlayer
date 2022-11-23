#ifndef ONLINESEACHER_H
#define ONLINESEACHER_H

#include "searchresultwidget.h"
class OnlineSeacher : public QObject {
    Q_OBJECT
private:
    QString keyword;
    QProcess prog,down_prog;
public:
    /// 搜索程序与下载程序的名称
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
    /// doSearch()结束后触发
    void done();
    /// download()结束后触发
    void downloaded();
};

#endif // ONLINESEACHER_H
