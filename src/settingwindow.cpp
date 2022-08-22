#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QDebug>
SettingWindow::SettingWindow(QWidget *parent) :
    QWidget(parent),ui(new Ui::SettingWindow) {
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    ui->autoSaveBox->setTristate(false);
    ui->minOnCloseBox->setTristate(false);
    autoSave = ui->autoSaveBox->isChecked();
    minOnClose = ui->minOnCloseBox->isChecked();
    opacity = ui->spinBox->value() / 100.0;
    connect(ui->comboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&SettingWindow::changeEffectRequirement);
}

void SettingWindow::setAutoSave(bool f) {
    ui->autoSaveBox->setChecked(f);
    autoSave = f;
}

void SettingWindow::setminOnClose(bool f) {
    ui->minOnCloseBox->setChecked(f);
    minOnClose = f;
}

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
    ui->spinBox->setValue(70);
}

void SettingWindow::on_spinBox_valueChanged(int value) {
    opacity = value / 100.0;
    emit changeOpacityRequirement(value/100.0);
}

