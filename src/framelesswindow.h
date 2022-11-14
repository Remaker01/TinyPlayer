#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QAbstractButton>
/**
 * @brief 无边框窗口类
 */
class FramelessWindow : public QWidget {
    Q_OBJECT
private:
    inline void region(const QPoint &currentLoc, const QRect &_rect);
public:
    explicit FramelessWindow(QWidget *parent = nullptr);
    /// 鼠标位置
    enum Diretion:short {
        NONE = -1,UP,DOWN,/*LEFT,*/RIGHT,/*LEFT_TOP,LEFT_DOWN,*/RIGHT_TOP,RIGHT_DOWN
    };
    /**
     * @brief setCloseButton 设置关闭按钮
     * @param closeBtn 关闭按钮控件
     * @note 必须保证closeBtn在本FramelessWindow对象生命周期内有效
     */
    void setCloseButton(QAbstractButton *closeBtn);
    void setMinimizeButton(QAbstractButton *minBtn);
    void setMaxmizeButton(QAbstractButton *maxBtn);
protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
private:
    static constexpr int PADDING = 2; //设置窗口边距
    QPoint mouseLoc; //鼠标位置
    volatile bool leftBtnPressed; //左键是否按下
    volatile Diretion dir; //记录窗口大小改变的方向
signals:
};

#endif // FRAMELESSWINDOW_H
