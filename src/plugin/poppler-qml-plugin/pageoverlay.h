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

private:
    VerticalView* m_view;
    int m_pageIndex;
};
#endif // PAGEOVERLAY_H
