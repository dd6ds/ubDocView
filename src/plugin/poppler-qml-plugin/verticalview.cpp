/*
 * Copyright (C) 2015-2016
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

#include "verticalview.h"
#include "pdfdocument.h"
#include "pdfzoom.h"
#include "twips.h"
#include "sgtileitem.h"
#include "pagedecoration.h"
#include "pageoverlay.h"

#include "config.h"

#include <QImage>
#include <QtCore/qmath.h>

// FIXME: Sometimes layout is not created when a document is opened
// TODO: Check if everything works as expected when zooming

#define UPDATE_TIMER_MSECS 20

VerticalView::VerticalView(QQuickItem *parent)
    : QQuickItem(parent)
    , m_parentFlickable(nullptr)
    , m_document(nullptr)
    , m_zoomSettings(new PdfZoom(this))
    , m_cacheBuffer(200)
    , m_spacing(12)
    , m_rotation(Rotation::Rotate0)
    , m_currentPageIndex(-1)
    , m_visibleArea(0, 0, 0, 0)
    , m_bufferArea(0, 0, 0, 0)
    , m_renderHints()
    , m_showLinkHighlight(true)
    , m_linkHighlightColor(Qt::red)
    , m_error(PopplerError::NoError)
    , m_hasZoomChanged(false)
    , m_hasRotationChanged(false)
    , m_hasFlickableBeenScrolled(false)
    , m_hasFlickableBeenResized(false)
    , m_hasRenderHintsChanged(false)
{
    // Connect signals
    connect(this, &VerticalView::documentChanged, this, &VerticalView::updateLayout);
    connect(this, &VerticalView::parentFlickableChanged, this, &VerticalView::updateLayout);
    connect(this, &VerticalView::cacheBufferChanged, this, &VerticalView::updateLayout);
    connect(this, &VerticalView::spacingChanged, this, &VerticalView::updateLayout);
    connect(this, &VerticalView::renderHintsChanged, this, &VerticalView::onRenderHintsChanged);

    connect(RenderEngine::instance(), &RenderEngine::taskRenderFinished,
            this, &VerticalView::slotTaskRenderFinished);

    connect(&m_updateTimer, &QTimer::timeout, this, &VerticalView::updateLayout);

    connect(m_zoomSettings, &PdfZoom::zoomFactorChanged, [&]() {
        m_hasZoomChanged = true;
        updateLayout();
    });

    connect(this, &VerticalView::rotationChanged, [&]() {
        m_hasRotationChanged = true;
        updateLayout();
    });
}

VerticalView::~VerticalView()
{
    disconnect(RenderEngine::instance(), &RenderEngine::taskRenderFinished,
            this, &VerticalView::slotTaskRenderFinished);

    qDeleteAll(m_decorations);
    qDeleteAll(m_overlays);
    qDeleteAll(m_pages);
}

// Returns the parent QML Flickable
QQuickItem* VerticalView::parentFlickable() const
{
    return m_parentFlickable;
}

// Set the parent QML Flickable
void VerticalView::setParentFlickable(QQuickItem *flickable)
{
    if (m_parentFlickable == flickable)
        return;

    if (m_parentFlickable)
        m_parentFlickable->disconnect(this);

    m_parentFlickable = flickable;

    connect(m_parentFlickable, &QQuickItem::widthChanged, this, &VerticalView::onFlickableSizeChanged);
    connect(m_parentFlickable, &QQuickItem::heightChanged, this, &VerticalView::onFlickableSizeChanged);

    connect(m_parentFlickable, SIGNAL(contentXChanged()), this, SLOT(onFlickableScrolled()));
    connect(m_parentFlickable, SIGNAL(contentYChanged()), this, SLOT(onFlickableScrolled()));

    Q_EMIT parentFlickableChanged();
}

PdfDocument* VerticalView::document() const
{
    return m_document.data();
}

PdfZoom *VerticalView::zoomSettings() const
{
    return m_zoomSettings;
}
void VerticalView::setCurrentPageIndex(int currentPageIndex)
{
    if (m_currentPageIndex == currentPageIndex)
        return;

    m_currentPageIndex = currentPageIndex;
    Q_EMIT currentPageIndexChanged();
}

int VerticalView::cacheBuffer() const
{
    return m_cacheBuffer;
}

void VerticalView::setCacheBuffer(int cacheBuffer)
{
    if (m_cacheBuffer == cacheBuffer)
        return;

    m_cacheBuffer = cacheBuffer;
    Q_EMIT cacheBufferChanged();
}

int VerticalView::spacing() const
{
    return m_spacing;
}

void VerticalView::setSpacing(int spacing)
{
    if (m_spacing == spacing)
        return;

    m_spacing = spacing;
    Q_EMIT spacingChanged();
}

int VerticalView::currentPageIndex() const
{
    return m_currentPageIndex;
}

int VerticalView::pagesCount() const
{
    if (!m_document || m_document.data()->isLocked())
        return 0;

    return m_document.data()->pageCount();
}

VerticalView::Rotation VerticalView::rotation() const
{
    return m_rotation;
}

void VerticalView::setRotation(VerticalView::Rotation rotation)
{
    if (m_rotation == rotation)
        return;

    m_rotation = rotation;
    Q_EMIT rotationChanged();
}

PopplerError::Error VerticalView::error() const
{
    return m_error;
}

PdfDocument::RenderHints VerticalView::renderHints() const
{
    return m_renderHints;
}

void VerticalView::setRenderHints(const PdfDocument::RenderHints hints)
{
    if (m_renderHints == hints)
        return;

    m_renderHints = hints;
    Q_EMIT renderHintsChanged();
}

bool VerticalView::showLinkHighlight() const
{
    return m_showLinkHighlight;
}

void VerticalView::setShowLinkHighlight(bool show)
{
    if (m_showLinkHighlight == show)
        return;

    m_showLinkHighlight = show;
    Q_EMIT showLinkHighlightChanged();
}

QColor VerticalView::linkHighlightColor() const
{
    return m_linkHighlightColor;
}

void VerticalView::setLinkHighlightColor(const QColor &color)
{
    if (m_linkHighlightColor == color)
        return;

    m_linkHighlightColor = color;
    Q_EMIT linkHighlightColorChanged();
}

void VerticalView::setError(const PopplerError::Error &error)
{
    if (m_error == error)
        return;

    m_error = error;
    Q_EMIT errorChanged();
}

void VerticalView::initializeDocument(const QString &path)
{
    if (m_document)
        m_document->disconnect(this);

    setError(PopplerError::NoError);

    m_document = QSharedPointer<PdfDocument>(new PdfDocument());
    m_document->setPath(path);

    /* A lot of things happens when we set the path property in
     * m_document. Need to check if an error has been emitted. */
    setError(m_document->error());

    if (m_error != PopplerError::NoError && m_error != PopplerError::DocumentLocked) {
        m_document.clear();

        // Stop doing anything below.
        return;
    }

    m_document.data()->setRenderHints(m_renderHints);

    Q_EMIT documentChanged();

    // Init zoom settings
    m_zoomSettings->init();
}

