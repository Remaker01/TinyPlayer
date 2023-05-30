#ifndef SEARCHRESULTWIDGET_H
#define SEARCHRESULTWIDGET_H
#define __WIDGET_NEEDED__
#include <set>
#include "onlineseacher.h"
#include "ui_searchresultwidget.h"
namespace Ui {class SearchResultWidget;}

class SearchResultWidget : public QWidget{
    Q_OBJECT
public:
    explicit SearchResultWidget(QWidget *parent = nullptr);
    /// 设置条目，原有条目将被清除
    void setItems(const QList<ResultInfo> &results);
    /// 获取特定行的条目
    ResultInfo getItem(int row);
    /// 获取选中的条目
    QList<ResultInfo> getSelectedItems();
    Q_DECL_DEPRECATED_X("Use getSelectedItems instead") QList<QString> getSelectedURLs();
    void removeSelected();
    ~SearchResultWidget();
protected:
    void closeEvent(QCloseEvent *event);
signals:
    void addItemRequirement(bool autoDelete);
    void changPageRequirement(uint page);
private slots:
    void on_tableWidget_cellClicked(int row, int column);
    void on_copyButton_clicked();
    void on_nextButton_clicked();
    void on_prevButton_clicked();

private:
    Ui::SearchResultWidget *ui;
    QMovie *gif;
    inline uint getPage();
    inline void setPage(uint page);
};

#endif // SEARCHRESULTWIDGET_H
