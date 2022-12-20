/*
 * Copyright (C) 2015 Stefano Verzegnassi
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
import Lomiri.Components.Themes.Ambiance 1.3 as Theme

TextField {
    id: textField

    // FIXME: This property is meant to be read-only, but we need to set its value herein.
    property bool expanded: false

    property Component popover

    secondaryItem: AbstractButton {
        id: popoverButton

        height: parent.height
        visible: !textField.highlighted
        width: visible ? height : 0

        onClicked: {
            textField.expanded = !textField.expanded

            if (textField.expanded) {
                var dialog = PopupUtils.open(textField.popover, popoverButton)

                dialog.Component.onDestruction.connect(function() {
                    textField.expanded = false
                })
            }
        }

        VerticalDivider { anchors { top: parent.top; bottom: parent.bottom; left: parent.left } }

        Rectangle {
            anchors.fill: parent
            color: theme.palette.selected.background
            visible: textField.expanded
        }

        Icon {
            width: units.gu(2); height: width
            anchors.centerIn: parent

            name: "go-down"
            color: "Grey"
            rotation: textField.expanded ? 180 : 0
            Behavior on rotation { LomiriNumberAnimation { } }
        }
    }

    default property alias textFieldData: textFieldContent.data
    Item {
        id: textFieldContent
        anchors {
            fill: parent
            rightMargin: popoverButton.width
        }
    }

    /* style */
    style: Theme.TextFieldStyle {
        background: LomiriShape {
            anchors.fill: parent
            aspect: LomiriShape.DropShadow
        }
    }
}