bool VerticalView::adjustZoomToWidth()
{
    if (!m_zoomSettings)
        return false;

    bool result;

    result = m_zoomSettings->adjustZoomToWidth();
    m_hasZoomChanged = result;

    return result;
}

bool VerticalView::adjustZoomToPage()
{
    if (!m_zoomSettings)
        return false;

    bool result;

    result = m_zoomSettings->adjustZoomToPage();
    m_hasZoomChanged = result;

    return result;
}

bool VerticalView::adjustAutomaticZoom()
{
    if (!m_zoomSettings)
        return false;

    bool result;

    result = m_zoomSettings->adjustAutomaticZoom();
    m_hasZoomChanged = result;

    return result;
}

void VerticalView::positionAtBeginning()
{
    m_parentFlickable->setProperty("contentY", 0);
}

void VerticalView::positionAtIndex(int index, qreal top, qreal left)
{
    SGTileItem* page = m_pages.value(index);

    if (!page) {
        PdfDocument* doc = m_document.data();

        const qreal &zoomFactor = m_zoomSettings->zoomFactor();
        qreal totalHeight = 0;

        for (int j=0; j < doc->pageCount(); ++j) {
            const QSize &s = doc->pageSize(j);
            QRect pageRect;

            if (m_rotation == Rotation::Rotate0 || m_rotation == Rotate180) {
                pageRect = QRect(
                            (this->width() - Twips::convertPointsToPixels(s.width(), zoomFactor)) * 0.5,
                            totalHeight,
                            Twips::convertPointsToPixels(s.width(), zoomFactor),
                            Twips::convertPointsToPixels(s.height(), zoomFactor)
                            );
            } else {
                pageRect = QRect(
                            (this->width() - Twips::convertPointsToPixels(s.height(), zoomFactor)) * 0.5,
                            totalHeight,
                            Twips::convertPointsToPixels(s.height(), zoomFactor),
                            Twips::convertPointsToPixels(s.width(), zoomFactor)
                            );
            }

            if (j == index) {
                page = createPage(j, pageRect);
                break;
            }

            totalHeight += (pageRect.height() + m_spacing);
        }
    }

    m_parentFlickable->setProperty("contentY", page->x() + left);
    m_parentFlickable->setProperty("contentY", page->y() + top);
    setCurrentPageIndex(index);
}

