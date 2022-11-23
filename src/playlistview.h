#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H
#define __WIDGET_NEEDED__
#include "global_include.h"
class PlayListView : public QListView {
    Q_OBJECT
private:
    QAction *del,*open,*showDetail;
    QMenu *menu;
    QStringListModel *model;
    QStringList playList;
    void initMenu();
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
public:
    explicit PlayListView(QWidget *parent = nullptr);
    void contextMenuEvent ( QContextMenuEvent *e);
    QModelIndexList getSelections();
//    void setOpacity(double value);
    /// 提交修改。在对播放列表进行操作后调用
    void commitChange();
    QStringList &list();
    ~PlayListView();
signals:
    void mediaDropin(const QStringList &medias);
    void itemDelRequirement();
    void openRequirement(int row);
    void downloadRequirement(const QModelIndexList &medias);
    void showDetailRequirement(int row);
};

#endif // PLAYLISTVIEW_H
