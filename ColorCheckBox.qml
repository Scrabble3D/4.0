import QtQuick
import QtQuick.Controls

// needed for dark mode on Android
CheckBox {
    id: control
    spacing: 6
    contentItem: Text {
        text: control.text
        font: control.font
        color: enabled ? config.myPalette.windowText : config.myPalette.mid
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }

}
