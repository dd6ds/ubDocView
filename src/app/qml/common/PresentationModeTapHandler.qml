import QtQuick 2.12
import Lomiri.Components 1.3

Item {
    id: presentationTapHandler

    signal leftClicked
    signal rightClicked

    visible: enabled

    TapHandler {
        enabled: presentationTapHandler.enabled
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad | PointerDevice.Stylus
        onTapped: {
            switch (eventPoint.event.button) {
                case Qt.LeftButton:
                    presentationTapHandler.leftClicked()
                    break
                case Qt.RightButton:
                    presentationTapHandler.rightClicked()
                    break
            }
        }
    }
}
