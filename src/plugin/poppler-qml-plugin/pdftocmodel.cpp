/*
 * Copyright (C) 2015, 2016
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

#include "pdftocmodel.h"
#include <QDomDocument>
#include <QDomElement>

#include <QDebug>
#include <QDomNamedNodeMap>
#include <QDomNode>

PdfTocModel::PdfTocModel(QAbstractListModel *parent):
    QAbstractListModel(parent)
{ }

void PdfTocModel::setDocument(const QSharedPointer<Poppler::Document>& document)
{
    m_document = document;
    fillModel();
}

QHash<int, QByteArray> PdfTocModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[PageIndexRole] = "pageIndex";
    roles[LevelRole] = "level";
    return roles;
}

int PdfTocModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return m_entries.count();
}

QVariant PdfTocModel::data(const QModelIndex & index, int role) const
{
    if (index.row() < 0 || index.row() > m_entries.count())
        return QVariant();

    const TocEntry &tocEntry = m_entries.at(index.row());

    switch (role) {
    case TitleRole:
        return tocEntry.title;
    case PageIndexRole:
        return tocEntry.pageIndex;
    case ParentNodeIndexRole:
        return tocEntry.parentNodeIndex;
    case LevelRole:
        return tocEntry.level;
    default:
        return 0;
    }
}

QVariantMap PdfTocModel::get(int index) const
{
    if (index < 0 || index > m_entries.length() - 1) {
        qWarning() << Q_FUNC_INFO << "Index not valid, return undefined";
        return QVariantMap();
    }

    const TocEntry &item = m_entries.at(index);

    QVariantMap map;
    map["title"] = item.title;
    map["pageIndex"] = item.pageIndex;
    map["parentNodeIndex"] = item.parentNodeIndex;
    map["level"] = item.level;

    return map;
}

void PdfTocModel::fillModel() {
    if (m_entries.count() != 0) {
        beginResetModel();
        m_entries.clear();
        endResetModel();

        Q_EMIT countChanged();
    }

    if (!m_document->outline().isEmpty()) {
        qDebug() << "[PDF] Parsing toc model";
        QVector<Poppler::OutlineItem> outline = m_document->outline();
        recursiveGetEntries(outline, 0);
    }
}

void PdfTocModel::recursiveGetEntries(const QVector<Poppler::OutlineItem> &items, int nodeLevel, int parentNodeIndex)
{
    for (const Poppler::OutlineItem &item : items) {
        TocEntry entry;
        entry.title = item.name();
        entry.parentNodeIndex = parentNodeIndex;
        entry.level = nodeLevel;

        QSharedPointer<const Poppler::LinkDestination> dest = item.destination();
        if (dest && !dest.isNull()) {
            entry.pageIndex = dest->pageNumber() - 1;
        }

        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_entries.append(entry);
        endInsertRows();

        Q_EMIT countChanged();

        // Look for children entries
        if (item.hasChildren()) {
            recursiveGetEntries(item.children(), nodeLevel + 1, m_entries.count() - 1);
        }
    }
}

PdfTocModel::~PdfTocModel()
{ }
