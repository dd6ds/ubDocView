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

#ifndef PDFZOOM_H
#define PDFZOOM_H

#include <QObject>
#include <QSharedPointer>

class VerticalView;

class PdfZoom : public QObject
{
    Q_OBJECT
    Q_ENUMS(ZoomMode)
    Q_PROPERTY(ZoomMode zoomMode READ zoomMode NOTIFY zoomModeChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(qreal minimumZoom READ minimumZoom CONSTANT)     //WRITE setMinimumZoom NOTIFY minimumZoomChanged)
    Q_PROPERTY(qreal maximumZoom READ maximumZoom CONSTANT)     //WRITE setMaximumZoom NOTIFY maximumZoomChanged)
    Q_PROPERTY(qreal valueFitWidthZoom READ valueFitWidthZoom NOTIFY valueFitWidthZoomChanged)
    Q_PROPERTY(qreal valueFitPageZoom READ valueFitPageZoom NOTIFY valueFitPageZoomChanged)
    Q_PROPERTY(qreal valueAutomaticZoom READ valueAutomaticZoom NOTIFY valueAutomaticZoomChanged)

public:
    PdfZoom(VerticalView *view);
    ~PdfZoom();

    // TODO: FIXME: They should be:
    // - Manual - as lok plugin
    // - FitWidth - as lok plugin
    // - FitPage - min(FitWidth, FitHeight)
    // - Automatic - min(FitWidth, 1.0)
    enum ZoomMode {
        Manual = 0x0,
        FitWidth = 0x1,
        FitPage = 0x2,
        Automatic = 0x4
    };

    ZoomMode    zoomMode() const;
    qreal       zoomFactor() const;
    void        setZoomFactor(const qreal zoom);

    qreal       minimumZoom() const;
//    void        setMinimumZoom(const qreal newValue);

    qreal       maximumZoom() const;
//    void        setMaximumZoom(const qreal newValue);

    qreal       valueFitWidthZoom() const;
    qreal       valueFitPageZoom() const;
    qreal       valueAutomaticZoom() const;

    bool adjustZoomToWidth(bool changeMode = true);
    bool adjustZoomToPage(bool changeMode = true);
    bool adjustAutomaticZoom(bool changeMode = true);

    void init();

Q_SIGNALS:
    void zoomModeChanged();
    void zoomModesAvailableChanged();
    void zoomFactorChanged();
//    void minimumZoomChanged();
//    void maximumZoomChanged();
    void valueFitWidthZoomChanged();
    void valueFitPageZoomChanged();
    void valueAutomaticZoomChanged();

private:
    VerticalView* m_view;

    ZoomMode m_zoomMode;

    qreal m_zoomFactor;

    qreal m_minimumZoom;
    qreal m_maximumZoom;

    qreal m_valueFitWidthZoom;
    qreal m_valueFitPageZoom;
    qreal m_valueAutomaticZoom;

private:
    void setZoomMode(const ZoomMode zoomMode);
    void updateZoomValues();
};

#endif // PDFZOOM_H
