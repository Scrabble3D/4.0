import QtQuick
import QtQuick.Controls

Image {
    property string tiptext
    visible: config.showTips
    source: "qrc:///resources/info.png"

    TapHandler {
        onTapped: tip.visible = true
    }
    TextMetrics {
        id: tipMetrics
        font: tip.font
        text: tip.text
    }
    property int tipWidth: 300

    property var bgColor: config.myPalette.ToolTipBase ? config.myPalette.ToolTipBase : "#ffffca"
    property var fgColor: config.myPalette.ToolTipText ? config.myPalette.toolTipText : "#000000"

    ToolTip {
        id: tip
        visible: false
        delay: 0
        timeout: 5000
        text: tiptext

        contentItem: Label {
            color: fgColor
            text: tip.text
            wrapMode: Text.Wrap
        }

        implicitWidth: tipMetrics.width > tipWidth ? tipWidth : tipMetrics.width
                                                     + tip.leftMargin + tip.rightMargin
        background: Rectangle {
            // required on macOS
            color: bgColor
        }
    }
}
