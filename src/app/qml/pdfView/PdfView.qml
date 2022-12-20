/*
 * Copyright (C) 2013-2015 Canonical, Ltd.
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
import DocumentViewer.PDF 2.0 as PDF
import DocumentViewer 1.0

import "../common"
import "../common/utils.js" as Utils

// FIXME: After a resizing of the window, keep the current page visible.

Page {
    id: pdfPage
    title: DocumentViewer.getFileBaseNameFromPath(file.path)

    header: PageHeader {
        flickable: pdfView

        trailingActionBar.actions: [ searchText, goToPage, /*startPresentation,*/ nightModeToggle, fileDetails, rotateRight, rotateLeft ]

        contents: ListItemLayout {
            anchors.centerIn: parent

            title {
                font.weight: Font.DemiBold
                text: pdfPage.title
            }

            subtitle {
                textSize: Label.Small
                // TRANSLATORS: the first argument (%1) refers to the page currently shown on the screen,
                // while the second one (%2) refers to the total pages count.
                text: i18n.tr("Page %1 of %2").arg(pdfView.currentPageIndex + 1).arg(pdfView.pagesCount)
            }

            ZoomSelector {
                SlotsLayout.position: SlotsLayout.Trailing
                view: pdfView
                visible: DocumentViewer.desktopMode || mainView.wideWindow
            }
        }
    }

    // Reset night mode shader settings when closing the page
    // Component.onDestruction: mainView.nightModeEnabled = false

    // FIXME: TODO: Broken with the new PDF plugin
    /*
    Keys.onPressed: {
        if (event.key == Qt.Key_F5) { pageStack.push(Qt.resolvedUrl("./PdfPresentation.qml"), {'poppler': poppler}); }
    }
    */

    ScalingPinchArea {
        id: pinchArea
        objectName: "pinchArea"

        anchors {
            top: pdfPage.header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        clip: true

        enabled: !pdfView.moving

        targetFlickable: pdfView
        onTotalScaleChanged: targetFlickable.updateContentSize(totalScale)

        maximumZoom: pdfView.zoomSettings.maximumZoom
        minimumZoom: {
            if (DocumentViewer.desktopMode || mainView.wideWindow)
                return pdfView.zoomSettings.minimumZoom


            return pdfView.zoomSettings.valueAutomaticZoom
        }

        Binding {
            when: !pinchArea.pinch.active
            target: pinchArea
            property: "zoomValue"
            value: pdfView.zoomSettings.zoomFactor
        }

        Rectangle {
            // Since UITK 1.3, the MainView background is white.
            // We need to set a different color, otherwise pages
            // boundaries are not visible.
            anchors.fill: parent
            color: "#f5f5f5"
        }

        ScrollView {
            anchors.fill: parent

            PDF.Viewer {
                id: pdfView
                objectName: "pdfView"
                anchors.fill: parent

                function updateContentSize(tgtScale) {
                    zoomSettings.zoomFactor = tgtScale
                }

                renderHints: PDF.Document.Antialiasing | PDF.Document.TextAntialiasing

                linkHighlightColor: LomiriColors.orange

                spacing: units.gu(4)

                documentPath: file.path
                clip: false

                Component.onCompleted: {
                    // WORKAROUND: Fix for wrong grid unit size
                    flickDeceleration = 1500 * units.gridUnit / 8
                    maximumFlickVelocity = 2500 * units.gridUnit / 8

                    var t = pdfView.document.documentInfo(PDF.Document.Title);
                    if (t) {
                        pdfPage.title = t
                    }
                }

                onErrorChanged: {
                    // TODO: Error management
                    console.log(pdfView.error)

                    switch(pdfView.error) {
                    case PDF.Error.DocumentLocked:
                        PopupUtils.open(Qt.resolvedUrl("DocumentLockedDialog.qml"), pdfPage)
                        break;
                    }
                }

                // FIXME: TODO: Not the best management for this. See if/how it can be improved.
                property var hint
                onLinkHovered: {
                    if (!hint) {
                        var hintComponent = Qt.createComponent("LinkHint.qml");
                        hint = hintComponent.createObject(pdfView, { "x": mouseX, "y": mouseY - units.gu(3), "linkInfo": linkInfo })
                    }
                }
                onIsLinkHoveredChanged: {
                    if (!isLinkHovered && hint)
                        hint.destroy()
                }

                onLinkClicked: {
                    if (!isTouch) {
                        if (linkInfo.url) {
                            Qt.openUrlExternally(linkInfo.url)
                        } else {
                            pdfView.positionAtIndex(linkInfo.pageIndex, linkInfo.top, linkInfo.left)
                        }
                    }
                }

                onLinkPressAndHold: {
                    if (isTouch) {
                        PopupUtils.open(Qt.resolvedUrl("OpenLinkDialog.qml"), pdfPage, { linkInfo: linkInfo })
                    }
                }

                ScalingMouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    enabled: !pdfView.moving

                    targetFlickable: pdfView
                    onTotalScaleChanged: targetFlickable.updateContentSize(totalScale)

                    thresholdZoom: minimumZoom + (maximumZoom - minimumZoom) * 0.75
                    maximumZoom: {
                        if (DocumentViewer.desktopMode || mainView.wideWindow)
                            return 3.0

                        return minimumZoom * 3
                    }
                    minimumZoom: {
                        if (DocumentViewer.desktopMode || mainView.wideWindow)
                            return pdfView.zoomSettings.minimumZoom


                        return pdfView.zoomSettings.valueAutomaticZoom
                    }

                    Binding {
                        target: mouseArea
                        property: "zoomValue"
                        value: pdfView.zoomSettings.zoomFactor
                    }
                }
            }
        }
    }

    BottomEdge {
        id: contentsBottomEdge

        // WORKAROUND: BottomEdge component loads the page async while draging it
        // this cause a very bad visual.
        // To avoid that we create it as soon as the component is ready and keep
        // it invisible until the user start to drag it.
        // Fix from: http://bazaar.launchpad.net/~phablet-team/address-book-app/trunk/revision/528
        property var _realPage: null

        hint {
            action: Action {
                // TRANSLATORS: "Contents" refers to the "Table of Contents" of a PDF document.
                text: i18n.tr("Contents")
                iconName: "view-list-symbolic"  // FIXME: Needs ToC icon.
                onTriggered: contentsBottomEdge.commit()
            }
            flickable: pdfPage.flickable

            visible: contentsBottomEdge.visible
            enabled: contentsBottomEdge.enabled
        }

        contentComponent: Item {
            implicitWidth: contentsBottomEdge.width
            implicitHeight: contentsBottomEdge.height
            children: contentsBottomEdge._realPage
        }

        enabled: pdfView.document.tocModel.count > 0
        visible: enabled

        onCollapseCompleted: {
            _realPage = contentsPage.createObject(null)
            _realPage.header.leadingActionBar.actions = collapseAction
        }

        Component.onCompleted:  {
            _realPage = contentsPage.createObject(null)
            _realPage.header.leadingActionBar.actions = collapseAction
        }

        Action {
            id: collapseAction
            text: i18n.tr("Cancel")
            iconName: "down"
            onTriggered: contentsBottomEdge.collapse()
        }

        Component {
            id: contentsPage

            PdfContentsPage {
                width: contentsBottomEdge.width
                height: contentsBottomEdge.height
                enabled: contentsBottomEdge.status === BottomEdge.Committed
                active: contentsBottomEdge.status === BottomEdge.Committed
                visible: contentsBottomEdge.status !== BottomEdge.Hidden
            }
        }
    }

    /*** ACTIONS ***/

    Action {
        id: searchText
        iconName: "search"
        text: i18n.tr("Search")
        // onTriggered: pageMain.state = "search"
        //Disable it until we provide search in Poppler plugin.
        enabled: false
    }

    Action {
        id: goToPage
        objectName:"gotopage"
        iconName: "browser-tabs"
        text: i18n.tr("Go to page...")
        onTriggered: PopupUtils.open(Qt.resolvedUrl("PdfViewGotoDialog.qml"))
    }

    // FIXME: TODO: Broken with the new PDF plugin
    Action {
        id: startPresentation
        objectName:"presentationmode"
        iconName: "slideshow"
        text: i18n.tr("Presentation")
        onTriggered: pageStack.push(Qt.resolvedUrl("./PdfPresentation.qml"), {'poppler': poppler})
    }

    Action {
        id: nightModeToggle
        iconName: "night-mode"
        text: mainView.nightModeEnabled ? i18n.tr("Disable night mode") : i18n.tr("Enable night mode")
        onTriggered: mainView.nightModeEnabled = !mainView.nightModeEnabled
    }

    Action {
        id: fileDetails
        objectName: "detailsAction"
        text: i18n.tr("Details")
        iconName: "info"
        onTriggered: pageStack.push(Qt.resolvedUrl("../common/DetailsPage.qml"))
    }

    Action {
        id: rotateRight
        text: i18n.tr("Rotate 90° right")
        iconName: "rotate-right"
        onTriggered: {
            var r = pdfView.rotation
            r += 1

            if (r > 3)
                r = 0
            else if (r < 0)
                r = 3

            pdfView.rotation = r
        }
    }

    Action {
        id: rotateLeft
        text: i18n.tr("Rotate 90° left")
        iconName: "rotate-left"
        onTriggered: {
            var r = pdfView.rotation
            r -= 1

            if (r > 3)
                r = 0
            else if (r < 0)
                r = 3

            pdfView.rotation = r
        }
    }
}
