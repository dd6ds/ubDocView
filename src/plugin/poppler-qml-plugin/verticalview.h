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

#ifndef VERTICALVIEW_H
#define VERTICALVIEW_H

#include <QQuickItem>
#include <QTimer>
#include <QSharedPointer>
#include <QVector>
#include <QHash>
#include <QRectF>
#include <QQmlContext>
#include <QQmlEngine>

#include "pdfdocument.h"
#include "pdferror.h"
#include "pdfrendertask.h"
#include "../../app/renderengine.h"

class SGTileItem;
class PageDecoration;
class PageOverlay;
class PdfZoom;

class VerticalView : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(Rotation)
    Q_PROPERTY(QQuickItem*              parentFlickable    READ parentFlickable    WRITE setParentFlickable    NOTIFY parentFlickableChanged)
    Q_PROPERTY(PdfDocument*             document           READ document                                       NOTIFY documentChanged)
    Q_PROPERTY(PdfZoom*                 zoomSettings       READ zoomSettings)
    Q_PROPERTY(int                      cacheBuffer        READ cacheBuffer        WRITE setCacheBuffer        NOTIFY cacheBufferChanged)
    Q_PROPERTY(int                      spacing            READ spacing            WRITE setSpacing            NOTIFY spacingChanged)
    Q_PROPERTY(PdfDocument::RenderHints renderHints        READ renderHints        WRITE setRenderHints        NOTIFY renderHintsChanged)
    Q_PROPERTY(PopplerError::Error      error              READ error                                          NOTIFY errorChanged)
    Q_PROPERTY(int                      currentPageIndex   READ currentPageIndex                               NOTIFY currentPageIndexChanged)
    Q_PROPERTY(int                      pagesCount         READ pagesCount                                     NOTIFY documentChanged)
    Q_PROPERTY(bool                     showLinkHighlight  READ showLinkHighlight  WRITE setShowLinkHighlight  NOTIFY showLinkHighlightChanged)
    Q_PROPERTY(QColor                   linkHighlightColor READ linkHighlightColor WRITE setLinkHighlightColor NOTIFY linkHighlightColorChanged)
    Q_PROPERTY(Rotation                 rotation           READ rotation           WRITE setRotation           NOTIFY rotationChanged)

    // Text search
    Q_PROPERTY(QString                  searchText              READ searchText              NOTIFY searchTextChanged)
    Q_PROPERTY(bool                     searchInProgress        READ searchInProgress        NOTIFY searchInProgressChanged)
    Q_PROPERTY(int                      searchResultsCount      READ searchResultsCount      NOTIFY searchResultsChanged)
    Q_PROPERTY(int                      currentSearchResult     READ currentSearchResult     NOTIFY currentSearchResultChanged)
    Q_PROPERTY(QColor                   searchHighlightColor    READ searchHighlightColor    WRITE setSearchHighlightColor    NOTIFY searchHighlightColorChanged)
    Q_PROPERTY(QColor                   currentSearchHighlightColor READ currentSearchHighlightColor WRITE setCurrentSearchHighlightColor NOTIFY searchHighlightColorChanged)

