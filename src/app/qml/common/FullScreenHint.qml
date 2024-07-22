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
 
import QtQuick 2.12
import Lomiri.Components 1.3

Rectangle {
    id: fullscreenExitHint
    objectName: "fullscreenExitHint"

    height: units.gu(6)
    width: Math.min(units.gu(50), parent.width - units.gu(12))
    radius: units.gu(1)
    color: theme.palette.normal.backgroundSecondaryText
    opacity: 0.85

    Behavior on opacity {
        LomiriNumberAnimation {
            duration: LomiriAnimation.SlowDuration
        }
    }
    onOpacityChanged: {
        if (opacity == 0.0) {
            fullscreenExitHint.destroy()
        }
    }

    Label {
        color: theme.palette.normal.background
        font.weight: Font.Light
        fontSizeMode: Text.HorizontalFit
        minimumPixelSize: Label.XSmall
        textSize: Label.Large
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors {
            fill: parent
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }

        text: mainView.hasKeyboard ? i18n.tr("Press ESC to exit presentation mode")
                                    : i18n.tr("Swipe from the bottom edge to exit")
    }

    Timer {
        running: fullscreenExitHint.visible
        interval: 3000
        onTriggered: fullscreenExitHint.opacity = 0
    }

    Connections {
        target: mainView
        onFullscreenChanged: {
            if (!target.fullscreen) {
                fullscreenExitHint.destroy()
            }
        }
    }
}
