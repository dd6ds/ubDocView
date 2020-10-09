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

#include "pdfdocument.h"
#include "twips.h"

#include <poppler/qt5/poppler-qt5.h>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>

#include <QtConcurrent/QtConcurrent>

// TODO: Add QQuickAsyncImageProvider for thumbnails
// TODO: Handle protected documents

PdfDocument::PdfDocument():
    m_path("")
  , m_tocModel(nullptr)
  , m_error(PopplerError::NoError)
  , m_renderHints(0)
{
    connect(this, &PdfDocument::renderHintsChanged, this, &PdfDocument::updateRenderHints);
}

void PdfDocument::setPath(const QString &pathName)
{
    if (pathName.isEmpty())
        return;

    m_path = pathName;
    Q_EMIT pathChanged();

    loadDocument();
}

bool PdfDocument::isLocked()
{
    if (!m_popDocument)
        return true;

    return m_popDocument.data()->isLocked();
}

int PdfDocument::pageCount()
{
    if (!m_popDocument)
        return 0;

    return m_popDocument.data()->numPages();
}

QDateTime PdfDocument::getDocumentDate(QString data)
{
    if (!m_popDocument)
        return QDateTime();

    if (data == "CreationDate" || data == "ModDate")
        return m_popDocument->date(data);
    else
        return QDateTime();
}

QString PdfDocument::getDocumentInfo(QString data)
{
    if (!m_popDocument)
        return QString("");

    if (data == "Title" || data == "Subject" || data == "Author" || data == "Creator" || data == "Producer")
        return m_popDocument->info(data);
    else
        return QString("");
}

QVariant PdfDocument::documentInfo(PdfDocument::DocumentInfo info)
{
    if (!m_popDocument)
        return QVariant();

    switch (info) {
    case Title:
        return m_popDocument->info("Title");
    case Subject:
        return m_popDocument->info("Subject");
    case Author:
        return m_popDocument->info("Author");
    case Creator:
        return m_popDocument->info("Creator");
    case Producer:
        return m_popDocument->info("Producer");
    case CreationDate:
        return m_popDocument->date("CreationDate");
    case ModifiedDate:
        return m_popDocument->date("ModDate");
    default:
        return QVariant();
    }
}

bool PdfDocument::unlock(const QString &ownerPassword, const QString &userPassword)
{
    bool result = false;

    m_popDocument.data()->unlock(ownerPassword.toLatin1(), userPassword.toLatin1());

    // It seems that Poppler::Document::unlock() returns true even with the wrong password :/
    result = !isLocked();

    if (result) {
        completeIntialization();
    }

    return result;
}

QObject *PdfDocument::tocModel() const
{
    return m_tocModel;
}

QSize PdfDocument::pageSize(int index) const
{
    QSize s;

    if (m_popDocument) {
        Poppler::Page* page = m_popDocument.data()->page(index);

        s = page->pageSize();

        delete page;
    }

    return s;
}

PopplerError::Error PdfDocument::error() const
{
    return m_error;
}

PdfDocument::RenderHints PdfDocument::renderHints() const
{
    return m_renderHints;
}

void PdfDocument::setRenderHints(const RenderHints hints)
{
    if (m_renderHints == hints)
        return;

    m_renderHints = hints;
    Q_EMIT renderHintsChanged();
}

void PdfDocument::updateRenderHints()
{
    if (!m_popDocument)
        return;

    Poppler::Document* doc = m_popDocument.data();

    doc->setRenderHint(Poppler::Document::RenderHint::Antialiasing,      m_renderHints & PdfDocument::Antialiasing);
    doc->setRenderHint(Poppler::Document::RenderHint::TextAntialiasing,  m_renderHints & PdfDocument::TextAntialiasing);
    doc->setRenderHint(Poppler::Document::RenderHint::TextHinting,       m_renderHints & PdfDocument::TextHinting);
    doc->setRenderHint(Poppler::Document::RenderHint::TextSlightHinting, m_renderHints & PdfDocument::TextSlightHinting);
    doc->setRenderHint(Poppler::Document::RenderHint::OverprintPreview,  m_renderHints & PdfDocument::OverprintPreview);
    doc->setRenderHint(Poppler::Document::RenderHint::ThinLineSolid,     m_renderHints & PdfDocument::ThinLineSolid);
    doc->setRenderHint(Poppler::Document::RenderHint::ThinLineShape,     m_renderHints & PdfDocument::ThinLineShape);
}

QImage PdfDocument::paintPage(int pageIndex, const qreal &zoom, QRect rect, Rotation rotate) const
{
    QImage result;

    if (m_popDocument) {
        if (pageIndex >= 0 || pageIndex < m_popDocument.data()->numPages()) {
            Poppler::Page* page = m_popDocument.data()->page(pageIndex);

            result = page->renderToImage(
                        DEFAULT_DPI * Twips::getUnitsRatio() * zoom,
                        DEFAULT_DPI * Twips::getUnitsRatio() * zoom,
                        rect.x(), rect.y(), rect.width(), rect.height(),
                        Poppler::Page::Rotation(rotate));

            delete page;
        }
    }

    return result;
}

QList<Poppler::Link *> PdfDocument::pageLinks(int pageIndex) const
{
    QList<Poppler::Link *> result;

    if (!m_links.empty())
        result = m_links.value(pageIndex);

    return result;
}

bool PdfDocument::loadDocument()
{
    qDebug() << "Loading document...";

    if (m_path.isEmpty()) {
        qDebug() << "Can't load the document, path is empty.";
        setError(PopplerError::FileNotFound);
        return false;
    }

    Poppler::Document* doc = Poppler::Document::load(m_path);
    m_popDocument = QSharedPointer<Poppler::Document>(doc);

    if (!doc || doc->isLocked()) {
        qDebug() << "ERROR : Can't open the document located at " + m_path;
        setError(PopplerError::DocumentLocked);

        return false;
    }

    setError(PopplerError::NoError);
    qDebug() << "Document loaded successfully !";

    completeIntialization();

    return true;

}

void PdfDocument::setError(const PopplerError::Error &error)
{
    if (m_error == error)
        return;

    m_error = error;
    Q_EMIT errorChanged();
}

void PdfDocument::completeIntialization()
{
    updateRenderHints();

    // Init toc model
    m_tocModel = new PdfTocModel;
    m_tocModel->setDocument(m_popDocument);
    Q_EMIT tocModelChanged();

    Q_EMIT pageCountChanged();

    // WORKAROUND: Getting links from pages seems a bit trivial and causes SIGFAULTs.
    // Do it once at document initialization and cache them for any future request.
    for (int i=0; i<m_popDocument->numPages(); ++i) {
        Poppler::Page* page = m_popDocument->page(i);

        QList<Poppler::Link *> l = page->links();
        m_links.insert(i, l);

        delete page;
    }
}

PdfDocument::~PdfDocument()
{
    Q_FOREACH(QList<Poppler::Link *> list, m_links)
        qDeleteAll(list);

    delete m_tocModel;
}

QString PdfDocument::path() const
{
    return m_path;
}
