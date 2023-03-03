#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H
#define __WIDGET_NEEDED__
#include "global_include.h"
/**
 * @brief 无边框窗口类
 */
class FramelessWindow : public QWidget {
    Q_OBJECT
private:
    inline void region(const QPoint &currentLoc, const QRect &_rect);
public:
    explicit FramelessWindow(QWidget *parent);
    /// 鼠标位置
    enum Diretion:short {
        NONE = -1,UP,DOWN,/*LEFT,*/RIGHT,/*LEFT_TOP,LEFT_DOWN,*/RIGHT_TOP,RIGHT_DOWN
    };
    /**
     * @brief setCloseButton 设置关闭按钮
     * @param closeBtn 关闭按钮控件
     * @note 必须保证closeBtn在本窗口生命周期内有效
     */
    void setCloseButton(QAbstractButton *closeBtn);
    void setMinimizeButton(QAbstractButton *minBtn);
    void setMaxmizeButton(QAbstractButton *maxBtn);
//    void addSubMenu(const QString &name,QList<QString> &items,int loc = -1); //unused
    /**
     * setMenu 设置菜单包含的actions和样式表。原有actions和样式表将被清空
     * @param actions 定义的actions
     * @param style 样式表，默认为空
     */
    void setMenu(QList<QAction*> actions,const QString &style = QString());
//    QMenu *getMenu();
protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void showEvent(QShowEvent *ev);
private:
    static constexpr int PADDING = 2; //设置窗口边距
    QPoint mouseLoc; //鼠标位置
    volatile bool leftBtnPressed; //左键是否按下
    volatile Diretion dir; //记录窗口大小改变的方向
    QMenu* menu = nullptr;
//    QMenu* subMenu = nullptr;
public slots:
    /// 在指定范围legalRange内显示菜单
    void showMenu(QRect legalRange);
};

#endif // FRAMELESSWINDOW_H
