#ifndef PLAYERBUTTON_H
#define PLAYERBUTTON_H
#define __WIDGET_NEEDED__
#include "global_include.h"
/**
 * 播放器按钮类
 */
class PlayerButton : public QLabel {
    Q_OBJECT
    int w,h;  //初始大小
    bool clickable = false;
public:
    explicit PlayerButton(QWidget *parent = nullptr);
    ///设置鼠标点击时是否相应clicked()信号
    void setClickable(bool f);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    /// 改变状态。与{button->setToolTip(toolTip);button->setPixmap(pixmap);}相同
    void changeState(const QString &toolTip, const QPixmap &pixmap);
    ~PlayerButton();
signals:
    /// 鼠标点击信号
    void clicked();
public slots:
    void click();
};

#endif // PLAYERBUTTON_H
