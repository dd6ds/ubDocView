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

#include "pagedecoration.h"

#include <QDebug>
#include <QSGSimpleRectNode>

#define PAGEDECORATION_BORDER_WIDTH 1

PageDecoration::PageDecoration(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

PageDecoration::~PageDecoration()
{ }

QSGNode *PageDecoration::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *)
{
    QSGSimpleRectNode* node = static_cast<QSGSimpleRectNode*>(oldNode);
    QQuickWindow* wnd = window();

    QRectF outterRect = boundingRect().adjusted(
                PAGEDECORATION_BORDER_WIDTH * -1,
                PAGEDECORATION_BORDER_WIDTH * -1,
                PAGEDECORATION_BORDER_WIDTH,
                PAGEDECORATION_BORDER_WIDTH);

    if (!node && wnd) {
        node = new QSGSimpleRectNode();
        node->setColor(QColor::fromRgb(0, 0, 0, 12));

        auto whitePaperNode = new QSGSimpleRectNode();
        whitePaperNode->setColor(Qt::white);
        whitePaperNode->setRect(boundingRect());

        node->appendChildNode(whitePaperNode);
    }

    node->setRect(outterRect);

    return node;
}

void PageDecoration::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    update();
}
