#ifndef PLAYERBUTTON_H
#define PLAYERBUTTON_H
#include <QLabel>
#include <QMouseEvent>
class PlayerButton : public QLabel {
    Q_OBJECT
    int w,h;
public:
    PlayerButton(int w,int h,QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    ~PlayerButton();
signals:
    //void mousePressed();
    void clicked();
};

#endif // PLAYERBUTTON_H
