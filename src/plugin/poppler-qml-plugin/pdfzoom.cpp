/*
 * Copyright (C) 2016 Stefano Verzegnassi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pdfzoom.h"
#include "verticalview.h"
#include "pdfdocument.h"
#include "twips.h"

static qreal getZoomToFitWidth(const qreal &width, int documentWidth)
{
    return qreal(width / Twips::convertPointsToPixels(documentWidth, 1.0));
}

static qreal getZoomToFitHeight(const qreal &height, int documentHeight)
{
    return qreal(height / Twips::convertPointsToPixels(documentHeight, 1.0));
}

PdfZoom::PdfZoom(VerticalView *view)
    : QObject(view)
    , m_view(view)
    , m_zoomMode(ZoomMode::Manual)
    , m_zoomFactor(1.0)
    , m_minimumZoom(0.25)
    , m_maximumZoom(4.0)
{   }

PdfZoom::~PdfZoom()
{   }

PdfZoom::ZoomMode PdfZoom::zoomMode() const
{
    return m_zoomMode;
}

void PdfZoom::setZoomMode(const PdfZoom::ZoomMode zoomMode)
{
    if (m_zoomMode == zoomMode)
        return;

    m_zoomMode = zoomMode;
    Q_EMIT zoomModeChanged();
}

void PdfZoom::updateZoomValues()
{
    int maxWidth = 0;
    int maxHeight = 0;
    for (int i=0; i<m_view->document()->pageCount(); ++i) {
        int w, h;
        if (m_view->rotation() == VerticalView::Rotation::Rotate0 || m_view->rotation() == VerticalView::Rotate180) {
            h = m_view->document()->pageSize(i).height();
            w = m_view->document()->pageSize(i).width();
        } else {
            w = m_view->document()->pageSize(i).height();
            h = m_view->document()->pageSize(i).width();
        }

        if (h > maxHeight)
            maxHeight = h;

        if (w > maxWidth)
            maxWidth = w;
    }

    // Maximum zoom value to fit the width of the flickable
    m_valueFitWidthZoom = getZoomToFitWidth(m_view->parentFlickable()->width(), maxWidth);

    // Maximum zoom value that ensures that each page is fully visible in the view (without any
    // need for scrolling)
    m_valueFitPageZoom = qMin(getZoomToFitHeight(m_view->parentFlickable()->height(), maxHeight),
                              m_valueFitWidthZoom);

    // Like ZoomMode::FitWidth, but its maximum value is limited to a 1.0x factor
    m_valueAutomaticZoom = qMin(1.0, m_valueFitWidthZoom);

    Q_EMIT valueFitWidthZoomChanged();
    Q_EMIT valueFitPageZoomChanged();
    Q_EMIT valueAutomaticZoomChanged();
}

qreal PdfZoom::zoomFactor() const
{
    return m_zoomFactor;
}

void PdfZoom::setZoomFactor(const qreal zoom)
{
    if (m_zoomFactor == zoom || zoom < m_minimumZoom || zoom > m_maximumZoom)
        return;

    m_zoomFactor = zoom;

    if (m_zoomFactor != m_valueFitWidthZoom
            && m_zoomFactor != m_valueFitPageZoom
            && m_zoomFactor != m_valueAutomaticZoom)
        setZoomMode(PdfZoom::Manual);

    Q_EMIT zoomFactorChanged();
}

qreal PdfZoom::minimumZoom() const
{
    return m_minimumZoom;
}

/*
void PdfZoom::setMinimumZoom(const qreal newValue)
{
    if (m_minimumZoom == newValue)
        return;

    m_minimumZoom = newValue;
    Q_EMIT minimumZoomChanged();
}
*/

qreal PdfZoom::maximumZoom() const
{
    return m_maximumZoom;
}

/*
void PdfZoom::setMaximumZoom(const qreal newValue)
{
    if (m_maximumZoom == newValue)
        return;

    m_maximumZoom = newValue;
    Q_EMIT maximumZoomChanged();
}
*/

qreal PdfZoom::valueFitWidthZoom() const
{
    return m_valueFitWidthZoom;
}

qreal PdfZoom::valueFitPageZoom() const
{
    return m_valueFitPageZoom;
}

qreal PdfZoom::valueAutomaticZoom() const
{
    return m_valueAutomaticZoom;
}

bool PdfZoom::adjustZoomToWidth(bool changeMode)
{
    if (!m_view->document())
        return false;

    if (changeMode)
        setZoomMode(PdfZoom::FitWidth);

    int maxWidth = 0;
    for (int i=0; i<m_view->document()->pageCount(); ++i) {
        int w = m_view->document()->pageSize(i).width();
        if (w > maxWidth)
            maxWidth = w;
    }

    updateZoomValues();

    if (m_zoomFactor != m_valueFitWidthZoom) {
        setZoomFactor(m_valueFitWidthZoom);

        qDebug() << Q_FUNC_INFO << "- value:" << m_zoomFactor << "- changeMode:" << changeMode;
        return true;
    }

    return false;
}

bool PdfZoom::adjustZoomToPage(bool changeMode)
{
    if (!m_view->document())
        return false;

    if (changeMode)
        setZoomMode(PdfZoom::FitPage);

    updateZoomValues();

    if (m_zoomFactor != m_valueFitPageZoom) {
        setZoomFactor(m_valueFitPageZoom);

        qDebug() << Q_FUNC_INFO << "- value:" << m_zoomFactor << "- changeMode:" << changeMode;
        return true;
    }

    return false;
}

bool PdfZoom::adjustAutomaticZoom(bool changeMode)
{
    if (!m_view->document())
        return false;

    if (changeMode)
        setZoomMode(PdfZoom::Automatic);

    updateZoomValues();

    if (m_zoomFactor != m_valueAutomaticZoom) {
        setZoomFactor(m_valueAutomaticZoom);

        qDebug() << Q_FUNC_INFO << "- value:" << m_zoomFactor << "- changeMode:" << changeMode;
        return true;
    }

    return false;
}

void PdfZoom::init()
{
    setZoomMode(ZoomMode::Automatic);
}
