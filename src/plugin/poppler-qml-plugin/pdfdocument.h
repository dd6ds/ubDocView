#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <QObject>
#include <QSharedPointer>
#include <poppler/qt5/poppler-qt5.h>

#include "../../app/renderengine.h"
#include "pdftocmodel.h"
#include "pdferror.h"

class PdfDocument : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PdfDocument)
    Q_ENUMS(Rotation)
    Q_ENUMS(DocumentInfo)
    Q_ENUMS(RenderHint)
    Q_FLAGS(RenderHints)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
    Q_PROPERTY(QObject* tocModel READ tocModel NOTIFY tocModelChanged)
    Q_PROPERTY(PopplerError::Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(RenderHints renderHints READ renderHints WRITE setRenderHints NOTIFY renderHintsChanged)

public:
    explicit PdfDocument();
    ~PdfDocument();

    enum Rotation {
        Rotate0 = Poppler::Page::Rotate0,
        Rotate90 = Poppler::Page::Rotate90,
        Rotate180 = Poppler::Page::Rotate180,
        Rotate270 = Poppler::Page::Rotate270,
    };

    enum DocumentInfo {
        Title,
        Subject,
        Author,
        Creator,
        Producer,
        CreationDate,
        ModifiedDate
    };

    enum RenderHint {
        Antialiasing  = 0x00000001,
        TextAntialiasing = 0x00000002,
        TextHinting = 0x00000004,
        TextSlightHinting = 0x00000008,
        OverprintPreview = 0x00000010,
        ThinLineSolid = 0x00000020,
        ThinLineShape = 0x00000040
    };
    Q_DECLARE_FLAGS(RenderHints, RenderHint)

    QString path() const;
    void setPath(const QString &pathName);

    bool isLocked();

    int pageCount();

    QImage paintPage(int pageIndex, const qreal &zoom, QRect rect, Rotation rotate = Rotate0) const;

    QList<Poppler::Link *> pageLinks(int pageIndex) const;

    Q_INVOKABLE QDateTime getDocumentDate(QString data);
    Q_INVOKABLE QString getDocumentInfo(QString data);
    Q_INVOKABLE QVariant documentInfo(DocumentInfo info);

    Q_INVOKABLE bool unlock(const QString &ownerPassword, const QString &userPassword);

    QObject *tocModel() const;

    QSize pageSize(int index) const;

    PopplerError::Error error() const;

    RenderHints renderHints() const;
    void setRenderHints(const RenderHints hints);

Q_SIGNALS:
    void pathChanged();
    void pageCountChanged();
    void tocModelChanged();
    void errorChanged();
    void renderHintsChanged();

private Q_SLOTS:
    void updateRenderHints();

private:
    bool loadDocument();
    void setError(const PopplerError::Error &error);
    void completeIntialization();

private:
    QString m_path;
    PdfTocModel* m_tocModel;
    PopplerError::Error m_error;
    RenderHints m_renderHints;

    QHash<int, QList<Poppler::Link *>> m_links;

    QSharedPointer<Poppler::Document> m_popDocument;
};

#endif // PDFDOCUMENT_H
