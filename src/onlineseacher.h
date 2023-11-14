#ifndef ONLINESEACHER_H
#define ONLINESEACHER_H
#include "global_include.h"
#include <QMessageBox>
/**
 * @brief The ResultInfo struct:
 * 结果信息结构体，在SearchResultWidget中添加的条目必须符合此结构体格式
 */
struct ResultInfo {
    QString title,artist,url;
    ResultInfo() = default;
    ResultInfo(const QString &title,const QString artist,const QString uri) {
        this->title = title;
        this->artist = artist;
        url = uri;
    }
};
class OnlineSeacher : public QObject {
    Q_OBJECT
private:
    QString keyword;
    QProcess prog,down_prog;
    void connectSlots();
	static const char *getSuffix(const QString &uri);
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
	/// 执行搜索动作。此函数是异步的。
    void doSearch(int method,uint page = 1);
	/// 执行下载动作。此函数是异步的。
	void download(const QList<QUrl> &uri, const QString &path, const QList<QString> &names);
	//以后新增搜索引擎使用
    virtual ~OnlineSeacher();
signals:
    /// doSearch()结束后触发
    void done();
    /// download()结束后触发
    void downloaded();
};

#endif // ONLINESEACHER_H
