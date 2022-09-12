#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QMessageBox>
namespace Ui {class SettingWindow;}

class SettingWindow : public QWidget {
    Q_OBJECT
private:
    bool autoSave;
    bool minOnClose;
    double opacity;
    friend class PlayerWindow;
    Ui::SettingWindow *ui;
public:
    explicit SettingWindow(QWidget *parent = nullptr);
    void setAutoSave(bool f);
    void setminOnClose(bool f);
    void setOpacityValue(double opac);
    bool getAutoSave();
    bool getminOnClose();
    QString getDownLoc();
    ~SettingWindow();
private slots:
    void on_minOnCloseBox_stateChanged(int arg1);
    void on_autoSaveBox_stateChanged(int arg1);
    void on_pushButton_clicked();
    void on_spinBox_valueChanged(int value);

    void on_locButton_clicked();

signals:
    void changeEffectRequirement(int index);
    void changeOpacityRequirement(double opac);
};

#endif // SETTINGWINDOW_H
