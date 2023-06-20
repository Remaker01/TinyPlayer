#include "settingwindow.h"
#include "ui_settingwindow.h"
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
/* 测试项目:
 * 1.两个框全部设置为选中->下次打开仍选中 ok
 * 2.两个框全部不选择->下次打开仍全不选中，且不自动加载播放列表 ok
 * 3.设置下载路径为"音乐文件夹"->下次打开仍为“音乐文件夹” ok
 * 4.设置下载路径为桌面->下次打开仍为桌面 ok
 */
void SettingWindow::loadSettings(QSettings &setting) {
    QStringList keys = setting.childGroups(),keys_general = setting.childKeys();
    // 在[General]项下的设置
    if(keys_general.contains("TOOLBAR_OPAC"))
        setOpacityValue(setting.value("TOOLBAR_OPAC").toDouble());
    else
        setting.setValue("TOOLBAR_OPAC",opacity);
    if(keys_general.contains("AUTOLOAD"))
        ui->autoSaveBox->setChecked(setting.value("AUTOLOAD").toBool());
    else
        setting.setValue("AUTOLOAD",ui->autoSaveBox->isChecked());
    if(keys_general.contains("MIN_ONCLOSE"))
        ui->minOnCloseBox->setChecked(setting.value("MIN_ONCLOSE").toBool());
    else
        setting.setValue("MIN_ONCLOSE",ui->minOnCloseBox->isChecked());
    if(keys.contains("DOWNLOAD")) {
        setDownLoc(setting.value("DOWNLOAD/LOC").toString());
    }
    else {
        setting.setValue("DOWNLOAD/LOC",getDownLoc());
    }
}

void SettingWindow::saveSettings(QSettings &setting) {
    if(setting.format() == QSettings::InvalidFormat) {
#ifndef NDEBUG
        qCritical() << "ERROR:saveSettings:format of the setting is invalid.Exiting.";
#endif
        return;
    }
    setting.setValue("AUTOLOAD", ui->autoSaveBox->isChecked());
    setting.setValue("MIN_ONCLOSE",ui->minOnCloseBox->isChecked());
    setting.setValue("TOOLBAR_OPAC",opacity);
    setting.setValue("DOWNLOAD/LOC",getDownLoc());
}

inline void SettingWindow::initUi() {
    QPalette pl;
    QPixmap img(":/Icons/images/back.jpg");
    int w = maximumWidth(),h = img.height()/2;
    pl.setBrush(backgroundRole(),QBrush(img.scaled(w,h)));
    setPalette(pl);
    ui->effectBox->setView(new QListView());
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

void SettingWindow::setDownLoc(QString loc) {
    if(QStandardPaths::writableLocation(QStandardPaths::MusicLocation).compare(loc,Qt::CaseInsensitive) == 0) {
        ui->locEdit->setText("音乐文件夹");
    }
    else {
       ui->locEdit->setText(loc);
    }
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
