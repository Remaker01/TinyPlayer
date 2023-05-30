#include "searchresultwidget.h"
#include <QDebug>
#undef NDEBUG
/* 分页查询
 * 思路：
 * 1.OnlineSeacher::doSearch中添加一个参数
 * 2.SearchResultWidget中添加上一页与下一页按钮。点击后：
 *   1.发送一个信号，参数为页数，PlayerWindow捕获到这个信号后立即重新开始搜索
 *   2.更新页数
 *   3.搜索结束后，调用SearchResultWidget::setItems重新设定条目
 *   4.如果新页数为第一页，则“上一页”按钮无效；如果少于某数量的结果，则“下一页”无效
 *   在等待结果期间，可以播放一个动画，一旦出结果就立即停止
 */
SearchResultWidget::SearchResultWidget(QWidget *parent) :
    QWidget(parent),ui(new Ui::SearchResultWidget) {
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    QStringList headers({"标题","歌手","链接"});
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    connect(ui->insertButton,&QPushButton::clicked,this,[this] {emit addItemRequirement(ui->autoDelBox->isChecked());});
    gif = new QMovie(":/Icons/images/waiting.gif");gif->setParent(this);
    ui->gifLabel->setMovie(gif);
}

void SearchResultWidget::setItems(const QList<ResultInfo> &results) {
    gif->stop();
    ui->gifLabel->setVisible(false);
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(results.size());
    for (int i = 0; i < results.size(); i++) {
        QTableWidgetItem *itemTitle = new QTableWidgetItem(results[i].title),
        *itemArtist = new QTableWidgetItem(results[i].artist);
        QLabel *itemUrl = new QLabel(results[i].url);
        itemUrl->setCursor(Qt::PointingHandCursor);
        itemUrl->setAlignment(Qt::AlignCenter);
        itemUrl->setToolTip("点击打开");
        ui->tableWidget->setItem(i,0,itemTitle);
        ui->tableWidget->setItem(i,1,itemArtist);
        ui->tableWidget->setCellWidget(i,2,itemUrl);
    }
    ui->nextButton->setEnabled(results.size() >= 5);
}

ResultInfo SearchResultWidget::getItem(int row) {
    ResultInfo info;
    if(row < 0||row >= ui->tableWidget->rowCount())
        return info;
    QTableWidgetItem *itemTitle = ui->tableWidget->item(row,0),  //itemAt是根据坐标选
            *itemArtist = ui->tableWidget->item(row,1);
    QLabel *itemUrl = (QLabel*)ui->tableWidget->cellWidget(row,2);
    info.title = itemTitle->text();
    info.artist = itemArtist->text();
    info.url = itemUrl->text();
    return info;
}

QList<ResultInfo> SearchResultWidget::getSelectedItems() {
    QList<QTableWidgetItem*> itemSelected = ui->tableWidget->selectedItems();
    QList<ResultInfo> result;
    result.reserve(itemSelected.size());
    for(int i = 0; i < itemSelected.size(); i++) {
        int p = itemSelected[i]->row();
        result.append(getItem(p));
    }
    return result;
}

QList<QString> SearchResultWidget::getSelectedURLs() {
    QList<ResultInfo> itemSelected = getSelectedItems();
    QList<QString> result;
    result.reserve(itemSelected.size());
    for(int i = 0; i < itemSelected.size(); i++) {
        result.append(itemSelected[i].url);
    }
    return result;
}

void SearchResultWidget::removeSelected() {
    QList<QTableWidgetItem*> itemSelected = ui->tableWidget->selectedItems();
    std::set<int,std::greater<int> > rows;
    for (QTableWidgetItem *i:qAsConst(itemSelected))
        rows.insert(i->row());
    for (int i:rows) {
        ui->tableWidget->removeRow(i);
    }
    ui->tableWidget->clearSelection();
}

void SearchResultWidget::closeEvent(QCloseEvent *event) {
    QWidget::closeEvent(event);
    ui->pageLabel->setText(QStringLiteral("<span style=\"font-size:small\">当前第1页</span>"));
}

void SearchResultWidget::on_tableWidget_cellClicked(int row, int column) {
    if(column != 2)
        return;
    QLabel *textlabel = (QLabel*)(ui->tableWidget->cellWidget(row,column));
    QDesktopServices::openUrl(QUrl(textlabel->text()));
}

uint SearchResultWidget::getPage() {
    const QString pageStr = ui->pageLabel->text().mid(33);
    //查找第一个非数字
    int n = 0;
    for(QChar ch:pageStr) {
        if(!ch.isDigit())
            break;
        n++;
    }
    return pageStr.midRef(0,n).toUInt();
}

void SearchResultWidget::setPage(uint page) {
    const QString pageStr = "<span style=\"font-size:small\">当前第%1页</span>";
    ui->pageLabel->setText(pageStr.arg(page));
    emit changPageRequirement(page);
}

void SearchResultWidget::on_nextButton_clicked() {
    if(gif->state() == QMovie::Running)
        return;
    setPage(getPage()+1);
    ui->gifLabel->setVisible(true);
    gif->start();
    ui->prevButton->setEnabled(true); //TODO:check this
}

void SearchResultWidget::on_copyButton_clicked() {
    auto sel = getSelectedItems();
    if(sel.isEmpty())
        return;
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(sel[0].url);
}

SearchResultWidget::~SearchResultWidget() {
    delete ui;
}

void SearchResultWidget::on_prevButton_clicked() {
    if(gif->state() == QMovie::Running)
        return;
    uint page = getPage();
    if(page > 1) {
        setPage(page-1);
        ui->gifLabel->setVisible(true);
        gif->start();
        if(page == 2)
            ui->prevButton->setEnabled(false);
    }
}