void VerticalView::positionAtEnd()
{
    const int posY = this->height() - m_parentFlickable->property("height").toInt();

    m_parentFlickable->setProperty("contentY", posY);
}

QVariantMap VerticalView::linkAtPosition(int x, int y)
{
    QVariantMap result;
    int pageIndex = -1;
    SGTileItem* page;

    if (!m_pages.isEmpty()) {
        auto i = m_pages.begin();
        while (i != m_pages.end()) {
            page = i.value();

            if (page->area().contains(x, y)) {
                pageIndex = i.key();
                break;
            }

            i++;
        }
    }

    if (pageIndex == -1) {
        return QVariantMap();
    }

    QList<Poppler::Link *> links = m_document->pageLinks(pageIndex);
    Q_FOREACH (Poppler::Link *link, links) {
        bool rotatedBy90 = (m_rotation == VerticalView::Rotate90 ||
                            m_rotation == VerticalView::Rotate270);

        QRectF linkRect;
        int x1 = (!rotatedBy90 ? page->width() : page->height()) * link->linkArea().left();
        int y1 = (!rotatedBy90 ? page->height() : page->width()) * link->linkArea().top();
        int x2 = (!rotatedBy90 ? page->width() : page->height()) * link->linkArea().right();
        int y2 = (!rotatedBy90 ? page->height() : page->width()) * link->linkArea().bottom();
        int w  = page->width();
        int h  = page->height();

        switch (m_rotation) {
        case VerticalView::Rotate0:
            linkRect.setLeft   ( page->x() + x1 );
            linkRect.setTop    ( page->y() + y1 );
            linkRect.setRight  ( page->x() + x2 );
            linkRect.setBottom ( page->y() + y2 );
            break;
        case VerticalView::Rotate90:
            linkRect.setLeft   ( page->x() + w - y2 );
            linkRect.setTop    ( page->y() + x1     );
            linkRect.setRight  ( page->x() + w - y1 );
            linkRect.setBottom ( page->y() + x2     );
            break;
        case VerticalView::Rotate180:
            linkRect.setLeft   ( page->x() + w - x2 );
            linkRect.setTop    ( page->y() + h - y2 );
            linkRect.setRight  ( page->x() + w - x1 );
            linkRect.setBottom ( page->y() + h - y1 );
            break;
        case VerticalView::Rotate270:
            linkRect.setLeft   ( page->x() + y1     );
            linkRect.setTop    ( page->y() + h - x2 );
            linkRect.setRight  ( page->x() + y2     );
            linkRect.setBottom ( page->y() + h - x1 );
            break;
        }

        if (linkRect.contains(x, y)) {
            if (link->linkType() == Poppler::Link::LinkType::Goto) {
                auto l = static_cast<Poppler::LinkGoto*>(link);

                if (!l->isExternal()) {
                    Poppler::LinkDestination dest = l->destination();

                    result["pageIndex"] = dest.pageNumber() - 1;
                    result["top"] = dest.top();
                    result["left"] = dest.left();

                    break;
                }
            } else if (link->linkType() == Poppler::Link::LinkType::Browse) {
                auto l = static_cast<Poppler::LinkBrowse*>(link);

                result["url"] = l->url();

                break;
            }
        }
    }

    return result;
}

