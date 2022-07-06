#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>
namespace Ui {class SettingWindow;}

class SettingWindow : public QWidget {
    Q_OBJECT
    bool autoSave;
    bool minOnClose;
public:

    explicit SettingWindow(QWidget *parent = nullptr);
    void setAutoSave(bool f);
    void setminOnClose(bool f);
    bool getAutoSave();
    bool getminOnClose();
    ~SettingWindow();

private slots:
    void on_minOnCloseBox_stateChanged(int arg1);
    void on_autoSaveBox_stateChanged(int arg1);

    void on_pushButton_clicked();

private:
    Ui::SettingWindow *ui;
};

#endif // SETTINGWINDOW_H