public:
    VerticalView(QQuickItem *parent = 0);
    ~VerticalView();

    enum Rotation {
        Rotate0 = 0,
        Rotate90 = 1,
        Rotate180 = 2,
        Rotate270 = 3
    };

    QQuickItem* parentFlickable() const;
    void        setParentFlickable(QQuickItem* flickable);

    Q_INVOKABLE void initializeDocument(const QString& path);

    PdfDocument* document() const;

    PdfZoom*    zoomSettings() const;

    int         cacheBuffer() const;    // in pixels
    void        setCacheBuffer(int cacheBuffer);

    int         spacing() const;    // in pixels
    void        setSpacing(int spacing);

    int         currentPageIndex() const;
    int         pagesCount() const;

    Rotation    rotation() const;
    void        setRotation(Rotation rotation);

    PdfDocument::RenderHints renderHints() const;
    void setRenderHints(const PdfDocument::RenderHints hints);

    bool showLinkHighlight() const;
    void setShowLinkHighlight(bool show);

    QColor linkHighlightColor() const;
    void setLinkHighlightColor(const QColor &color);

    PopplerError::Error error() const;

    Q_INVOKABLE bool adjustZoomToWidth();
    Q_INVOKABLE bool adjustZoomToPage();
    Q_INVOKABLE bool adjustAutomaticZoom();

    Q_INVOKABLE void positionAtBeginning();
    Q_INVOKABLE void positionAtIndex(int index, qreal top = 0, qreal left = 0);
    Q_INVOKABLE void positionAtEnd();

    Q_INVOKABLE QVariantMap linkAtPosition(int x, int y);

    Q_INVOKABLE bool unlock(const QString &ownerPassword, const QString &userPassword);

    // Text search API (exposed to QML)
    Q_INVOKABLE void search(const QString &text, bool caseSensitive = false);
    Q_INVOKABLE void clearSearch();
    Q_INVOKABLE void nextSearchResult();
    Q_INVOKABLE void previousSearchResult();

    QString searchText() const;
    bool    searchInProgress() const;
    int     searchResultsCount() const;
    int     currentSearchResult() const;   // 0-based, -1 = none

    QColor  searchHighlightColor() const;
    void    setSearchHighlightColor(const QColor &color);
    QColor  currentSearchHighlightColor() const;
    void    setCurrentSearchHighlightColor(const QColor &color);

    // Used by PageOverlay
    QList<QRectF> searchResultsForPage(int pageIndex) const;
    int     currentSearchResultPage() const;
    QRectF  currentSearchResultRect() const;

    // Maps a rect normalized to the unrotated page (0..1) to item coordinates
    // of a page item with the given (already rotated) size.
    QRectF  mapNormalizedRect(const QRectF &normalized, qreal width, qreal height) const;

Q_SIGNALS:
    void parentFlickableChanged();
    void documentChanged();
    void cacheBufferChanged();
    void spacingChanged();
    void errorChanged();
    void currentPageIndexChanged();
    void renderHintsChanged();
    void showLinkHighlightChanged();
    void linkHighlightColorChanged();
    void rotationChanged();
    void searchTextChanged();
    void searchInProgressChanged();
    void searchResultsChanged();
    void currentSearchResultChanged();
    void searchHighlightColorChanged();
    void searchFinished();

private Q_SLOTS:
    void searchNextChunk();
    void updateLayout();
    void onFlickableScrolled();
    void onFlickableSizeChanged();
    void onRenderHintsChanged();
    void slotTaskRenderFinished(AbstractRenderTask* task, QImage img);

private:

    QQuickItem*                 m_parentFlickable;
    QSharedPointer<PdfDocument> m_document;
    PdfZoom*                    m_zoomSettings;
    int                         m_cacheBuffer;
    int                         m_spacing;
    Rotation                    m_rotation;

    int                         m_currentPageIndex;

    QRect                       m_visibleArea;
    QRect                       m_bufferArea;

    PdfDocument::RenderHints    m_renderHints;

    bool                        m_showLinkHighlight;
    QColor                      m_linkHighlightColor;

    PopplerError::Error         m_error;

    QTimer                      m_updateTimer;

    QMap<int, SGTileItem*>      m_pages;
    QMap<int, PageDecoration*>  m_decorations;
    QMap<int, PageOverlay*>     m_overlays;

    bool                        m_hasZoomChanged;
    bool                        m_hasRotationChanged;
    bool                        m_hasFlickableBeenScrolled;
    bool                        m_hasFlickableBeenResized;
    bool                        m_hasRenderHintsChanged;

    // Text search state
    struct SearchHit {
        int    page;
        QRectF rect;    // normalized, unrotated
    };

    QString                     m_searchText;
    bool                        m_searchCaseSensitive;
    bool                        m_searchInProgress;
    int                         m_searchNextPage;       // next page to scan
    int                         m_searchStartPage;      // page the user was on when searching
    QVector<SearchHit>          m_searchHits;           // ordered by page
    QHash<int, QList<QRectF>>   m_searchResultsByPage;
    int                         m_currentSearchResult;
    QTimer                      m_searchTimer;
    QColor                      m_searchHighlightColor;
    QColor                      m_currentSearchHighlightColor;

private:
    void setSearchInProgress(bool inProgress);
    void setCurrentSearchResult(int index);
    void goToCurrentSearchResult();
    void resetSearch();

    void setCurrentPageIndex(int currentPageIndex);
    void setError(const PopplerError::Error &error);
    SGTileItem* createPage(int index, const QRect &pageRect);
    void clearView();

    PdfRenderTask *createTask(const QRect &rect, int pageIndex, int id) const;
};

#endif // VERTICALVIEW_H
