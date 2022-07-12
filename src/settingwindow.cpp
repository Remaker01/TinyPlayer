#include "settingwindow.h"
#include "ui_settingwindow.h"
SettingWindow::SettingWindow(QWidget *parent) :
    QWidget(parent),ui(new Ui::SettingWindow) {
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    ui->autoSaveBox->setTristate(false);
    ui->minOnCloseBox->setTristate(false);
    autoSave = ui->autoSaveBox->isChecked();
    minOnClose = ui->minOnCloseBox->isChecked();
}

void SettingWindow::setAutoSave(bool f) {ui->autoSaveBox->setChecked(f);}

void SettingWindow::setminOnClose(bool f) {ui->minOnCloseBox->setChecked(f);}

bool SettingWindow::getAutoSave() {return autoSave;}

bool SettingWindow::getminOnClose() {return minOnClose;}

SettingWindow::~SettingWindow() {
    delete ui;
}

void SettingWindow::on_minOnCloseBox_stateChanged(int arg1) {
    if(arg1 == Qt::Checked)
        minOnClose = true;
    else
        minOnClose = false;
}

void SettingWindow::on_autoSaveBox_stateChanged(int arg1) {
    if(arg1 == Qt::Checked)
        autoSave = true;
    else
        autoSave = false;
}


void SettingWindow::on_pushButton_clicked() {
    ui->minOnCloseBox->setChecked(false);
    ui->autoSaveBox->setChecked(true);
}

