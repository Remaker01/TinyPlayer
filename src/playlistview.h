#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H
#include <QListView>
#include <QUrl>
#include <QDropEvent>
#include <QMimeData>
#include <QContextMenuEvent>
#include <QMenu>
class PlayListView : public QListView
{
    Q_OBJECT
private:
    QAction *del;
public:
    PlayListView(QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
    void contextMenuEvent ( QContextMenuEvent *e);
    QModelIndexList getSelections();
signals:
    void mediaDropin(const QStringList &medias);
    void itemDelRequirement();
    void openDirRequirement(int row);
};

#endif // PLAYLISTVIEW_H
