#ifndef LORENDERTASK_H
#define LORENDERTASK_H

#include <QObject>
#include <QDebug>
#include <QImage>
#include <QSharedPointer>
#include <QHash>
#include <QQueue>
#include <QAtomicInt>

#include "../../app/rendertask.h"
#include <QSharedPointer>
#include "pdfdocument.h"

class PdfRenderTask : public AbstractRenderTask
{
public:
    virtual RenderTaskType type() { return RttPdfPage; }
    virtual QImage doWork();
    virtual bool canBeRunInParallel(AbstractRenderTask* prevTask);
    virtual void prepare() { }

    //double resolution() { return m_resolution; }
    //void setResolution(double r) { m_resolution = r; }
    QRect area() { return m_area; }
    void setArea(const QRect &a) { m_area = a; }
    QSize size() { return m_size; }
    void setSize(const QSize &s) { m_size = s; }
    int rotation() { return m_rotation; }
    void setRotation(int r) { m_rotation = r; }
    int page() { return m_page; }
    void setPage(int p) { m_page = p; }
    QSharedPointer<PdfDocument> document() { return m_document; }
    void setDocument(QSharedPointer<PdfDocument> d) { m_document = d; }
    qreal zoom() { return m_zoom; }
    void setZoom(qreal z) { m_zoom = z; }
protected:
    int m_page;
    QRect m_area;
    QSize m_size;
    int m_rotation;
    qreal m_zoom;
    //double m_resolution;
    QSharedPointer<PdfDocument> m_document;
};
#endif // LORENDERTASK_H
