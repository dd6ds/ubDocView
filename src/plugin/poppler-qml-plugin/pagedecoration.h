#ifndef PAGEDECORATION_H
#define PAGEDECORATION_H

#include <QQuickItem>

class PageDecoration: public QQuickItem
{
    Q_OBJECT

public:
    PageDecoration(QQuickItem *parent);
    ~PageDecoration();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

};
#endif // PAGEDECORATION_H
