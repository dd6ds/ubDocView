import QtQuick 2.4
import DocumentViewer.PDF 2.0 as PDF

Flickable {
    id: rootFlickable

    property alias document:    view.document
    property alias zoomSettings: view.zoomSettings
    property alias cacheBuffer: view.cacheBuffer
    property alias error:       view.error
    property alias currentPageIndex: view.currentPageIndex
    property alias pagesCount: view.pagesCount
    property alias spacing: view.spacing
    property alias renderHints: view.renderHints
    property alias showLinkHighlight: view.showLinkHighlight
    property alias linkHighlightColor: view.linkHighlightColor
    property alias rotation: view.rotation

    property string documentPath: ""

    property bool isLinkHovered: false
    signal linkHovered(var linkInfo, var mouseX, var mouseY)    // mouseX and mouseY are relative to rootFlickable
    signal linkClicked(var isTouch, var linkInfo)
    signal linkDoubleClicked(var isTouch, var linkInfo)
    signal linkPressAndHold(var isTouch, var linkInfo)

    function adjustZoomToWidth()
    {
        var oldZoom = view.zoomSettings.zoomFactor
        view.adjustZoomToWidth()

        var zoomScale = view.zoomSettings.zoomFactor / oldZoom
        rootFlickable.contentX *= zoomScale
        rootFlickable.contentY *= zoomScale
    }

    function adjustZoomToPage()
    {
        var oldZoom = view.zoomSettings.zoomFactor
        view.adjustZoomToPage()

        var zoomScale = view.zoomSettings.zoomFactor / oldZoom
        rootFlickable.contentX *= zoomScale
        rootFlickable.contentY *= zoomScale
    }

    function adjustAutomaticZoom()
    {
        var oldZoom = view.zoomSettings.zoomFactor
        view.adjustAutomaticZoom()

        var zoomScale = view.zoomSettings.zoomFactor / oldZoom
        rootFlickable.contentX *= zoomScale
        rootFlickable.contentY *= zoomScale
    }

    function setZoom(newValue)
    {
        var zoomScale = newValue / view.zoomSettings.zoomFactor;
        view.zoomSettings.zoomFactor = newValue;

        rootFlickable.contentX *= zoomScale;
        rootFlickable.contentY *= zoomScale;
    }

    function moveView(axis, diff)
    {
        if (axis == "vertical") {
            var maxContentY = Math.max(0, rootFlickable.contentHeight - rootFlickable.height)
            rootFlickable.contentY = Math.max(0, Math.min(rootFlickable.contentY + diff, maxContentY ))
        } else {
            var maxContentX = Math.max(0, rootFlickable.contentWidth - rootFlickable.width)
            rootFlickable.contentX = Math.max(0, Math.min(rootFlickable.contentX + diff, maxContentX ))
        }
    }

    function positionAtBeginning() {
        view.positionAtBeginning()
    }

    function positionAtIndex(index, top, left) {
        if (typeof(top) === "undefined") top = 0
        if (typeof(left) === "undefined") left = 0

        view.positionAtIndex(index, top, left)
    }

    function positionAtEnd() {
        view.positionAtEnd()
    }

    function unlock(ownerPassword, userPassword) {
        return view.unlock(ownerPassword, userPassword)
    }

    onDocumentPathChanged: {
        if (documentPath)
            view.initializeDocument(documentPath)
    }

    // zoomFactor is not used here to set contentSize, since it's all managed
    // internally, in the LibreOffice.View component.
    contentHeight: Math.max(rootFlickable.height, view.height)
    contentWidth: Math.max(rootFlickable.width, view.width)

    boundsBehavior: Flickable.StopAtBounds

    PDF.VerticalView {
        id: view

        x: Math.max(0, (rootFlickable.width - view.width) * 0.5)
        y: Math.max(0, (rootFlickable.height - view.height) * 0.5)

        parentFlickable: rootFlickable
    }

    MouseArea {
        anchors.fill: parent
        // Explicitely set rootFlickable as parent, otherwise it's rootFlickable.contentItem
        parent: rootFlickable
        enabled: !rootFlickable.moving

        acceptedButtons: Qt.LeftButton

        hoverEnabled: true
        onPositionChanged: {
            var linkInfo = view.linkAtPosition(rootFlickable.contentX + mouse.x - view.x,
                                               rootFlickable.contentY + mouse.y - view.y);

            // Don't emit linkHovered() if it's a touch event.
            // Hover boxes are a mouse-related thing.
            if (touchArea.touchPressed)
                return

            if (linkInfo.pageIndex || linkInfo.url) {
                cursorShape = Qt.PointingHandCursor
                rootFlickable.linkHovered(linkInfo, mouse.x, mouse.y)
                rootFlickable.isLinkHovered = true
            } else {
                cursorShape = Qt.ArrowCursor
                rootFlickable.isLinkHovered = false
            }
        }

        onClicked: {
            var isTouch = touchArea.touchPressed
            var linkInfo = view.linkAtPosition(rootFlickable.contentX + mouse.x - view.x,
                                               rootFlickable.contentY + mouse.y - view.y);

            if (linkInfo.pageIndex || linkInfo.url) {
                rootFlickable.linkClicked(isTouch, linkInfo)
            }
        }

        onDoubleClicked: {
            var isTouch = touchArea.touchPressed
            var linkInfo = view.linkAtPosition(rootFlickable.contentX + mouse.x - view.x,
                                               rootFlickable.contentY + mouse.y - view.y);

            if (linkInfo.pageIndex || linkInfo.url) {
                rootFlickable.linkDoubleClicked(isTouch, linkInfo)
            }
        }

        onPressAndHold: {
            var isTouch = touchArea.touchPressed
            var linkInfo = view.linkAtPosition(rootFlickable.contentX + mouse.x - view.x,
                                               rootFlickable.contentY + mouse.y - view.y);

            if (linkInfo.pageIndex || linkInfo.url) {
                rootFlickable.linkPressAndHold(isTouch, linkInfo)
            }
        }

        // QML MultiPointTouchArea does not propagate events to its parent items. That
        // would have led to a more complex logic.
        // However, events returned by QML MouseArea signals don't inherit QEvent, so
        // we can not check for the type event.
        // We need to create our own component then.
        PDF.TouchDetectionArea {
            id: touchArea
            anchors.fill: parent
            enabled: !rootFlickable.moving
        }
    }
}
