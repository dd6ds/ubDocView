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
import Ubuntu.Components.Popups 1.3
import QtQuick.Layouts 1.1

Dialog {
    id: unlockDialog

    title: i18n.tr("Document is locked")
    text: i18n.tr("Please insert a password in order to unlock this document")

    TextField {
        id: passwordField
        width: parent.width

        hasClearButton: true

        Keys.onReturnPressed: tryUnlock()
        onAccepted: tryUnlock()

        Component.onCompleted: forceActiveFocus()

        onTextChanged: {
            if (text.length === 0) {
                errorLabel.visible = false
            }
        }
    }

    Label {
        id: errorLabel
        text: i18n.tr("Entered password is not valid")
        color: UbuntuColors.red
        visible: false
    }

    RowLayout {
        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(-1)
        }

        Button {
            text: i18n.tr("Cancel")
            onClicked: close()
            Layout.fillWidth: true
        }

        Button {
            text: i18n.tr("Unlock")
            color: UbuntuColors.green
            Layout.fillWidth: true

            onClicked: goToPage()
        }
    }

    function close() {
         PopupUtils.close(unlockDialog)
    }

    function tryUnlock() {
        var result = pdfView.unlock(passwordField.text, passwordField.text)

        if (result) {
            close()
        } else {
            errorLabel.visible = true
        }
    }
}
