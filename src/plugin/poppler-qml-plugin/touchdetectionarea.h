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
