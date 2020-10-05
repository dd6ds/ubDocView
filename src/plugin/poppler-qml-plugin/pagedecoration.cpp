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
