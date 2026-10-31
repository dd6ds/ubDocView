/*
 * Copyright (C) 2026
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

// Header shown while searching text inside a PDF document.
PageHeader {
    id: searchHeader

    // The PDF.Viewer to search in
    property var view

    readonly property bool hasResults: view && view.searchResultsCount > 0
    readonly property bool notFound: view && searchField.text.length > 0
                                     && view.searchText === searchField.text
                                     && !view.searchInProgress
                                     && view.searchResultsCount === 0

    signal closeRequested()

    function focusField() {
        searchField.forceActiveFocus()
        searchField.selectAll()
    }

    // Run the search right now (skipping the typing delay), or go to the next
    // match if the current text has already been searched.
    function acceptSearch() {
        if (!view)
            return

        if (searchField.text !== view.searchText) {
            searchDelay.stop()
            view.search(searchField.text)
        } else {
            view.nextSearchResult()
        }
    }

    leadingActionBar.actions: Action {
        iconName: "back"
        text: i18n.tr("Close search")
        onTriggered: searchHeader.closeRequested()
    }

    trailingActionBar {
        numberOfSlots: 2
        actions: [
            Action {
                iconName: "down"
                text: i18n.tr("Next match")
                enabled: searchHeader.hasResults
                onTriggered: view.nextSearchResult()
            },
            Action {
                iconName: "up"
                text: i18n.tr("Previous match")
                enabled: searchHeader.hasResults
                onTriggered: view.previousSearchResult()
            }
        ]
    }

    contents: TextField {
        id: searchField
        objectName: "pdfSearchField"

        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        primaryItem: Icon {
            height: units.gu(2); width: height
            name: "search"
        }

        secondaryItem: Row {
            spacing: units.gu(1)
            height: parent.height

            ActivityIndicator {
                anchors.verticalCenter: parent.verticalCenter
                height: units.gu(2); width: height
                running: view && view.searchInProgress
                visible: running
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter
                visible: searchField.text.length > 0 && view && view.searchText.length > 0
                textSize: Label.Small
                color: searchHeader.notFound ? theme.palette.normal.negative
                                             : theme.palette.normal.backgroundSecondaryText
                text: {
                    if (!view)
                        return ""

                    if (searchHeader.notFound)
                        return i18n.tr("No matches")

                    // TRANSLATORS: %1 is the index of the selected match, %2 the total number of matches
                    return i18n.tr("%1 of %2")
                               .arg(view.currentSearchResult + 1)
                               .arg(view.searchResultsCount)
                }
            }
        }

        placeholderText: i18n.tr("Search in document...")
        inputMethodHints: Qt.ImhNoPredictiveText

        onTextChanged: {
            if (text.length === 0) {
                searchDelay.stop()
                if (view)
                    view.clearSearch()
            } else {
                searchDelay.restart()
            }
        }

        onAccepted: searchHeader.acceptSearch()

        Keys.onPressed: {
            if (event.key === Qt.Key_Escape) {
                searchHeader.closeRequested()
                event.accepted = true
            } else if ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter)
                       && (event.modifiers & Qt.ShiftModifier)) {
                if (view)
                    view.previousSearchResult()
                event.accepted = true
            }
        }

        onVisibleChanged: if (visible) searchHeader.focusField()
    }

    // Search while typing, but wait until the user pauses.
    Timer {
        id: searchDelay
        interval: 400
        onTriggered: {
            if (view && searchField.text.length > 0 && searchField.text !== view.searchText)
                view.search(searchField.text)
        }
    }

    // Reset the field (and thereby the highlights) when the search header is closed.
    onVisibleChanged: {
        if (!visible) {
            searchDelay.stop()
            searchField.text = ""
        }
    }
}
