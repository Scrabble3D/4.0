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
        id:     tipMetrics
        font:   tip.font
        text:   tip.text
    }
    property int tipWidth: 300

    ToolTip {
        id: tip
        visible: false
        delay: 0
        timeout: 5000
        text: tiptext
        implicitWidth: tipMetrics.width > tipWidth
                           ? tipWidth
                           : tipMetrics.width + tip.leftMargin + tip.rightMargin
    }
}
