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

#ifndef PAGEOVERLAY_H
#define PAGEOVERLAY_H

#include <QQuickItem>

class VerticalView;
class QSGSimpleRectNode;

class PageOverlay: public QQuickItem
{
    Q_OBJECT

public:
    PageOverlay(VerticalView *view, int pageIndex, QQuickItem *parent);
    ~PageOverlay();

    bool showLinkHighlight() const;
    void setShowLinkHighlight(bool show);

    QColor linkHighlightColor() const;
    void setLinkHightlightColor(const QColor &color);

protected:
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    //virtual void hoverMoveEvent(QHoverEvent * event);

private:
    void drawLinkBorders(QSGSimpleRectNode *parentNode);
    void drawBorders(QSGSimpleRectNode *parentNode, const QColor &color);

private:
    VerticalView* m_view;
    int m_pageIndex;
};
#endif // PAGEOVERLAY_H
