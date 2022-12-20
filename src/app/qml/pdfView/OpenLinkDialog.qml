/*
 * Copyright (C) 2015, 2016 Stefano Verzegnassi
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

import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3
import QtQuick.Layouts 1.1

Dialog {
    id: openLinkDialog
    objectName:"PdfViewGotoDialog"

    property var linkInfo

    title: linkInfo.url ? i18n.tr("Open link externally") : i18n.tr("Go to page %1").arg(linkInfo.pageIndex + 1)
    text: i18n.tr("Are you sure?")

    RowLayout {
        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(-1)
        }

        Button {
            text: i18n.tr("Cancel")
            onClicked: PopupUtils.close(openLinkDialog)
            Layout.fillWidth: true
        }

        Button {
            text: linkInfo.url ? i18n.tr("Open") : i18n.tr("Go to destination")
            color: LomiriColors.green
            Layout.fillWidth: true

            onClicked: openLink()
        }
    }

    function openLink() {
        if (linkInfo.url) {
            Qt.openUrlExternally(linkInfo.url)
        } else {
            pdfView.positionAtIndex(linkInfo.pageIndex, linkInfo.top, linkInfo.left)
        }

        PopupUtils.close(openLinkDialog)
    }
}
