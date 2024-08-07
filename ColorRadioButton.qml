import QtQuick
import QtQuick.Controls

// needed for dark mode on Android
RadioButton {
    id: control
    spacing: 6
    contentItem: Text {
        text: control.text
        font: control.font
        color: control.enabled ? config.myPalette.windowText : config.myPalette.disabled.windowText
        leftPadding: control.indicator.width + control.spacing
        verticalAlignment: Text.AlignVCenter
    }
}
