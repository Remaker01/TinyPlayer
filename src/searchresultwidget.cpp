#include "searchresultwidget.h"
#include <QDebug>
#undef NDEBUG
SearchResultWidget::SearchResultWidget(QWidget *parent) :
    QWidget(parent),ui(new Ui::SearchResultWidget) {
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    QStringList headers({"标题","歌手","链接"});
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    connect(ui->insertButton,&QPushButton::clicked,this,[this] {emit addItemRequirement(ui->autoDelBox->isChecked());});
}

void SearchResultWidget::setItems(const QList<ResultInfo> &results) {
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

void SearchResultWidget::on_tableWidget_cellClicked(int row, int column) {
    if(column != 2)
        return;
    QLabel *textlabel = (QLabel*)(ui->tableWidget->cellWidget(row,column));
    QDesktopServices::openUrl(QUrl(textlabel->text()));
}

SearchResultWidget::~SearchResultWidget() {
    delete ui;
}

void SearchResultWidget::on_copyButton_clicked() {
    auto sel = getSelectedItems();
    if(sel.isEmpty())
        return;
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(sel[0].url);
}
