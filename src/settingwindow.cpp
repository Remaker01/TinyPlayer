#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QDebug>
#include <QStorageInfo>
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

QString SettingWindow::getDownLoc() {
    QString text = ui->locEdit->text();
    if(text == "音乐文件夹")
        return QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    return text;
}

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
    ui->locEdit->setText("音乐文件夹");
}

void SettingWindow::on_spinBox_valueChanged(int value) {
    opacity = value / 100.0;
    emit changeOpacityRequirement(value/100.0);
}

void SettingWindow::on_locButton_clicked() {
#ifdef Q_OS_WIN
    extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif
    QString loc = QFileDialog::getExistingDirectory(this,"选择文件夹");
    if(!loc.isEmpty()) {
#ifdef Q_OS_WIN
        qt_ntfs_permission_lookup++;
#endif
        if(QFileInfo(loc).isWritable()&&QStorageInfo(loc).bytesAvailable() > 0)
            ui->locEdit->setText(loc);
        else
            QMessageBox::warning(this,"警告","指定目录不可写，请重新选择");
#ifdef Q_OS_WIN
        qt_ntfs_permission_lookup--;
#endif
    }
}
