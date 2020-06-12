#ifndef QSLIDESTAKEDWIDGET_H
#define QSLIDESTAKEDWIDGET_H
#include <QWidget>
#include <QStackedWidget>
#include <QEasingCurve>

class QSlideStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    enum t_direction {
        LEFT2RIGHT,
        RIGHT2LEFT,
        TOP2BOTTOM,
        BOTTOM2TOP,
        AUTOMATIC
    };
    explicit QSlideStackedWidget(QWidget *parent);
    ~QSlideStackedWidget() {}
public slots:
    void setSpeed(int speed);
    void setAnimation(enum QEasingCurve::Type animationtype);
    void setVerticalMode(bool vertical=true);
    void setWrap(bool wrap);
    void slideInNext();
    void slideInPrev();
    void slideInIdx(int idx, enum t_direction direction=AUTOMATIC);
    void slideInWgt(QWidget * widget, enum t_direction direction=AUTOMATIC);
signals:
    void animationFinished(void);
protected slots:
    void animationDoneSlot(void);
protected:
    QWidget *m_mainwindow;
    int m_speed;
    enum QEasingCurve::Type m_animationtype;
    bool m_vertical;
    int m_now;
    int m_next;
    bool m_wrap;
    QPoint m_pnow;
    bool m_active;
    QList<QWidget*> blockedPageList;
};
#endif
