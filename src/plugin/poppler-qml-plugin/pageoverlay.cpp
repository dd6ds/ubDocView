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

#include "pageoverlay.h"
#include "pdfdocument.h"
#include "verticalview.h"

#include <QDebug>

// scenegraph painting
#include <QSGNode>
#include <QSGSimpleRectNode>

// mouse cursor
//#include <QCursor>

// Poppler Qt5
#include <poppler/qt5/poppler-qt5.h>

PageOverlay::PageOverlay(VerticalView *view, int pageIndex, QQuickItem *parent)
    : QQuickItem(parent)
    , m_view(view)
    , m_pageIndex(pageIndex)
{
    setFlag(ItemHasContents, true);
    //setAcceptHoverEvents(true);

    connect(m_view, &VerticalView::showLinkHighlightChanged, this, &PageOverlay::update);
    connect(m_view, &VerticalView::linkHighlightColorChanged, this, &PageOverlay::update);

    // Text search highlights
    connect(m_view, &VerticalView::searchResultsChanged, this, &PageOverlay::update);
    connect(m_view, &VerticalView::currentSearchResultChanged, this, &PageOverlay::update);
    connect(m_view, &VerticalView::searchHighlightColorChanged, this, &PageOverlay::update);
}

PageOverlay::~PageOverlay()
{ }

QSGNode *PageOverlay::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *)
{
    QSGNode* node = static_cast<QSGNode*>(oldNode);
    QQuickWindow* wnd = window();

    if (!node && wnd) {
        node = new QSGNode();
    }

    if (!node)
        return nullptr;

    // Remove the nodes from the previous paint, otherwise they pile up on every update.
    while (QSGNode* child = node->firstChild()) {
        node->removeChildNode(child);
        delete child;
    }

    if (!m_view->document() || !boundingRect().isValid())
        return node;

    // Search highlights
    const QList<QRectF> hits = m_view->searchResultsForPage(m_pageIndex);

    if (!hits.isEmpty()) {
        const bool currentOnThisPage = (m_view->currentSearchResultPage() == m_pageIndex);
        const QRectF currentRect = m_view->currentSearchResultRect();

        Q_FOREACH (const QRectF &hit, hits) {
            const bool isCurrent = currentOnThisPage && hit == currentRect;

            // Slightly enlarge the box so it doesn't clip the glyphs
            QRectF r = m_view->mapNormalizedRect(hit, width(), height());
            r.adjust(-1, -1, 1, 1);

            auto hitNode = new QSGSimpleRectNode();
            hitNode->setColor(isCurrent ? m_view->currentSearchHighlightColor()
                                        : m_view->searchHighlightColor());
            hitNode->setRect(r);

            if (isCurrent) {
                QColor border = m_view->currentSearchHighlightColor();
                border.setAlpha(255);
                drawBorders(hitNode, border);
            }

            node->appendChildNode(hitNode);
        }
    }

    if (m_view->showLinkHighlight()) {
        QList<Poppler::Link *> links = m_view->document()->pageLinks(m_pageIndex);

        Q_FOREACH(Poppler::Link *link, links) {
            if (link->linkType() == Poppler::Link::Goto ||
                    link->linkType() == Poppler::Link::Browse) {

                bool rotatedBy90 = (m_view->rotation() == VerticalView::Rotate90 ||
                                    m_view->rotation() == VerticalView::Rotate270);

                QRectF linkRect;
                int x1 = (!rotatedBy90 ? width() : height()) * link->linkArea().left();
                int y1 = (!rotatedBy90 ? height() : width()) * link->linkArea().top();
                int x2 = (!rotatedBy90 ? width() : height()) * link->linkArea().right();
                int y2 = (!rotatedBy90 ? height() : width()) * link->linkArea().bottom();
                int w  = this->width();
                int h  = this->height();

                switch (m_view->rotation()) {
                case VerticalView::Rotate0:
                    linkRect.setLeft   ( x1 );
                    linkRect.setTop    ( y1 );
                    linkRect.setRight  ( x2 );
                    linkRect.setBottom ( y2 );
                    break;
                case VerticalView::Rotate90:
                    linkRect.setLeft   ( w - y2 );
                    linkRect.setTop    ( x1     );
                    linkRect.setRight  ( w - y1 );
                    linkRect.setBottom ( x2     );
                    break;
                case VerticalView::Rotate180:
                    linkRect.setLeft   ( w - x2 );
                    linkRect.setTop    ( h - y2 );
                    linkRect.setRight  ( w - x1 );
                    linkRect.setBottom ( h - y1 );
                    break;
                case VerticalView::Rotate270:
                    linkRect.setLeft   ( y1     );
                    linkRect.setTop    ( h - x2 );
                    linkRect.setRight  ( y2     );
                    linkRect.setBottom ( h - x1 );
                    break;
                }

                auto linkNode = new QSGSimpleRectNode();

                const QColor &linkColor = m_view->linkHighlightColor();
                linkNode->setColor(QColor::fromRgb(linkColor.red(), linkColor.green(), linkColor.blue(), 8));
                linkNode->setRect(linkRect);

                drawLinkBorders(linkNode);

                node->appendChildNode(linkNode);
            }
        }
    }

    return node;
}

