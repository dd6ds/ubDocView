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

import QtQuick 2.12
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3
import DocumentViewer.LibreOffice 1.0 as LibreOffice
import DocumentViewer 1.0

PageHeader {
    id: defaultHeader

    property var targetPage

    function nextPage() {
        nextPageAction.trigger()
    }

    function previousPage() {
        previousPageAction.trigger()
    }

    contents: ListItemLayout {
        anchors.centerIn: parent

        title {
            elide: Text.ElideMiddle
            font.weight: Font.DemiBold
            text: defaultHeader.title
        }

        subtitle {
            textSize: Label.Small
            text: {
                if (!targetPage.contentItem)
                    return i18n.tr("Loading...")

                switch(targetPage.contentItem.loDocument.documentType) {
                case LibreOffice.Document.TextDocument:
                    return i18n.tr("LibreOffice text document")
                case LibreOffice.Document.SpreadsheetDocument:
                    return i18n.tr("LibreOffice spread sheet")
                case LibreOffice.Document.PresentationDocument:
                    return i18n.tr("LibreOffice presentation")
                case LibreOffice.Document.DrawingDocument:
                    return i18n.tr("LibreOffice Draw document")
                case LibreOffice.Document.OtherDocument:
                    return i18n.tr("Unknown LibreOffice document")
                default:
                    return i18n.tr("Unknown document type")
                }
            }
        }

        ZoomSelector {
            SlotsLayout.position: SlotsLayout.Trailing
            view: targetPage.contentItem.loView
            visible: targetPage.contentItem && (DocumentViewer.desktopMode || mainView.wideWindow)
        }
    }

    trailingActionBar.actions: [
        Action {
            id: startPresentationAction
            objectName:"presentationmode"
            iconName: "slideshow"
            text: i18n.tr("Presentation")
            enabled: !targetPage.isPresentationMode
            visible: enabled
            shortcut: "F5"
            onTriggered: targetPage.isPresentationMode = true
        },
        Action {
            id: exitPresentationAction
            iconName: "cancel"
            enabled: targetPage.isPresentationMode
            visible: enabled
            text: i18n.tr("Exit presentation mode")
            shortcut: StandardKey.Cancel
            onTriggered: targetPage.isPresentationMode = false
        },
        Action {
            id: nextPageAction
            iconName: "next"
            enabled: targetPage.isPresentationMode && targetPage.isPresentation
            visible: enabled
            text: i18n.tr("Next page")
            shortcut: StandardKey.MoveToNextChar
            onTriggered: {
                if (!targetPage.contentItem.loView.inLastPage) {
                    targetPage.contentItem.loView.currentPart = (targetPage.contentItem.loView.currentPart + 1)
                }
            }
        },
        Action {
            id: previousPageAction
            iconName: "previous"
            enabled: targetPage.isPresentationMode && targetPage.isPresentation
            visible: enabled
            text: i18n.tr("Previous page")
            shortcut: StandardKey.MoveToPreviousChar
            onTriggered: {
                if (!targetPage.contentItem.loView.inFirstPage) {
                    targetPage.contentItem.loView.currentPart = (targetPage.contentItem.loView.currentPart - 1)
                }
            }
        },
        Action {
            // FIXME: Autopilot test broken... seems not to detect we're now using an ActionBar since the switch to UITK 1.3
            objectName: "gotopage"
            iconName: "browser-tabs"
            text: i18n.tr("Go to position…")
            visible: targetPage.contentItem.loDocument.documentType == LibreOffice.Document.TextDocument

            onTriggered: {
                var popupSettings = {
                    view: targetPage.contentItem.loView
                }

                PopupUtils.open(Qt.resolvedUrl("LOViewGotoDialog.qml"), targetPage, popupSettings)
            }
        },

        Action {
            iconName: "night-mode"
            text: mainView.nightModeEnabled ? i18n.tr("Disable night mode") : i18n.tr("Enable night mode")

            onTriggered: mainView.nightModeEnabled = !mainView.nightModeEnabled
        },

        Action {
            objectName: "detailsAction"
            text: i18n.tr("Details")
            iconName: "info"
            enabled: !targetPage.isPresentationMode
            visible: enabled

            onTriggered: pageStack.push(Qt.resolvedUrl("../common/DetailsPage.qml"))
        }
    ]

    Shortcut {
        sequence: "Return"
        onActivated: nextPageAction.trigger()
    }

    Shortcut {
        sequence: "Enter"
        onActivated: nextPageAction.trigger()
    }

    Shortcut {
        sequence: "Backspace"
        onActivated: previousPageAction.trigger()
    }
}
