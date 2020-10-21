/*
 * Copyright (C) 2016
 *          Stefano Verzegnassi <verzegnassi.stefano@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
