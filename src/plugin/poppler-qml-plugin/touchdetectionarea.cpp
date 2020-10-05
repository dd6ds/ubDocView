#include "touchdetectionarea.h"

#include <QDebug>

TouchDetectionArea::TouchDetectionArea(QQuickItem *parent)
    : QQuickItem(parent)
    , m_touchPressed(false)
{ }

void TouchDetectionArea::touchEvent(QTouchEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        m_touchPressed = true;
        Q_EMIT touchPressedChanged();
    } else if (event->type() == QEvent::TouchEnd) {
        m_touchPressed = false;
        Q_EMIT touchPressedChanged();
    }

    event->ignore();
}
