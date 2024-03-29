#include "playlistview.h"
#ifndef NDEBUG
#include <QDebug>
#endif
PlayListView::PlayListView(QWidget *parent):QListView(parent) {
	QStringListModel *model = new QStringListModel(this);
    setModel(model);
    setAcceptDrops(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setFrameShape(QFrame::Box);
    setFont(QFont("微软雅黑",9));
    initMenu();
}

inline void PlayListView::initMenu() {
    menu = new QMenu(this);
    open = new QAction("打开目录",this);
    del = new QAction("删除",this);
    showDetail = new QAction("详细信息",this);
    menu->addActions({del,open,showDetail});
    connect(del,&QAction::triggered,this,&PlayListView::itemDelRequirement);
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
#define CONNECT_DOWNLOAD connect(open,&QAction::triggered,this,[&,this](){emit downloadRequirement(tmp);})
void PlayListView::contextMenuEvent(QContextMenuEvent *e) {
    //逻辑：选中多个时，如果选中的条目中存在本地音乐，则只有删除有效，否则下载也有效。其余项目均无效
    const QModelIndexList tmp = QListView::selectedIndexes();
    if(!tmp.empty()) {
        int first = tmp[0].row();
        if(tmp.size() > 1) {
            showDetail->setEnabled(false);
            bool hasLocal = false;
            for (const QModelIndex &idx:tmp) {
				if(!playList[idx.row()].contains("\n[")) {
                    hasLocal = true;
                    open->setText("打开目录");
                    open->setEnabled(false);
                    break;
                }
            }
            if(!hasLocal) {
                open->setText("下载");
                CONNECT_DOWNLOAD;
            }
        }
        else {
			if(playList[first].contains("\n[")) {
                open->setText("下载");
                CONNECT_DOWNLOAD;
            }
            else {
                 open->setText("打开目录");
                 connect(open,&QAction::triggered,this,[&,this](){
                     emit openRequirement(first);
                 });
            }
        }
        connect(showDetail,&QAction::triggered,this,[&,this]() {
            emit showDetailRequirement(first);
        });
        menu->exec(QCursor::pos());
    }
    disconnect(showDetail,&QAction::triggered,this,nullptr);
    disconnect(open,&QAction::triggered,this,nullptr);
    showDetail->setEnabled(true);
    open->setEnabled(true);
    QListView::contextMenuEvent(e);


}
#undef CONNECT_DOWNLOAD
QModelIndexList PlayListView::getSelections() {return QListView::selectedIndexes();}

void PlayListView::setSelected(int row) {
    clearSelection();
	QStringListModel *model_ = (QStringListModel*)model();
	QModelIndex target = model_->index(row,0);
    selectionModel()->select(target,QItemSelectionModel::Select);
    setCurrentIndex(target);
}

void PlayListView::commitChange() {((QStringListModel*)model())->setStringList(playList);}

QStringList PlayListView::list() {return playList;}

void PlayListView::setList(const QStringList &list) {
	playList = list;
	((QStringListModel*)model())->setStringList(playList);
}

void PlayListView::clear() {
	playList.clear();
	((QStringListModel*)model())->setStringList(playList);
}

PlayListView::~PlayListView() {}
