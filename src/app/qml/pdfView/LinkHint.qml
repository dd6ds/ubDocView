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

import QtQuick 2.4
import Ubuntu.Components 1.3

Rectangle {
    id: rootItem
    height: units.gu(2)
    width: hintText.paintedWidth + units.gu(2)
    color: "white"

    property var linkInfo

    border {
        width: units.dp(1)
        color: "black"
    }

    Label {
        id: hintText
        anchors.centerIn: parent
        text: linkInfo.url ? i18n.tr("Open link externally: %1").arg(linkInfo.url)
                           : i18n.tr("Go to page %1").arg(linkInfo.pageIndex + 1)
    }

    Timer {
        interval: 2000
        running: true
        onTriggered: rootItem.destroy()
    }
}
