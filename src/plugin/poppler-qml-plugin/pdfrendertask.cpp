#include "pdfrendertask.h"

bool PdfRenderTask::canBeRunInParallel(AbstractRenderTask* prevTask)
{
    Q_ASSERT(prevTask != nullptr);
    return true;
}

QImage PdfRenderTask::doWork()
{
    return m_document.data()->paintPage(m_page, m_zoom, m_area, PdfDocument::Rotation(m_rotation));
}
