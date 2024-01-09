/*
 * Copyright (C) 2014-2016 Canonical, Ltd.
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
import QtQuick.Layouts 1.1
import Lomiri.Components.Popups 1.3
import DocumentViewer 1.0

PageHeader {
    id: rootItem

    property alias activityRunning: activity.running
    property Page page: null

    contents: ListItemLayout {
        anchors.centerIn: parent

        ActivityIndicator {
            id: activity
            SlotsLayout.position: SlotsLayout.Leading
            visible: running
        }

        title {
            font.weight: Font.DemiBold
            elide: Text.ElideMiddle
            text: rootItem.title
        }

        subtitle.text: file.mimetype.description
    }

    trailingActionBar.actions: [
        Action {
            id: startPresentation
            objectName:"presentationmode"
            iconName: "slideshow"
            text: i18n.tr("Presentation")
            enabled: !rootItem.page.isPresentationMode
            visible: enabled
            shortcut: "F5"
            onTriggered: rootItem.page.isPresentationMode = true
        },
        Action {
            id: exitPresentation
            iconName: "cancel"
            enabled: rootItem.page.isPresentationMode
            visible: enabled
            text: i18n.tr("Exit presentation mode")
            shortcut: StandardKey.Cancel
            onTriggered: rootItem.page.isPresentationMode = false
        },
        Action {
            iconName: "night-mode"
            text: mainView.nightModeEnabled ? i18n.tr("Disable night mode") : i18n.tr("Enable night mode")
            onTriggered: mainView.nightModeEnabled = !mainView.nightModeEnabled
            visible: mainView.theme.name != 'Lomiri.Components.Themes.SuruDark' ||
                     mainView.previousTheme != 'Lomiri.Components.Themes.SuruDark' &&
                     mainView.nightModeEnabled
        },

        Action {
            objectName: "detailsAction"
            text: i18n.tr("Details")
            iconName: "info"
            enabled: !rootItem.page.isPresentationMode
            visible: enabled
            onTriggered: pageStack.push(Qt.resolvedUrl("../common/DetailsPage.qml"))
        }
    ]
}
