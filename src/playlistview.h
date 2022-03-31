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
public:
    PlayListView(QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
signals:
    void mediaDropin(const QStringList &medias);
};

#endif // PLAYLISTVIEW_H
