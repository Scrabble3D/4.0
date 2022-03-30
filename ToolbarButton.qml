import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.ToolButton {
    id: control

    SystemPalette {
        id: myPalette
        colorGroup: enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    implicitWidth: 28
    implicitHeight: 28

    padding: 4

    icon.width: 32
    icon.height: 32
    icon.color: Color.transparent(isDark(myPalette.base)
                                  ? "white"
                                  : "black", enabled ? 1.0 : 0.2)

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: Color.transparent(isDark(myPalette.base)
                                 ? "white"
                                 : "black", enabled ? 1.0 : 0.2)
    }

    background: Rectangle {
        id: backRect
        color: control.enabled && control.hovered
                ? control.pressed ? myPalette.dark
                                  : myPalette.mid
                : "transparent"
        border.color: myPalette.dark
        border.width: control.enabled && control.hovered ? 1 : 0
    }
}
