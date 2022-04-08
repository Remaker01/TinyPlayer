#include "playlistview.h"
#ifndef NDEBUG
#include <QDebug>
#endif
PlayListView::PlayListView(QWidget *parent) {
    setParent(parent);
    setAcceptDrops(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PlayListView::dragEnterEvent(QDragEnterEvent *e) {e->accept();}

void PlayListView::dragMoveEvent(QDragMoveEvent *e) {
    e->setDropAction(Qt::MoveAction);
    e->accept();
}

void PlayListView::dropEvent(QDropEvent *e) {
    //获取拖拽进的文件带路径的文件名
    QList<QUrl> urls(e->mimeData()->urls());
    QStringList medias(QUrl::toStringList(urls,QUrl::PreferLocalFile));
    std::sort(medias.begin(),medias.end());
    emit mediaDropin(medias);
    e->accept();
    QListView::dropEvent(e);
}

void PlayListView::contextMenuEvent(QContextMenuEvent *e) {
    QModelIndexList tmp = QListView::selectedIndexes();
    if(!tmp.empty()) {
        QMenu menu(this);
        del = menu.addAction("删除");
        connect(del,&QAction::triggered,this,&PlayListView::itemDelRequirement);
        if(tmp.size() == 1) {
            QAction *open = menu.addAction("打开目录"),*showDetail = menu.addAction("详细信息");
            connect(open,&QAction::triggered,this,[&,this]() {
               emit openDirRequirement(tmp[0].row());
            });
            connect(showDetail,&QAction::triggered,this,[&,this](){
               emit showDetailRequirement(tmp[0].row());
            });
            open->deleteLater();
            showDetail->deleteLater();
        }
        menu.exec(QCursor::pos());
    }
    QListView::contextMenuEvent(e);
}

QModelIndexList PlayListView::getSelections() {return QListView::selectedIndexes();}