bool VerticalView::unlock(const QString &ownerPassword, const QString &userPassword)
{
    bool result = false;

    if (m_document) {
        result = m_document.data()->unlock(ownerPassword, userPassword);

        if (result) {
            Q_EMIT documentChanged(); // which trigger a layout update
        }
    }

    return result;
}

// FIXME: There's something wrong here. Improve the events management.
// e.g. change zoom mode and immediately scroll the view. We have ~40
// RenderTask requests for new rendering just two pages.
void VerticalView::updateLayout()
{
    if (!m_parentFlickable || !m_document || m_document.data()->isLocked())
        return;

    if (m_zoomSettings->zoomMode() == PdfZoom::FitWidth) {
        if (m_zoomSettings->adjustZoomToWidth(false))
            m_hasZoomChanged = true;
    }

    else if (m_zoomSettings->zoomMode() == PdfZoom::FitPage) {
        if (m_zoomSettings->adjustZoomToPage(false))
            m_hasZoomChanged = true;
    }

    else if (m_zoomSettings->zoomMode() == PdfZoom::Automatic) {
        if (m_zoomSettings->adjustAutomaticZoom(false))
            m_hasZoomChanged = true;
    }

    if (!m_pages.isEmpty() && (m_hasZoomChanged || m_hasRotationChanged || m_hasRenderHintsChanged)) {
        clearView();
        m_hasZoomChanged = false;
        m_hasRotationChanged = false;
        m_hasRenderHintsChanged = false;
    }

    // Just for convenience.
    QRect documentRect(this->boundingRect().toRect());

    // Update visible area
    QRect visibleRect(m_parentFlickable->property("contentX").toInt(),
                      m_parentFlickable->property("contentY").toInt(),
                      m_parentFlickable->width(),
                      m_parentFlickable->height());

    m_visibleArea = visibleRect.intersected(documentRect);

    // Update buffer area
    QRect bufferRect(m_visibleArea.left()   -  m_cacheBuffer,
                     m_visibleArea.top()    -  m_cacheBuffer,
                     m_visibleArea.width()  + (m_cacheBuffer * 2),
                     m_visibleArea.height() + (m_cacheBuffer * 2));

    m_bufferArea = bufferRect.intersected(documentRect);

    // Delete tiles that are outside the loading area
    if (!m_pages.isEmpty()) {
        auto i = m_pages.begin();
        while (i != m_pages.end()) {
            SGTileItem* page = i.value();

            // Ok - we still need this item.
            if (m_bufferArea.intersects(page->area())) {
                i++;
                continue;
            }

            RenderEngine::instance()->dequeueTask(page->id());

            m_overlays.value(i.key())->deleteLater();
            m_overlays.remove(i.key());

            m_decorations.value(i.key())->deleteLater();
            m_decorations.remove(i.key());

            page->deleteLater();
            i = m_pages.erase(i);
        }
    }


    PdfDocument* doc = m_document.data();

    const qreal &zoomFactor = m_zoomSettings->zoomFactor();


    int docWidth = 0;
    int totalHeight = 0;

    for (int i=0; i < doc->pageCount(); ++i) {
        if (m_rotation == Rotation::Rotate0 || m_rotation == Rotate180) {
            const int w = Twips::convertPointsToPixels(doc->pageSize(i).width(), zoomFactor);
            if (docWidth < w)
                docWidth = w;
        } else {
            const int h = Twips::convertPointsToPixels(doc->pageSize(i).height(), zoomFactor);
            if (docWidth < h)
                docWidth = h;
        }
    }

    for (int j=0; j < doc->pageCount(); ++j) {
        const QSize &s = doc->pageSize(j);
        QRect pageRect;

        if (m_rotation == Rotation::Rotate0 || m_rotation == Rotate180) {
            pageRect = QRect(
                        (docWidth - Twips::convertPointsToPixels(s.width(), zoomFactor)) * 0.5,
                        totalHeight,
                        Twips::convertPointsToPixels(s.width(), zoomFactor),
                        Twips::convertPointsToPixels(s.height(), zoomFactor)
                        );
        } else {
            pageRect = QRect(
                        (docWidth - Twips::convertPointsToPixels(s.height(), zoomFactor)) * 0.5,
                        totalHeight,
                        Twips::convertPointsToPixels(s.height(), zoomFactor),
                        Twips::convertPointsToPixels(s.width(), zoomFactor)
                        );
        }

        if (pageRect.intersects(m_bufferArea)) {
            createPage(j, pageRect);
        }

        totalHeight += (pageRect.height() + m_spacing);
    }

    setWidth(docWidth);
    setHeight(totalHeight);

    m_hasFlickableBeenResized = false;
    m_hasFlickableBeenScrolled = false;
}

