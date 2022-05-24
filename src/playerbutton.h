#ifndef PLAYERBUTTON_H
#define PLAYERBUTTON_H
#include <QLabel>
#include <QMouseEvent>
/**
 * 播放器按钮类
 */
class PlayerButton : public QLabel {
    Q_OBJECT
    int w,h;  //初始大小
    bool replyClick = true;
public:
    explicit PlayerButton(QWidget *parent = nullptr);
    ///设置鼠标点击时是否相应clicked()信号
    void setReplyClick(bool f);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void changeState(const QString &toolTip, const QPixmap &pixmap);
    ~PlayerButton();
signals:
    /// 鼠标点击信号
    void clicked();
public slots:
    void click();
};

#endif // PLAYERBUTTON_H
