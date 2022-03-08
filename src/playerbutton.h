#ifndef PLAYERBUTTON_H
#define PLAYERBUTTON_H
#include <QLabel>
#include <QMouseEvent>
/**
 * 播放器按钮类
 */
class PlayerButton : public QLabel {
    Q_OBJECT
    const int w,h;  //初始大小
    bool replyClick = true;
public:
    PlayerButton(int W,int H,QWidget *parent = nullptr);
    ///设置鼠标点击时是否相应clicked()信号
    void setReplyClick(bool f);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    ~PlayerButton();
signals:
    //void mousePressed();
    /// 鼠标点击信号
    void clicked();
};

#endif // PLAYERBUTTON_H
