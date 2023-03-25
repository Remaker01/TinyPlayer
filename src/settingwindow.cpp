#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QDebug>
#include <QStorageInfo>
SettingWindow::SettingWindow(QWidget *parent) :
    QWidget(parent),ui(new Ui_SettingWindow) {
    QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,QApplication::applicationDirPath());
    ui->setupUi(this);
//    setWindowFlag(Qt::Window);
    ui->autoSaveBox->setTristate(false);
    ui->minOnCloseBox->setTristate(false);
    opacity = ui->spinBox->value() / 100.0;
    connect(ui->effectBox,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&SettingWindow::changeEffectRequirement);
    initUi();
}

inline void SettingWindow::initUi() {
    QPalette pl;
    QPixmap img(":/Icons/images/back.jpg");
    int w = maximumWidth(),h = img.height()/2;
    pl.setBrush(backgroundRole(),QBrush(img.scaled(w,h)));
    setPalette(pl);
}

void SettingWindow::setAutoSave(bool f) {
    ui->autoSaveBox->setChecked(f);
    //autoSave = f;
}

void SettingWindow::setminOnClose(bool f) {
    ui->minOnCloseBox->setChecked(f);
    //minOnClose = f;
}

void SettingWindow::setOpacityValue(double opac) {
    ui->spinBox->setValue((int)(0.5 + opac * 100.0)); //防止浮点数误差
    opacity = opac;
}

bool SettingWindow::getAutoSave() {return ui->autoSaveBox->isChecked();}

bool SettingWindow::getminOnClose() {return ui->minOnCloseBox->isChecked();}

QString SettingWindow::getDownLoc() {
    QString text = ui->locEdit->text();
    if(text == "音乐文件夹")
        return QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    return text;
}

//int SettingWindow::getSrchMethod() {return ui->methodBox->currentIndex();}

SettingWindow::~SettingWindow() {
    delete ui;
}

void SettingWindow::on_pushButton_clicked() {
    ui->minOnCloseBox->setChecked(false);
    ui->autoSaveBox->setChecked(true);
    ui->spinBox->setValue(70);
    ui->effectBox->setCurrentIndex(0);
    ui->locEdit->setText("音乐文件夹");
}

void SettingWindow::on_spinBox_valueChanged(int value) {
    opacity = value / 100.0;
    emit changeOpacityRequirement(opacity);
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
        // 检查目录是否可写且有空间
        if(QFileInfo(loc).isWritable()&&QStorageInfo(loc).bytesAvailable() > 0)
            ui->locEdit->setText(loc);
        else
            QMessageBox::warning(nullptr,"警告","指定目录不可写，请重新选择");
#ifdef Q_OS_WIN
        qt_ntfs_permission_lookup--;
#endif
    }
}
