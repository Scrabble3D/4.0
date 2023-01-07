import QtQuick
import QtQuick.Controls

Button {
    id: control

    icon.color: Color.transparent(isDark(config.myPalette.button)
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
        color: config.myPalette.buttonText
    }

    background: Rectangle {
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: !down ? Qt.lighter(config.myPalette.button, 1.2) : config.myPalette.button
            }
            GradientStop {
                position: 0.2
                color: config.myPalette.button
            }
            GradientStop {
                position: 0.8
                color: !down ? Qt.darker(config.myPalette.button, 1.2) : config.myPalette.button
            }
        }
        border.width: control.hovered ? 2 : 0
        border.color: config.myPalette.button
    }
}