void PageOverlay::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    update();
}

/*
 *  QML MouseArea overwrite any attempt to set this from C++
 *  Disable this code and do it through QML (see Viewer.qml).
 *  TODO: Check performance of the QML code that handles this.
 *
void PageOverlay::hoverMoveEvent(QHoverEvent *event)
{
    // We only handle what's strictly necessary for changing the cursor aspect.
    // Any other mouse event is handled directly through QML.
    const QPointF &curPos = event->posF();

    QList<Poppler::Link *> links = m_view->document()->pageLinks(m_pageIndex);
    Q_FOREACH (Poppler::Link *link, links) {
        QRectF linkRect(link->linkArea().x() * this->width(),
                        link->linkArea().y() * this->height(),
                        link->linkArea().width() * this->width(),
                        link->linkArea().height() * this->height());

        if (linkRect.contains(curPos)) {
            setCursor(QCursor(Qt::PointingHandCursor));
            return;
        }
    }

    // Not hovering a link. Restore default cursor.
    unsetCursor();
}
*/

void PageOverlay::drawLinkBorders(QSGSimpleRectNode *parentNode)
{
    const QColor &linkColor = m_view->linkHighlightColor();
    drawBorders(parentNode, QColor::fromRgb(linkColor.red(), linkColor.green(), linkColor.blue(), 64));
}

void PageOverlay::drawBorders(QSGSimpleRectNode *parentNode, const QColor &color)
{
    auto node = parentNode;

    auto linkBorderGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 8);
    linkBorderGeometry->setDrawingMode(GL_LINES);
    linkBorderGeometry->setLineWidth(1);

    QSGGeometry::Point2D* vertex = linkBorderGeometry->vertexDataAsPoint2D();
    vertex[0].set(node->rect().left(), node->rect().top());
    vertex[1].set(node->rect().left(), node->rect().bottom());

    vertex[2].set(node->rect().right(), node->rect().top());
    vertex[3].set(node->rect().right(), node->rect().bottom());

    vertex[4].set(vertex[0].x, vertex[0].y);
    vertex[5].set(vertex[2].x, vertex[2].y);

    vertex[6].set(vertex[1].x, vertex[1].y);
    vertex[7].set(vertex[3].x, vertex[3].y);

    auto linkBorderMaterial = new QSGFlatColorMaterial;
    linkBorderMaterial->setColor(color);

    auto linkBorderNode = new QSGGeometryNode;

    linkBorderNode->setGeometry(linkBorderGeometry);
    linkBorderNode->setFlag(QSGNode::OwnsGeometry);

    linkBorderNode->setMaterial(linkBorderMaterial);
    linkBorderNode->setFlag(QSGNode::OwnsMaterial);

    node->appendChildNode(linkBorderNode);
}
