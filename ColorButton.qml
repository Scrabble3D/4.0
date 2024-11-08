import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

// needed for dark mode on Android
T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)
    padding: 6
    horizontalPadding: padding + 2
    spacing: 6

    icon.width: 24
    icon.height: 24

    function isDark(aColor) {
        var temp = Qt.darker(aColor, 1) //Force conversion to color QML type object
        var a = 1 - ( 0.299 * temp.r + 0.587 * temp.g + 0.114 * temp.b);
        return temp.a > 0 && a >= 0.3
    }

    icon.color: Color.transparent(isDark(palette.button)
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
        color: palette.buttonText
    }

    background: Rectangle {
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: !control.down ? Qt.lighter(control.palette.button, 1.2) : control.palette.button
            }
            GradientStop {
                position: 0.2
                color: control.palette.button
            }
            GradientStop {
                position: 0.8
                color: !control.down ? Qt.darker(control.palette.button, 1.2) : control.palette.button
            }
        }
        border.width: control.hovered ? 2 : 0
        border.color: control.palette.button
    }
}
