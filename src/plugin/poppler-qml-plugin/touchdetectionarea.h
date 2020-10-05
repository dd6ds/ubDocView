#ifndef TOUCHDETECTIONAREA_H
#define TOUCHDETECTIONAREA_H

#include <QQuickItem>

class TouchDetectionArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool touchPressed READ touchPressed NOTIFY touchPressedChanged)
public:
    TouchDetectionArea(QQuickItem *parent = 0);
    bool touchPressed() const { return m_touchPressed; }

Q_SIGNALS:
    void touchPressedChanged();

protected:
    virtual void touchEvent(QTouchEvent *event);

private:
    bool m_touchPressed;
};

#endif // TOUCHDETECTIONAREA_H
