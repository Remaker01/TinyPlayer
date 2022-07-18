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
    explicit OnlineSeacher(QObject *parent = nullptr);
    OnlineSeacher(const QString &kwd,QObject *parent = nullptr);
    void setKeyWord(const QString &kwd);
    QList<ResultInfo> analyzeResult();
    bool doSearch();
signals:
    void done();
};

#endif // ONLINESEACHER_H
