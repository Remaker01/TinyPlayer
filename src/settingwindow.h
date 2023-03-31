#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H
#define __WIDGET_NEEDED__
#include "global_include.h"
//namespace Ui {class SettingWindow;}
class Ui_SettingWindow;
class SettingWindow : public QWidget {
    Q_OBJECT
private:
    double opacity;
    friend class PlayerWindow;
    Ui_SettingWindow *ui;
    void initUi();
public:
    explicit SettingWindow(QWidget *parent = nullptr);
    void loadSettings(QSettings &setting);
    void saveSettings(QSettings &setting);
    void setAutoSave(bool f);
    void setminOnClose(bool f);
    void setOpacityValue(double opac);
    bool getAutoSave();
    bool getminOnClose();
    QString getDownLoc();
    void setDownLoc(QString loc);
//    int getSrchMethod();
//    void setSrchMethod(int m);
    ~SettingWindow();
private slots:
//    void on_minOnCloseBox_stateChanged(int arg1);
//    void on_autoSaveBox_stateChanged(int arg1);
    void on_pushButton_clicked();
    void on_spinBox_valueChanged(int value);
    void on_locButton_clicked();

signals:
    void changeEffectRequirement(int index);
    void changeOpacityRequirement(double opac);
};

#endif // SETTINGWINDOW_H
