import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.ToolButton {
    id: control

    property int size: 28
    implicitWidth: size
    implicitHeight: size

    padding: 4

    icon.width: size - 6
    icon.height: size - 6
    icon.color: Color.transparent(isDark(palette.base)
                                     ? "white"
                                     : "black",
                                  enabled ? 1.0 : 0.2)

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: Color.transparent(isDark(palette.base)
                                 ? "white"
                                 : "black", enabled ? 1.0 : 0.2)
    }

    background: Rectangle {
        id: backRect
        color: control.enabled && control.hovered
                ? control.pressed ? palette.dark
                                  : palette.mid
                : "transparent"
        border.color: palette.dark
        border.width: control.enabled && control.hovered ? 1 : 0
    }
}
