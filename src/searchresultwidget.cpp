#include "searchresultwidget.h"
#include <QDebug>
SearchResultWidget::SearchResultWidget(QWidget *parent) :
    QWidget(parent),ui(new Ui::SearchResultWidget) {
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    QStringList headers{"标题","歌手","URL"};
    ui->tableWidget->setColumnCount(headers.size());
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    QList<ResultInfo> infos;
    setItems(infos);
    connect(ui->insertButton,&QPushButton::clicked,this,&SearchResultWidget::addItemRequirement);
}

void SearchResultWidget::setItems(QList<ResultInfo> results) {
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

SearchResultWidget::~SearchResultWidget() {
    delete ui;
}

void SearchResultWidget::on_tableWidget_cellPressed(int row, int column) {
    if(column != 2)
        return;
    QLabel *textlabel = dynamic_cast<QLabel*>(ui->tableWidget->cellWidget(row,column));
    if(textlabel == nullptr)
        return;
    QString text = textlabel->text();
    QDesktopServices::openUrl(QUrl(text));
}
