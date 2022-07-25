#ifndef SEARCHRESULTWIDGET_H
#define SEARCHRESULTWIDGET_H

#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include "ui_searchresultwidget.h"
namespace Ui {class SearchResultWidget;}
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
class SearchResultWidget : public QWidget{
    Q_OBJECT
public:
    explicit SearchResultWidget(QWidget *parent = nullptr);
    void setItems(QList<ResultInfo> results);
    ResultInfo getItem(int row);
    QList<ResultInfo> getSelectedItems();
    QList<QString> getSelectedURLs();
    void removeSelected();
    ~SearchResultWidget();
signals:
    void addItemRequirement();
private slots:
    void on_tableWidget_cellClicked(int row, int column);

private:
    Ui::SearchResultWidget *ui;
};

#endif // SEARCHRESULTWIDGET_H