void VerticalView::clearView()
{   
    Q_FOREACH(PageDecoration* decoration, m_decorations)
        decoration->deleteLater();

    Q_FOREACH(PageOverlay* overlay, m_overlays)
        overlay->deleteLater();

    Q_FOREACH(SGTileItem* page, m_pages)
        page->deleteLater();

    m_decorations.clear();
    m_overlays.clear();
    m_pages.clear();
}

void VerticalView::onFlickableScrolled()
{
    if (m_updateTimer.isActive())
        return;

    m_hasFlickableBeenScrolled = true;

    // Update current page index
    int newCurrentPageIndex = -1;
    int flickCenterY = m_parentFlickable->property("contentY").toInt() + m_parentFlickable->height() * 0.5;

    // First attempt: check at the exact center
    for (auto i = m_pages.begin(); i != m_pages.end(); ++i) {
        QRect pageRect = i.value()->area();
        QPoint flickableCenter(this->width() * 0.5, flickCenterY);

        if (pageRect.contains(flickableCenter)) {
            newCurrentPageIndex = i.key();
            break;
        }
    }

    // currentPageIndex not found
    // Second attempt: sum the spacing to flickCenterY, so we're sure we'll find a page this time.
    if (newCurrentPageIndex == -1) {
        flickCenterY += m_spacing;

        for (auto i = m_pages.begin(); i != m_pages.end(); ++i) {
            QRect pageRect = i.value()->area();
            QPoint flickableCenter(this->width() * 0.5, flickCenterY);

            if (pageRect.contains(flickableCenter)) {
                newCurrentPageIndex = i.key();
                break;
            }
        }
    }

    // Set the value: -1 = current page not found
    setCurrentPageIndex(newCurrentPageIndex);

    // updateLayout() is triggered by the timer
    m_updateTimer.setSingleShot(true);
    m_updateTimer.start(UPDATE_TIMER_MSECS);
}

void VerticalView::onFlickableSizeChanged()
{
    m_hasFlickableBeenResized = true;
    updateLayout();
}

void VerticalView::onRenderHintsChanged()
{
    if (!m_document || m_document.data()->isLocked())
        return;

    m_document.data()->setRenderHints(m_renderHints);

    m_hasRenderHintsChanged = true;
    updateLayout();
}

SGTileItem *VerticalView::createPage(int index, const QRect &pageRect)
{
    SGTileItem* page;

    if (!m_pages.contains(index)) {
        page = new SGTileItem(pageRect, m_zoomSettings->zoomFactor(), RenderEngine::getNextId(), this);
        m_pages.insert(index, page);
        RenderEngine::instance()->enqueueTask(createTask(pageRect, index, page->id()));

        auto decoration = new PageDecoration(page);
        decoration->setZ(-1000);
        decoration->setWidth(page->width());
        decoration->setHeight(page->height());
        m_decorations.insert(index, decoration);

        auto overlay = new PageOverlay(this, index, page);
        overlay->setZ(1000);
        overlay->setWidth(page->width());
        overlay->setHeight(page->height());
        m_overlays.insert(index, overlay);

    } else {
        page = m_pages.value(index);
    }

    return page;
}

PdfRenderTask* VerticalView::createTask(const QRect &rect, int pageIndex, int id) const
{
    auto task = new PdfRenderTask();
    task->setId(id);
    task->setDocument(m_document);
    task->setPage(pageIndex);
    task->setRotation(m_rotation);
    task->setArea(QRect(0, 0, rect.width(), rect.height()));
    task->setZoom(m_zoomSettings->zoomFactor());
    return task;
}

void VerticalView::slotTaskRenderFinished(AbstractRenderTask *task, QImage img)
{
    if (task->type() == RttPdfPage) {
        int id = task->id();

        Q_FOREACH(SGTileItem* page, m_pages) {
            if (page->id() == id) {
                page->setData(img);
                break;
            }
        }
    }
}
